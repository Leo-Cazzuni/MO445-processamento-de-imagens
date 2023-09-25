#ifndef DYNAMICIFT_H
#define DYNAMICIFT_H

#include <segmentation/dynamicift/dynamicift.h>
#include <QWidget>

#include <segmentation/segmentation.h>

namespace Ui {
class DynamicIFT;
}

class DynamicIFT : public Segmentation
{
    Q_OBJECT

public:
    DynamicIFT(MainWindow *parent, View *view);
    ~DynamicIFT();


    void notifyImageUpdate() override;
    void notifyGradientUpdate(ArcWeightFunction *function) override;
protected:
    iftImage *generateLabel() override;
    void showEvent(QShowEvent *event) override;

    void execute() override;

private slots:
    void reset();

private:
    Ui::DynamicIFT *ui;
    iftImage *previousMarkers;
    //iftDynamicForest *forest;
    iftDynTrees *forest;

    void createImageForest();

    void iftDiffCreateSeedSets(const iftImage *markers, iftImage *previousMarkers, iftLabeledSet **new_seeds, iftSet **deleted_seeds);
};

#endif // DYNAMICIFT_H
