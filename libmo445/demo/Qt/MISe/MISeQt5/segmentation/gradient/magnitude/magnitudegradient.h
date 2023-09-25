#ifndef MAGNITUDEGRADIENT_H
#define MAGNITUDEGRADIENT_H

#include <QWidget>

#include <segmentation/gradient/arcweightfunction.h>

namespace Ui {
class MagnitudeGradient;
}

class MagnitudeGradient : public ArcWeightFunction
{
    Q_OBJECT

public:
    explicit MagnitudeGradient(QWidget *parent = nullptr);
    ~MagnitudeGradient();

public slots:
    void generate();

private:
    Ui::MagnitudeGradient *ui;


};

#endif // MAGNITUDEGRADIENT_H
