#include "threshold.h"
#include "ui_threshold.h"
#include "mainwindow.h"
#include <string>
#include <QToolTip>
#include <QtCharts>

using namespace QtCharts;

Threshold::Threshold(MainWindow *parent, View *view) :
    Segmentation(parent, view),
    ui(new Ui::Threshold)
{
    ui->setupUi(this);
    _name = "Threshold";
    annotationVisible = false;
    connect(ui->pbRun, SIGNAL(clicked()), this, SLOT(execute()));
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

iftImage *Threshold::generateLabel()
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

    return label;
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

