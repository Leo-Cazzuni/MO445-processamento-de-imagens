#include "largestcomponentselection.h"
#include "ui_largestcomponentselection.h"
#include <ift.h>

LargestComponentSelection::LargestComponentSelection(QWidget *parent) :
    Method(parent),
    ui(new Ui::LargestComponentSelection)
{
    ui->setupUi(this);
    _name = "Select Largest Component";
}

LargestComponentSelection::~LargestComponentSelection()
{
    delete ui;
}

iftImage *LargestComponentSelection::process(iftImage *img)
{
    iftAdjRel *A = iftSpheric(1.5);
    iftImage *res = iftSelectLargestComp(img, A);
    iftDestroyAdjRel(&A);
    return res;
}
