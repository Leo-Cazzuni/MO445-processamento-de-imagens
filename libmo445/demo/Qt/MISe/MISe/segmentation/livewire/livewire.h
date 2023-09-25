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

    void renderGraphics(iftImage *orig, iftImage *image, QGraphicsPixmapItem *imagePixmap, int sliceType) override;
    bool mouseClickGraphics(int x, int y, Qt::MouseButtons bts, Qt::KeyboardModifiers modifiers, int sliceType) override;
    bool mouseMoveGraphics(int x, int y, int sliceType) override;

private slots:
    void start();
    void reset();
    void undo();
    void done();

private:
    Ui::LiveWire *ui;

    bool started;
    const iftImage *saliencyImage;
    iftImage *imageTmp;
    iftImage *currentSlice;
    int currentSliceNumber;
    LivewireOrientation currentOrientation;
    iftVoxel lastPointClicked;
    iftVoxel firstPointClicked;

    void paintPath(int *path);

    // Segmentation interface
protected:
    QList<iftImage*> generateLabel() override;

    QGraphicsPixmapItem *imagePixmap[4];
    iftImage *origRenderImage[4];
    iftImage *origImage[4];
    iftImage *tmpRenderImage[4];

    ArcWeightFunction *currentArcWeightFunction = nullptr;

    int *path;
    QVector<int*> segments;
    int kpoints;
    int slice;

    // Segmentation interface
public:
    void notifyGradientUpdate(ArcWeightFunction *function) override;
};

#endif // LIVEWIRE_H
