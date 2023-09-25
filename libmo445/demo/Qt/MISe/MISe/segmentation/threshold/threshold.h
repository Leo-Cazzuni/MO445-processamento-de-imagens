#ifndef THRESHOLD_H
#define THRESHOLD_H

#include <QWidget>
#include "../segmentation.h"
#include "histogramview.h"

namespace Ui {
class Threshold;
}

class Threshold : public Segmentation
{
    Q_OBJECT

public:
    explicit Threshold(MainWindow *parent, View *view);
    ~Threshold();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void displayHistogram();
    void preview();

private:
    QList<iftImage*> generateLabel() override;

    Ui::Threshold *ui;
    HistogramView *histogram;

    QGraphicsPixmapItem *imagePixmap[4] = {NULL};
    iftImage *origRenderImage[4] = {NULL};
    iftImage *origImage[4] = {NULL};

    // Segmentation interface
public:
    void renderGraphics(iftImage *orig, iftImage *image, QGraphicsPixmapItem *imagePixmap, int sliceType) override;
};

#endif // THRESHOLD_H
