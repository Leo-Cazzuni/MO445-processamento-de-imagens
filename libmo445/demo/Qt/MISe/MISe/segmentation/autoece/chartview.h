#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QtCharts>
#include <QChartView>


class ChartView : public QChartView
{
public:
    ChartView(QWidget *&parent);
protected:
    void drawForeground(QPainter *painter, const QRectF &rect);
};

#endif // CHARTVIEW_H
