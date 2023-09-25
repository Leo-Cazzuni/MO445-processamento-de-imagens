#ifndef CURVESVIEW_H
#define CURVESVIEW_H

#include "chartview.h"
#include <QWidget>
#include <QtCharts>

namespace Ui {
class CurvesView;
}

class CurvesView : public QWidget
{
    Q_OBJECT

public:
    static CurvesView *getInstance();
    ~CurvesView();

    void initializeParameters(QVector<int> time_sec,
                              float** mean_intensities,
                              int n_final_labels);

    void changeCurveVisibility(int curveSelected, bool visibility);
    void removeCurve(int curveSelected);
    void PlotCurves(int currentCurve);
private:
    Ui::CurvesView *ui;
    QVector<QLineSeries*> series;
    QVector<int> time_sec;

    static CurvesView *_instance;
    CurvesView();

private slots:
    void changeValueSliderCurve(int sliderCurrentValue);

};

#endif // CURVESVIEW_H
