#ifndef SLICEVIEW_H
#define SLICEVIEW_H

#include <views/qmygraphicsview.h>
#include <views/view.h>

#include <QWidget>

namespace Ui {
class SliceView;
}

class SliceView : public QWidget
{
    Q_OBJECT

public:
    SliceView(QWidget *parent = nullptr);
    ~SliceView();

    void initializeSlice(int sliceType, SliceView *slice2, SliceView *slice3);
    void initalizeGraphicalView();
    void updateAllGraphicalViews();

    static void updateBrightnessAndConstrast(double f1, double f2, double g1, double g2);

    void showCurrentSlice();
    void updateLines();

    void createActions();
    void destroyActions();

    void set2DVisualizationMode();
    void set3DVisualizationMode();

    void startAnnotation(int mode);

signals:
    void requestGradientCalculation();

public slots:
    void slotZoomIn();
    void slotZoomOut();
    void slotNormalSize();
    void slotToogleLinesVisibility();
    void slotSetGradientVisibility(int state);
private slots:
    void slotUpdateSliceMousePosition(int x, int y);
    void slotReleaseView(int x, int y);
    void slotGraphicalViewClicked(int x, int y, Qt::MouseButtons, Qt::KeyboardModifiers modifiers);
    void slotChangeSliceImage();
    void slotSaveGraphicalView();

    void increaseBrush();
    void decreaseBrush();

private:
    Ui::SliceView *ui;

    double scaleFactor, abs_scaleFactor = 1.0;

    static double f1, f2, g1, g2;

    bool gradientVisibility;

    bool linesVisibility;
    SliceView *horizontalSlice;
    SliceView *verticalSlice;

    QGraphicsLineItem *verticalLine;
    QGraphicsLineItem *horizontalLine;

    QGraphicsPixmapItem *imagePixmapItem;
    QGraphicsTextItem **orientationIndicators;

    QImage *sliceImage;
    int sliceType;
    View *view;

    iftImage *getCurrentRemovalMarkersSlice();

    iftImage *getCurrentSliceImage();
    iftImage *getCurrentNormalizedSliceImage();
    iftImage *getCurrentMarkerSlice();
    iftImage *getCurrentLabelSlice();
    iftImage *getCurrentBorderSlice();
    iftImage *getCurrentGradientSlice();
    int getMaxCurrentSlice();
    int getCurrentSliceNumber();
    void setCurrentSliceNumber(int index);

    void updateAllSlicesLines();
    void placeOrientationIndicators();
    void updateGraphicalView();

    void setIcons();

    void updateCursor();

    QMyGraphicsView *gvSlice();

    friend class MainWindow;
    friend class Segmentation;
};

#endif // SLICEVIEW_H
