#include "graphnode.h"
#include "graphedge.h"
#include "projection.h"

GraphNode::GraphNode(iftSample *sampPtr, int numFeats)
{
    radiusX_highlight = pointDefaultRx*3;
    radiusY_highlight = pointDefaultRy*3;
    radiusX_unhighlight = pointDefaultRx;
    radiusY_unhighlight = pointDefaultRy;

    radiusX = radiusX_unhighlight;
    radiusY = radiusY_unhighlight;

    obj_boundingRect.setX(int(-radiusX));
    obj_boundingRect.setY(int(-radiusY));
    obj_boundingRect.setWidth(int(2*radiusX));
    obj_boundingRect.setHeight(int(2*radiusY));

    samplePtr = sampPtr;
    nFeats = numFeats;

    signalhandler = new SignalHandler();
    fillColor = QColor(255,0,0,180);
    borderColor_unselected = QColor(Qt::black);
    borderColor_selected = QColor(Qt::cyan);
    sampleImagePath = "";
    thumbnailImagePath = "";
    sampleName = "";
    Z = -1;
    drawOption = optionPoint;
    fontSizeScalingFactor = 2;
    borderColor_supervised = QColor(Qt::magenta);
    borderColor_propagated = QColor(Qt::yellow);
    fontSize = 0.8;
    isMarked = false;
    edgeMode = EdgeMode_None;
    numberTimesChecked = 0;

    isHovered = false;
    setAcceptHoverEvents(true);
    shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setBlurRadius(50);
    shadowEffect->setOffset(4);
    setGraphicsEffect(shadowEffect);
    drawShadowInPoints = false;
    isSelectedByClassOrGroup = false;

    computeFeatPerplexity();
    computeFeatMean();
    computeFeatStdev();
    computeFeatWeight();
}

GraphNode::~GraphNode()
{
    delete signalhandler;
    return;
}

void GraphNode::computeFeatPerplexity()
{
    float entr = 0.0;
    bool valid = true;
    for (int i = 0; i < nFeats && valid; i++)
    {
        double x = double(samplePtr->feat[i]);
        if (!iftAlmostZero(x) && x > 0)
            entr += float(log2(x));
        else {
            valid = false;
        }
    }

    if(valid)
        this->featPerplexity = float(pow(2, double(-entr/nFeats)));
    else
        this->featPerplexity = -1;

}

void GraphNode::computeFeatMean()
{
    featMean = iftMean(this->samplePtr->feat, this->nFeats);
}

void GraphNode::computeFeatStdev()
{
    featStdev = iftStd(this->samplePtr->feat, this->nFeats);
}

void GraphNode::computeFeatWeight()
{
    featWeight = this->samplePtr->weight;
}

QRectF GraphNode::boundingRect() const
{
    return obj_boundingRect;
}

void GraphNode::mountToolTip()
{
    QString text;
    /* add the sampleName, the features and the extra info */
    text = "-> " + sampleName;
    text += "\n-> weight: " + (!iftAlmostZero(double(featWeight+1)) ? QString::number(double(featWeight), 'f', 4) : "-1.00 (not defined)");
    text += "\n-> perplexity: " + (!iftAlmostZero(double(featPerplexity+1)) ? QString::number(double(featPerplexity), 'f', 4) : "-1.00 (not defined)"); //featPerplexity != -1
    text += "\n-> mean: " + QString::number(double(featMean), 'f', 4);
    text += "\n-> stdev: " + QString::number(double(featStdev), 'f', 4);
    text += "\n-> feats: ";
    int nMaxFeats = 20;

    if(nFeats < nMaxFeats) {
        for(int f = 0; f < nFeats; f++) {
            text += QString::number(double(samplePtr->feat[f]), 'f', 2);
            if(f < nFeats-1) {
                if((f+1) % 5 == 0)
                    text += "\n    ";
                else
                    text += ", ";
            }
        }
    }
    else {
        for(int f = 0; f < nMaxFeats; f++) {
            text += QString::number(double(samplePtr->feat[f]), 'f', 2);
            if(f < nMaxFeats-1)
                if((f+1) % 5 == 0)
                    text += "\n    ";
                else
                    text += ", ";
            else
                text += ", ...";
        }
    }

    setToolTip(text);
}

void GraphNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    qreal diameterX = qreal(2*radiusX);
    qreal diameterY = qreal(2*radiusY);
    int alphaValue = 150; //200

    if(isSelectedByClassOrGroup)
        alphaValue = 20;

    if(drawOption == optionPoint || drawOption == optionDrawingEdges){

        QPen pen;

        setZValue(Z);
        fillColor = fillColor_original;
        fillColor.setAlpha(alphaValue);
        pen.setColor(fillColor);
        pen = QPen(QColor(0,0,0));

        if(isSelected()){
            borderColor_selected.setAlpha(255);
            pen = QPen(borderColor_selected, 0.); //0.8
            fillColor = fillColor.lighter(180);
        }

        QBrush brush(fillColor);
        painter->setBrush(brush);
        painter->setPen(pen);
        painter->drawEllipse(int(-radiusX), int(-radiusY), int(diameterX), int(diameterY));
    }
    else if(drawOption == optionText) {
        QPen pen;
        QFont font = painter->font();
        int boundBoxCeilFactor = 0;
        fillColor = fillColor_original;
        if(isSelected()){
            font.setBold(true);
            font.setItalic(true);
            font.setUnderline(true);
            fillColor.setAlpha(alphaValue);
            borderColor_selected.setAlpha(alphaValue);
        }

        if(isMarked == true){
            pen = QPen(QColor(0,0,0,alphaValue), 0);
            painter->setPen(pen);
            painter->drawEllipse(int(-radiusX), int(-radiusY), int(diameterX), int(diameterY));
        }

        font.setPointSize(int(diameterX/1.3));
        painter->setFont(font);
        pen = QPen(fillColor, fontSize);
        painter->setPen(pen);
        QRect boundingRect;
        painter->drawText(int(-radiusX), int(-radiusY), int(diameterX), int(diameterY), Qt::AlignCenter, QString::number(samplePtr->id), &boundingRect);
        obj_boundingRect = boundingRect.adjusted(0, 0, -pen.width(), -pen.width());
        boundBoxCeilFactor = int(double(radiusY)/3.1);
        obj_boundingRect.setY(obj_boundingRect.y()+boundBoxCeilFactor);
        obj_boundingRect.setHeight(obj_boundingRect.height()-boundBoxCeilFactor);
    }

/*
    if(displayNodeDataAsPointBorder) {
        QColor color(Qt::black);

        float val=1.0;
        if(currentNodeDataMode == "Perplexity") {
            val = float(1.0) - (featPerplexity - minGlobalFeatPerplexity) / (maxGlobalFeatPerplexity - minGlobalFeatPerplexity);
        }
        else if(currentNodeDataMode == "Mean") {
            val = float(1.0) - (featMean - minGlobalFeatMean) / (maxGlobalFeatMean - minGlobalFeatMean);
        }
        else if(currentNodeDataMode == "Stdev") {
            val = float(1.0) - (featStdev - minGlobalFeatStdev) / (maxGlobalFeatStdev - minGlobalFeatStdev);
        }
        else if(currentNodeDataMode == "Weight") {
            val = float(1.0) - (featWeight - minGlobalFeatWeight) / (maxGlobalFeatWeight - minGlobalFeatWeight);
        }

        qreal h,s,v;
        color.getHsvF(&h, &s, &v);
        color.setHsvF(h, s, qreal(val));
        color.setAlpha(alphaValue);
        QPen pen = QPen(color, 2.0);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(int(-radiusX), int(-radiusY), int(diameterX), int(diameterY));

    }*/

    graphicsEffect()->setEnabled(drawShadowInPoints);
}

void GraphNode::hoverEnterEvent(){
    isHovered=true;
    update();
}

void GraphNode::hoverLeaveEvent(){
    isHovered=false;
    update();
}


void GraphNode::setMaxGlobalFeatPerplexity(float val)
{
    maxGlobalFeatPerplexity = val;
}

void GraphNode::setMinGlobalFeatPerplexity(float val)
{
    minGlobalFeatPerplexity = val;
}

void GraphNode::setMaxGlobalFeatMean(float val)
{
    maxGlobalFeatMean = val;
}

void GraphNode::setMinGlobalFeatMean(float val)
{
    minGlobalFeatMean = val;
}

void GraphNode::setMaxGlobalFeatStdev(float val)
{
    maxGlobalFeatStdev = val;
}

void GraphNode::setMinGlobalFeatStdev(float val)
{
    minGlobalFeatStdev = val;
}


void GraphNode::setMaxGlobalFeatWeight(float val)
{
    maxGlobalFeatWeight = val;
}

void GraphNode::setMinGlobalFeatWeight(float val)
{
    minGlobalFeatWeight= val;
}


void GraphNode::setDisplayNodeDataAsPointBorder(bool val)
{
    displayNodeDataAsPointBorder = val;
}

void GraphNode::setCurrentNodeDataMode(QString val)
{
    currentNodeDataMode = val;
}

void GraphNode::setDrawShadowInPoints(bool val)
{
    drawShadowInPoints = val;
}

void GraphNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton){
        emit(signalhandler->doubleClicked(this->samplePtr->id));
    }
}
