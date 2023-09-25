#include "threshold.h"
#include "ui_threshold.h"
#include "mainwindow.h"
#include <string>
#include <QToolTip>
#include <QtCharts>
#include <views/slice/sliceview.h>

Threshold::Threshold(MainWindow *parent, View *view) :
    Segmentation(parent, view),
    ui(new Ui::Threshold)
{
    ui->setupUi(this);
    _name = "Threshold";
    annotationVisible = false;
    connect(ui->pbRun, SIGNAL(clicked()), this, SLOT(execute()));
    connect(ui->sbMax, SIGNAL(valueChanged(int)), this, SLOT(preview()));
    connect(ui->sbMin, SIGNAL(valueChanged(int)), this, SLOT(preview()));
    connect(ui->pbDisplayHistogram, SIGNAL(clicked()), this, SLOT(displayHistogram()));
    histogram = nullptr;
}

void Threshold::showEvent(QShowEvent *event) {
    Segmentation::showEvent(event);

    if (view->getImage() != nullptr) {
        ui->sbMax->setMaximum(iftMaximumValue(view->getImage()));
    }
}

Threshold::~Threshold()
{
    delete ui;
    if (histogram != nullptr)
        delete histogram;
}

QList<iftImage*>Threshold::generateLabel()
{
    if (!view->isLabelEmpty()){
        parent->destroyObjectTableActions();
        view->destroyLabel();
    }
    updateProgress(0.2, "Thresholding.");

    const iftImage *img   = view->getImage();
    int max         = ui->sbMax->value();
    int min         = ui->sbMin->value();
    iftImage *label = iftThreshold(img, min, max, 1);

    updateProgress(0.6);

    return {label};
}

void Threshold::renderGraphics(iftImage *orig, iftImage *image, QGraphicsPixmapItem *imageMap, int sliceType)
{
    origImage[sliceType] = orig;
    origRenderImage[sliceType] = image;
    imagePixmap[sliceType] = imageMap;
}

void Threshold::displayHistogram()
{
    if (histogram != nullptr)
        delete histogram;

    // Show histogram
    histogram = new HistogramView(view->getImage());
    histogram->setWindowFlags(Qt::WindowStaysOnTopHint);
    histogram->show();

//    Histogram *h2 = new Histogram(view->getImage());
    //    h2->show();
}

void Threshold::preview()
{
    iftColor yCbCr = iftRGBtoYCbCr({{255,0,0},1}, 255);
    for (int i = AXIAL; i <= SAGITTAL; i++) {
        int max         = ui->sbMax->value();
        int min         = ui->sbMin->value();
        if (origImage[i]) {
            iftImage *label = iftThreshold(origImage[i], min, max, 1);
            iftImage *tmp   = iftCopyImage(origRenderImage[i]);
            #pragma omp parallel for
            for (int p = 0; p < label->n; p++) {
                if (label->val[p]) {
                    iftSetYCbCr(tmp, p, yCbCr);
                }
            }
            SliceView::updateSlicePixmap(imagePixmap[i], tmp);
            iftDestroyImage(&tmp);
        }
    }
}

