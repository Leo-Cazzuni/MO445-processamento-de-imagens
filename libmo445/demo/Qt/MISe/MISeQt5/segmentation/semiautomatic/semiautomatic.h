#ifndef SEMIAUTOMATIC_H
#define SEMIAUTOMATIC_H

#include "../segmentation.h"

#include <QWidget>

namespace Ui {
class SemiAutomatic;
}

class SemiAutomatic : public Segmentation
{
    Q_OBJECT

public:
    explicit SemiAutomatic(MainWindow *parent, View *view);
    ~SemiAutomatic();

    void loadForest(iftImageForest *forest, iftLabeledSet *seeds);

    void notifyImageUpdate() override;
    void notifyGradientUpdate(ArcWeightFunction *function) override;
protected:
    void showEvent(QShowEvent *event) override;

    void execute() override;
protected slots:
    void reset();

private:
    void createDiffSeedSets(const iftImage *markers, iftLabeledSet **new_seeds, iftSet **deleted_seeds);

    void createImageForest();

    iftImage *generateLabel() override;

    Ui::SemiAutomatic *ui;
    iftImage *previousMarkers;
    iftImageForest *fst;
};

#endif // SEMIAUTOMATIC_H
