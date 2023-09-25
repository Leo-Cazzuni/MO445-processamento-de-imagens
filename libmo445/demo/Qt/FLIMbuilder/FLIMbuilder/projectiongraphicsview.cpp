#include "projectiongraphicsview.h"
#include "graphnode.h"

ProjectionGraphicsView::ProjectionGraphicsView(QWidget *parent): QGraphicsView(parent)
{
    qDebug();

    setMouseTracking(true);
    myTimer.start();
    validStart = false;
    autoReprojection = false;
    projection = dynamic_cast<Projection*>(this->window());
    backgroundColor = QColor(Qt::white);
    sceneRectColor = QColor(Qt::black);

    connect(this, SIGNAL(sceneAreaSizeChanged_signal()), this, SLOT(updateNodesPosition_slot()) );
}

ProjectionGraphicsView::~ProjectionGraphicsView()
{
    qDebug();

    return;
}

void ProjectionGraphicsView::updateNodesPosition_slot()
{
    qDebug();

    iftDataSet *dataset = projection->workingDataSet;
    if(dataset == nullptr || projection->graphNodes.size() <= 0 || projection->projGraphicsScene == nullptr)
        return;

    /* recompute the scene's scaling factor and the position of the nodes */
    computeSceneScalingFactor();

    qreal rx = qreal(pointDefaultRx);
    qreal ry = qreal(pointDefaultRy);
    qreal factor = projection->sceneScalingFactor - rx*4;

    for (int i = 0; i < projection->graphNodes.size(); i++) {
        qreal x = (dataset->projection->val[i*2])*factor + rx*2;
        qreal y = (dataset->projection->val[i*2 + 1])*factor + ry*2;
        GraphNode* node = projection->graphNodes.at(i);
        node->setPos(x, y);
        node->update();
    }
}

void ProjectionGraphicsView::computeSceneScalingFactor()
{
    qDebug();

    float rx = pointDefaultRx;
    float ry = pointDefaultRy;
    float scalingFactor_width = (this->size().width()-(rx*5));
    float scalingFactor_height = (this->size().height()-(ry*5));

    if(scalingFactor_height < scalingFactor_width)
        projection->sceneScalingFactor = int(scalingFactor_height+ ry*4);
    else
        projection->sceneScalingFactor = int(scalingFactor_width + rx*4);


}

void ProjectionGraphicsView::wheelEvent(QWheelEvent *event)
{
    qDebug();

    QPoint rotation_degrees = event->angleDelta() / 120;

    if (rotation_degrees.isNull())
        return;

    if (rotation_degrees.y() > 0){
        scaleView(pow(2, -rotation_degrees.y()));
    } else {
        scaleView(pow(2, -rotation_degrees.x()));
    }
}

void ProjectionGraphicsView::scaleView(qreal scaleFactor)
{
    qDebug();

    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.95 || factor > 10)
        return;

    scale(scaleFactor, scaleFactor);
}

void ProjectionGraphicsView::drawBackground(QPainter *painter, const QRectF &rect)
{
    qDebug();

    Q_UNUSED(rect);

    /* draw a white background */
    QRectF sceneRect = this->sceneRect();
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, backgroundColor);
    gradient.setColorAt(1, backgroundColor);
    painter->fillRect(rect.intersected(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);

    painter->setPen(sceneRectColor);
    painter->drawRect(sceneRect);
}

void ProjectionGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    nMilliseconds = int(myTimer.elapsed());
    MouseX_new = event->pos().x();
    MouseY_new = event->pos().y();

    QList<QGraphicsItem *> gItems = items(MouseX_old, MouseY_old);
    if(gItems.size()>0)
        for(int index=gItems.size()-1; index>=0; index--){ // it returns the last item
            GraphNode* node = dynamic_cast<GraphNode*>(gItems.at(index));
            if(node){
                if(gItems.at(index) != nullptr && gItems.at(index) != lastItem) {
                    /* if the mouse pointer is in the same place during a given time */
                    if(nMilliseconds>totalTimeReaction){
                        node->numberTimesChecked += 1;
                        lastItem = gItems.at(index);
                    }
                }
            }
        }

    myTimer.restart();
    MouseX_old = MouseX_new;
    MouseY_old = MouseY_new;
    QGraphicsView::mouseMoveEvent(event);
}

void ProjectionGraphicsView::mousePressEvent(QMouseEvent *event)
{
    qDebug();

    if (event->button() == Qt::RightButton) {
        event->accept();
        return;
    }
    QGraphicsView::mousePressEvent(event);

}

void ProjectionGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug();

    QGraphicsView::mouseReleaseEvent(event);
    projection->updateNumbSamplesShown();
}

void ProjectionGraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    qDebug();

    QGraphicsView::mouseDoubleClickEvent(event);
}

void ProjectionGraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    qDebug();

    emit mouseContextMenuRequest_signal(event);
}

void ProjectionGraphicsView::keyPressEvent(QKeyEvent *event)
{
    qDebug();

    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    // it allows the zoom in and out by using
    if(keyEvent->key() == Qt::Key_Up)
        scaleView(pow(2, +0.5));
    if(keyEvent->key() == Qt::Key_Down)
        scaleView(pow(2, -0.5));

    QWidget::keyPressEvent(event);
}

void ProjectionGraphicsView::updateSceneAutoReprojection_slot(bool autoReproject)
{
    qDebug();

    autoReprojection = autoReproject;
}

void ProjectionGraphicsView::resizeEvent(QResizeEvent* event)
{
    qDebug();

    /* update the scene's size */
    if(autoReprojection == true){
        int minDimension = (event->size().height() < event->size().width())? event->size().height() : event->size().width();
        scene()->setSceneRect(0,0,minDimension,minDimension);
        emit sceneAreaSizeChanged_signal();
    }

    QGraphicsView::resizeEvent(event);
}

void ProjectionGraphicsView::paintNodes()
{
    qDebug();

    /* paint the nodes in the scene accoding to the chosen option */
    //if(projection->currentNodeColorOption == "True class"){
    //    paintNodesByTrueClass();
    //}else
    if(projection->currentNodeColorOption == "Weights"){
        paintNodesByWeight();
    }else if(projection->currentNodeColorOption == "DICE"){
        paintNodesByDICE();
    }

    projection->projGraphicsScene->update();

}

void ProjectionGraphicsView::paintNodesByTrueClass()
{
    qDebug();

    if(projection->hashSampleId2GraphNodeRef.size() <= 0)
        return;

    iftDataSet *dataset = projection->workingDataSet;
    for (ulong i = 0; i < ulong(dataset->nsamples); i++) {
        GraphNode* node = projection->hashSampleId2GraphNodeRef[i];
        if(node == nullptr)
            continue;

        if(iftHasSampleStatus(dataset->sample[i], IFT_SUPERVISED)) {
            QColor color;
            if (dataset->sample[i].truelabel == 0)
                color = projection->hashMarkerName2MarkerColor["Background"];
            else
                color = projection->hashMarkerName2MarkerColor[QString("Marker %1").arg(dataset->sample[i].truelabel+1)];
            node->fillColor_original = color;
            node->sampleText = QString("%1").arg(dataset->sample[i].truelabel);
        } else {
            QColor color(0,0,0,255);
            node->fillColor_original = color;
            node->sampleText = "";
        }
        node->isMarked = false;
        // targetClass of -1 indicates that all classes must be shown
        if (this->targetClass > -1){
            if (node->samplePtr->truelabel != this->targetClass){
                node->setVisible(false);
            }else {
                node->setVisible(true);
            }
        } else {
            node->setVisible(true);
        }
    }
}

void ProjectionGraphicsView::paintNodesByWeight()
{
    qDebug();

    if(projection->hashSampleId2GraphNodeRef.size() <= 0 )
        return;

    if(projection->kernelimportance == nullptr)
        return;

    if (projection->workingDataSet == nullptr)
        return;

    iftDataSet *dataset = projection->workingDataSet;
    iftColorTable *ctb = nullptr, *ctb2 = nullptr;
    ctb2 = iftBlueToRedColorTable(256);
    ctb  = iftConvertYCbCrColorTableToRGBColorTable(ctb2, 255);

    for (ulong i = 0; i < ulong(dataset->nsamples); ++i) {
        GraphNode* node = projection->hashSampleId2GraphNodeRef[i];
        if(node == nullptr)
            continue;

        float weight = dataset->sample[i].weight;
        qDebug() << weight;
        if (weight < 0)
            weight = 0;
        int idx = int(weight*255);
        QColor color = QColor(ctb->color[idx].val[0], ctb->color[idx].val[1],ctb->color[idx].val[2]);
        node->fillColor_original = color;
        node->sampleText = QString("%1").arg(dataset->sample[i].truelabel);
        node->isMarked = false;
    }
    iftDestroyColorTable(&ctb);
    iftDestroyColorTable(&ctb2);
}

void ProjectionGraphicsView::paintNodesByDICE()
{
    qDebug();

    if(projection->hashSampleId2GraphNodeRef.size() <= 0 )
        return;

    if(projection->DICEimportance == nullptr)
        return;

    if (projection->workingDataSet == nullptr)
        return;

    iftDataSet *dataset = projection->workingDataSet;
    iftColorTable *ctb = nullptr, *ctb2 = nullptr;
    ctb2 = iftBlueToRedColorTable(256);
    ctb  = iftConvertYCbCrColorTableToRGBColorTable(ctb2, 255);

    for (ulong i = 0; i < ulong(dataset->nsamples); ++i) {
        GraphNode* node = projection->hashSampleId2GraphNodeRef[i];
        if(node == nullptr)
            continue;

        float weight = dataset->sample[i].weight;
        if (weight < 0)
            weight = 0;
        int idx = int(weight*255);
        QColor color = QColor(ctb->color[idx].val[0], ctb->color[idx].val[1],ctb->color[idx].val[2]);
        node->fillColor_original = color;
        node->sampleText = QString("%1").arg(dataset->sample[i].truelabel);
        node->isMarked = false;
    }
    iftDestroyColorTable(&ctb);
    iftDestroyColorTable(&ctb2);
}
