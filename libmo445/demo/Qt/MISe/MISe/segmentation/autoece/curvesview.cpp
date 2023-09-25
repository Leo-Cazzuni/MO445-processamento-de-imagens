#include "curvesview.h"
#include "ui_curvesview.h"
#include <QLineSeries>
#include <QtCharts>
#include <QChartView>

#include <views/view.h>

CurvesView *CurvesView::_instance = nullptr;

CurvesView::CurvesView() :
    QWidget(NULL),
    ui(new Ui::CurvesView)
{
    ui->setupUi(this);

}

CurvesView *CurvesView::getInstance()
{
    if (_instance == nullptr) {
        _instance = new CurvesView();
    }
    return _instance;
}

CurvesView::~CurvesView()
{
    delete ui;
}

void CurvesView::initializeParameters(QVector<int> time_sec, float **mean_intensities, int n_final_labels)
{
    this->time_sec=time_sec;
    int tsize = time_sec.size();

    series.clear();
    iftColorTable* colors= View::instance()->getColorTable();


    for(int i = 0; i < n_final_labels;i++){
        series.append(new QLineSeries());
        for(int t = 0; t < tsize; t++){
            series[i]->append(time_sec[t], mean_intensities[i][t]);
        }

        QColor color = iftColorToQColor(colors->color[i]);
        series[i]->setColor(color);

     }



     connect(ui->hsSelectedCurves,SIGNAL(valueChanged(int)),this, SLOT(changeValueSliderCurve(int)));

     ui->hsSelectedCurves->setMaximum(n_final_labels);

     PlotCurves(0);
}


void CurvesView::PlotCurves(int currentCurve)
{
//

    int tsize = time_sec.size();
    QChart* chart = new QChart();


    QValueAxis *axisX = new QValueAxis();
    QValueAxis *axisY = new QValueAxis();


    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    chart->setTitle(QString::asprintf("ECE Curves"));
       // chart->legend()->setVisible(true);
        chart->legend()->show();

    int i=1;
    if(currentCurve==0){
        for(QLineSeries *ith_series : series) {
                 chart->addSeries(ith_series);
                 ith_series->attachAxis(axisX);
                 ith_series->attachAxis(axisY);
                 ith_series->setPointsVisible(true);
                 ith_series->setPointLabelsVisible(false);
                 chart->legend()->markers(ith_series)[0]->setLabel(QString("Curve %1").arg(QString::number(i)));
                 i++;
            }
    }
    else{
        chart->addSeries(series[currentCurve-1]);
        series[currentCurve-1]->attachAxis(axisX);
        series[currentCurve-1]->attachAxis(axisY);
        series[currentCurve-1]->setPointLabelsVisible();
        series[currentCurve-1]->setPointsVisible(true);
        chart->legend()->markers(series[currentCurve-1])[0]->setLabel(QString("Curve %1").arg(QString::number(currentCurve)));
    }


    axisX->setTitleText("Time(sec)");
    axisX->setRange(0,time_sec[tsize-1]);
    axisX->setTickCount(10);
    axisX->gridVisibleChanged(true);
    axisX->setLabelFormat("%d");

    axisY->setTitleText("Mean Intensity");
    axisY->setRange(0,350);
    axisY->setTickCount(8);
    axisY->gridVisibleChanged(true);
    axisY->setLabelFormat("%d");
    axisY->tickInterval();


    ui->graphicsView->setChart(chart);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setRubberBand(QChartView::NoRubberBand);
    ui->graphicsView->update();

}


void CurvesView::changeValueSliderCurve(int sliderCurrentValue)
{
    PlotCurves(sliderCurrentValue);

}

void CurvesView::changeCurveVisibility(int curveSelected, bool visibility)
{
   if (curveSelected < series.size()) {
    series[curveSelected]->setVisible(visibility);
   }
}

void CurvesView::removeCurve(int curveSelected)
{
    if (curveSelected < series.size()){

        ui->graphicsView->chart()->removeSeries(series[curveSelected]);
        series.remove(curveSelected);
        ui->hsSelectedCurves->setMaximum(series.size());
    }
}

