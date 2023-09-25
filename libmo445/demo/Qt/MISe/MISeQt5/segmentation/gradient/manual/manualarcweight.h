#ifndef MANUALARCWEIGHT_H
#define MANUALARCWEIGHT_H

#include <QWidget>

#include <segmentation/gradient/arcweightfunction.h>

namespace Ui {
class ManualArcWeight;
}

class ManualArcWeight : public ArcWeightFunction
{
    Q_OBJECT

public:
    explicit ManualArcWeight(QWidget *parent = nullptr);
    ~ManualArcWeight();

public slots:
    void generate() override;

private slots:
    void locateMimg();

private:
    Ui::ManualArcWeight *ui;
};

#endif // MANUALARCWEIGHT_H
