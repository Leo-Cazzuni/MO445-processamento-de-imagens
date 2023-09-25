#include "dilation.h"
#include "ui_dilation.h"

Dilation::Dilation(QWidget *parent) :
    Method(parent),
    ui(new Ui::Dilation)
{
    ui->setupUi(this);
    _name = "Dilation";
}

Dilation::~Dilation()
{
    delete ui;
}

iftImage *Dilation::process(iftImage *img)
{
    float radius = ui->sbRadius_3->value();
    iftSet *S = NULL;
    iftImage *res = iftDilateBin(img, &S, radius);
    iftDestroySet(&S);
    return res;
}
