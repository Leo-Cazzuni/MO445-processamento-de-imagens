#ifndef QRENDERINGGRAPHICSVIEW_H
#define QRENDERINGGRAPHICSVIEW_H

#include "global.h"
#include "mainwindow.h"

class QRenderingGraphicsView: public QGraphicsView
{
    Q_OBJECT

public:
    explicit QRenderingGraphicsView(QWidget *parent = 0);
    explicit QRenderingGraphicsView(QGraphicsScene * scene, QWidget * parent = 0);
    ~QRenderingGraphicsView();

signals:
    void angleChanged(float tilt, float spin);
    void modeChanged(char mode);
    void renderClick(int x, int y);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    iftVoxel initialCoord;
    float tilt = 0, spin = 0;
    float spinStride = 5.0;
    float tiltStride = 5.0;
};

#endif // QRENDERINGGRAPHICSVIEW_H
