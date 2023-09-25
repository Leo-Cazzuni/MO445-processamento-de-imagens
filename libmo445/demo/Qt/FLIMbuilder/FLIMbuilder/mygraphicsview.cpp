#include "mygraphicsview.h"
#include <QMouseEvent>

MyGraphicsView::MyGraphicsView(QWidget *parent):
    QGraphicsView(parent)
{
    setMouseTracking(true);
}

MyGraphicsView::~MyGraphicsView()
{

}

void MyGraphicsView::mousePressEvent(QMouseEvent *event)
{
    QPointF p = mapToScene(event->pos());
    if (event->buttons() == Qt::LeftButton){
        emit printMarker(p.toPoint().x(),p.toPoint().y());
    } else if (event->buttons() == Qt::RightButton){
        emit eraseMarker(p.toPoint().x(),p.toPoint().y());

    }
}

void MyGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF p = mapToScene(event->pos());
    if (event->buttons() == Qt::LeftButton){
        emit printMarker(p.toPoint().x(),p.toPoint().y());
    } else if (event->buttons() == Qt::RightButton){
        emit eraseMarker(p.toPoint().x(),p.toPoint().y());
    }
    emit showIntensity(p.toPoint().x(),p.toPoint().y());
}

void MyGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if ((event->button() == Qt::LeftButton) || (event->button() == Qt::RightButton)){
        emit released();
    }
}

void MyGraphicsView::wheelEvent(QWheelEvent *event)
{
    QPoint rotation_degrees = event->angleDelta() / 120;

    if (rotation_degrees.isNull())
        return;

    if (rotation_degrees.y() > 0){
        emit forwardSlice(rotation_degrees.y());
    } else {
        emit backwardSlice(rotation_degrees.y());
    }
}
