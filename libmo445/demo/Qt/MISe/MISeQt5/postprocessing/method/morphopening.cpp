#include "morphopening.h"
#include "ui_morphopening.h"
#include <iftMathMorph.h>

MorphOpening::MorphOpening(QWidget *parent) :
    Method(parent),
    ui(new Ui::MorphOpening)
{
    _name = "Morphological Opening";
    ui->setupUi(this);
}

MorphOpening::~MorphOpening()
{
    delete ui;
}

iftImage *MorphOpening::process(iftImage *img)
{
    float radius = ui->sbRadius->value();
    return iftOpenBin(img, radius);
}
