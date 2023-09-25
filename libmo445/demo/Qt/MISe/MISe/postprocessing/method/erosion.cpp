#include "erosion.h"
#include "ui_erosion.h"
#include <ift.h>

Erosion::Erosion(QWidget *parent) :
    Method(parent),
    ui(new Ui::Erosion)
{
    ui->setupUi(this);
    _name = "Erosion";
}

Erosion::~Erosion()
{
    delete ui;
}

iftImage *Erosion::process(iftImage *img)
{
    float radius = ui->sbRadius->value();
    iftSet *S = NULL;
    iftImage *res = iftErodeBin(img, &S, radius);
    iftDestroySet(&S);
    return res;
}
