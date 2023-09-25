#ifndef LIVEWIRE_H
#define LIVEWIRE_H

#include <QWidget>

#include <segmentation/segmentation.h>

#include <segmentation/gradient/saliency/saliency.h>

namespace Ui {
class LiveWire;
}

enum LivewireOrientation {
    NONE,
    CLOCKWISE,
    COUNTERCLOCKWISE
};

class LiveWire : public Segmentation
{
    Q_OBJECT

public:
    explicit LiveWire(MainWindow *parent, View *view);
    ~LiveWire();

    void renderGraphics(QImage *image, QGraphicsPixmapItem *imagePixmap, int sliceType) override;
    bool mouseClickGraphics(int x, int y, Qt::MouseButtons bts, Qt::KeyboardModifiers modifiers, int sliceType) override;
    bool mouseMoveGraphics(int x, int y, int sliceType) override;

private slots:
    void start();
    void reset();
    void done();
    void changeOrientationMethod();

private:
    Ui::LiveWire *ui;

    bool started;
    Saliency *saliency;
    const iftImage *imageTmp;
    iftImage *currentSlice;
    int currentSliceNumber;
    LivewireOrientation currentOrientation;
    iftVoxel lastPointClicked;
    iftVoxel firstPointClicked;


    // Segmentation interface
protected:
    iftImage *generateLabel() override;

    QGraphicsPixmapItem *imagePixmap[4];
    QImage *origRenderImage[4];
    QImage *tmpRenderImage[4];

    int *path;
    int kpoints;
    int slice;

    // Segmentation interface
public:
    void notifyGradientUpdate(ArcWeightFunction *function) override;
};

#endif // LIVEWIRE_H
