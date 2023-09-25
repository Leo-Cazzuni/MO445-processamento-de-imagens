#include "magnitudegradient.h"
#include "ui_magnitudegradient.h"

MagnitudeGradient::MagnitudeGradient(QWidget *parent) :
    ArcWeightFunction(parent),
    ui(new Ui::MagnitudeGradient)
{
    ui->setupUi(this);
    _name = "Gradient Magnitude";
    connect(ui->sbAdjRel, SIGNAL(valueChanged(double)), this, SLOT(updateArcWeightParams()));
}

MagnitudeGradient::~MagnitudeGradient()
{
    delete ui;
}

void MagnitudeGradient::generate()
{
    float adjRelRadius = ui->sbAdjRel->value();
    loadDefault(adjRelRadius);
}
