#include "curvesece.h"
#include "ui_curvesece.h"

CurvesECE::CurvesECE(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurvesECE)
{
    ui->setupUi(this);
}

CurvesECE::~CurvesECE()
{
    delete ui;
}
