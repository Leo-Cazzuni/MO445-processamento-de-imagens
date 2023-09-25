#include "qrenderinggraphicsview.h"

QRenderingGraphicsView::QRenderingGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    setMouseTracking(true);
}

QRenderingGraphicsView::QRenderingGraphicsView(QGraphicsScene *scene, QWidget *parent) :
    QGraphicsView(scene, parent)
{
    setMouseTracking(true);
}

QRenderingGraphicsView::~QRenderingGraphicsView()
{

}

void QRenderingGraphicsView::mouseMoveEvent(QMouseEvent *event){

    if (event->buttons() == Qt::RightButton){
        spin = (initialCoord.x - event->pos().x());
        tilt = -(initialCoord.y - event->pos().y());

        initialCoord.x = event->pos().x();
        initialCoord.y = event->pos().y();

        emit modeChanged(FAST_MODE);
        emit angleChanged(tilt,spin);
    }
}

void QRenderingGraphicsView::mousePressEvent(QMouseEvent *event){
    if (event->button() == Qt::RightButton){
        initialCoord.x = event->pos().x();
        initialCoord.y = event->pos().y();
    } else if (event->button() == Qt::LeftButton){
        QPointF p = mapToScene(event->pos());
        emit renderClick(p.x(),p.y());
    }
}

void QRenderingGraphicsView::mouseReleaseEvent(QMouseEvent *event){
    if (event->button() == Qt::RightButton){
        emit modeChanged(QUALITY_MODE);
    }
}
