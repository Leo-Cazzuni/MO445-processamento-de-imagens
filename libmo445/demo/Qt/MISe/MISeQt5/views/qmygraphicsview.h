#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include "global.h"
#include "mainwindow.h"

class QMyGraphicsView: public QGraphicsView
{
    Q_OBJECT

public:    
    explicit QMyGraphicsView(QWidget *parent = 0);
    explicit QMyGraphicsView(QGraphicsScene * scene, QWidget * parent = 0);
    ~QMyGraphicsView();

signals:
    void positionChanged(int x, int y);
    void clicked(int x, int y, Qt::MouseButtons bts, Qt::KeyboardModifiers modifiers);
    void signalRelease(int x, int y);
    void zoomIn();
    void zoomOut();
    void increaseBrush();
    void decreaseBrush();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
};

#endif // MYGRAPHICSVIEW_H
