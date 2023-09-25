#ifndef AUTOECE_H
#define AUTOECE_H

#include <QWidget>
#include <QVector>
#include <segmentation/segmentation.h>
#include "curvesview.h"
#include "ift.h"
#include <mainwindow.h>

namespace Ui {
class AutoECE;
}

class AutoECE : public Segmentation
{
    Q_OBJECT

public:
    AutoECE(MainWindow *parent, View *view);
    ~AutoECE();

    QList<iftImage*> generateLabel() override;
    void     notifyImageUpdate()  override;
private slots:
    void toggleAdvancedOptions(bool checked);
    void changeValueSliderTime(int time);
    void changeTimeUserTimerEdit(QTime time);
    void viewECECurves();
    void toggleBackgroundSeeds(bool checked);

    void on_pushButton_clicked();

private:
    Ui::AutoECE *ui;
    QVector<int> time_sec;
    CurvesView *curves;

    float **mean_intensity_superspels;
    float **final_mean_intensity_superspels;
    int n_final_labels;
};

#endif // AUTOECE_H
