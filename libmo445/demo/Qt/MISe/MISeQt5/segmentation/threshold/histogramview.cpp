#include "histogramview.h"
#include "ui_histogramview.h"

#include <QLineSeries>

HistogramView::HistogramView(const iftImage *img) :
    QWidget(),
    ui(new Ui::HistogramView)
{
    ui->setupUi(this);

    otsuLine = nullptr;

    generateHistogram(img);

    ui->sbMax->setMaximum(maxValue);
    ui->sbMin->setMaximum(maxValue);
    ui->sbMax->setMinimum(0);
    ui->sbMin->setMinimum(0);
    ui->sbMax->setValue(maxValue);
    ui->sbMin->setValue(1);

    plotHistogram();

    connect(ui->sbMax, SIGNAL(valueChanged(int)), this, SLOT(updateInterval()));
    connect(ui->sbMin, SIGNAL(valueChanged(int)), this, SLOT(updateInterval()));
}

HistogramView::~HistogramView()
{
    delete series;
    delete axisX;
    delete axisY;
    delete otsuLine;
    delete ui;
    iftDestroyIntArray(&histogram);
}

void HistogramView::updateInterval()
{
    int start = ui->sbMin->value();
    int end = ui->sbMax->value();

    ui->sbMax->setMinimum(start + 1);
    ui->sbMin->setMaximum(end - 1);

    int maxFrequency = 0;
    for (int i = start; i <= end; i++) {
        if (histogram->val[i] > maxFrequency)
            maxFrequency = histogram->val[i];
    }

    axisX->setRange(start, end);
    axisY->setRange(0.0, maxFrequency);
}

void HistogramView::generateHistogram(const iftImage *img)
{
    maxValue = iftMaximumValue(img);

    histogram = iftCreateIntArray(maxValue + 1);

    // Calculate histogram
    for (int i = 0; i < img->n; i++) {
        histogram->val[img->val[i]]++;
    }

    // Add points
    QLineSeries *series0 = new QLineSeries();
    for (int i = 0; i <= histogram->n; i++) {
        *series0 << QPoint(i, histogram->val[i]);
    }

    // Create the area series
    series = new QAreaSeries(series0);

    // Calculate Otsu's value
    otsuValue = iftOtsu(img);
}

void HistogramView::plotHistogram()
{
    series->setName("Frequency");
    QPen pen(0x059605);
    pen.setWidth(1);
    series->setPen(pen);

    QLinearGradient gradient(QPoint(0, 0), QPoint(0, 1));
    gradient.setColorAt(0, 0x3cc63c);
    gradient.setColorAt(1, 0x26f626);
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    series->setBrush(gradient);

    // Create axes
    axisX = new QValueAxis;
    axisX->setLabelFormat("%.0f");

    axisY = new QValueAxis;
    axisY->setLabelFormat("%.0f");

    // Create chart
    QChart *chart = new QChart();
    chart->setMargins(QMargins(0,0,0,0));
    chart->addSeries(series);
    chart->setTitle("Histogram");
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    series->attachAxis(axisX);
    series->attachAxis(axisY);

    ui->chart->setChart(chart);
    ui->chart->setRenderHint(QPainter::Antialiasing);
    ui->chart->setRubberBand(QChartView::HorizontalRubberBand);

    updateInterval();

    // TODO the following code isnt working
//    QPointF p1 = chart->mapToPosition(QPointF(1024, 0));
//    QPointF p2 = chart->mapToPosition(QPointF(1024, 3400000));

//    otsuLine = ui->chart->scene()->addLine(QLineF(p1, p2));

//    qDebug() << chart->mapToPosition(QPointF(1024, 2000));
}
