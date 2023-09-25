#ifndef RENDERINGVIEW_H
#define RENDERINGVIEW_H

#include <QTableWidget>
#include <QWidget>

#include <views/view.h>

namespace Ui {
class RenderingView;
}

class RenderingView : public QWidget
{
    Q_OBJECT

public:
    explicit RenderingView(QWidget *parent = nullptr);
    ~RenderingView();

    void updateBrightnessAndConstrast(double f1, double f2, double g1, double g2);

    void updateRendition();

    void createActions();
    void destroyActions();
public slots:
    void slotZoomIn();
    void slotZoomOut();
    void slotNormalSize();
private slots:
    void slotUpdateRenderingAngle(float tilt, float spin);
    void slotUpdateRenderingMode(char mode);
    void slotRenderingClicked(int x,int y);
    void slotReleaseView();
    void slotDrawWireframe();
    void slotDrawPlanes();
    void slotSaveProjection();

signals:
    void updatedGraphicsViews();

private:
    Ui::RenderingView *ui;

    // TODO refactor to a super class
    double scaleFactor, abs_scaleFactor = 1.0;
    double f1, f2, g1, g2;

    View *view;

    QImage *projection;
    QGraphicsPixmapItem *projectionPixmap;

    void showProjection();

    void setIcons();
};

#endif // RENDERINGVIEW_H
