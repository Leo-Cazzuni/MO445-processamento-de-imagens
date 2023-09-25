#include "qmygraphicsview.h"

QMyGraphicsView::QMyGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    setMouseTracking(true);
}

QMyGraphicsView::QMyGraphicsView(QGraphicsScene *scene, QWidget *parent) :
    QGraphicsView(scene, parent)
{
    setMouseTracking(true);
}

QMyGraphicsView::~QMyGraphicsView()
{

}

void QMyGraphicsView::mouseMoveEvent(QMouseEvent *event){
    QPointF p = mapToScene(event->pos());
    // XOR
    if ((event->buttons() != Qt::LeftButton) != (event->buttons() != Qt::RightButton)){
        emit clicked(p.x(),p.y(), event->buttons(), event->modifiers());
    } else {
        emit positionChanged(p.x(),p.y());
    }
}

void QMyGraphicsView::mousePressEvent(QMouseEvent *event){
    QPointF p = mapToScene(event->pos());
    // XOR
    if ((event->buttons() != Qt::LeftButton) != (event->buttons() != Qt::RightButton) || event->buttons() == Qt::MiddleButton){
        emit clicked(p.x(),p.y(), event->buttons(), event->modifiers());
    }
}

void QMyGraphicsView::mouseReleaseEvent(QMouseEvent *event){
    QPointF p = mapToScene(event->pos());
    //if (event->buttons() & (Qt::LeftButton | Qt::RightButton)){
        emit signalRelease(p.x(),p.y());
    //}
}

void QMyGraphicsView::wheelEvent(QWheelEvent *event) {
    if (event->modifiers().testFlag(Qt::ControlModifier)) {
        if (event->angleDelta().y() > 0) {
            emit zoomIn();
        } else if (event->angleDelta().y() < 0){
            emit zoomOut();
        }
    } else if (event->modifiers().testFlag(Qt::ShiftModifier)) {
        if (event->angleDelta().y() > 0) {
            emit increaseBrush();
        } else if (event->angleDelta().y() < 0){
            emit decreaseBrush();
        }
    } else {
        QGraphicsView::wheelEvent(event);
    }
}
