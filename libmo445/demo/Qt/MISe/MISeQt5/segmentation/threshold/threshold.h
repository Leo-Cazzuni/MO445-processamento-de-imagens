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

private:
    iftImage *generateLabel() override;

    Ui::Threshold *ui;
    HistogramView *histogram;
};

#endif // THRESHOLD_H
