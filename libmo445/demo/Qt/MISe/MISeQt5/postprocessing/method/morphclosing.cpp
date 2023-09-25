#include "morphclosing.h"
#include "ui_morphclosing.h"
#include "iftMathMorph.h"

MorphClosing::MorphClosing(QWidget *parent) :
    Method(parent),
    ui(new Ui::MorphClosing)
{
    ui->setupUi(this);
    _name = "Morphological Closing";
}

MorphClosing::~MorphClosing()
{
    delete ui;
}

iftImage *MorphClosing::process(iftImage *img)
{
    float radius = ui->sbRadius->value();
    return iftCloseBin(img, radius);
}
