#include "brain.h"
#include "ui_brain.h"

Brain::Brain(QWidget *parent) :
    ArcWeightFunction(parent),
    ui(new Ui::Brain)
{
    ui->setupUi(this);
    _name = "Brain (Visva)";
}

Brain::~Brain()
{
    delete ui;
}

void Brain::generate()
{
    const iftImage *img = view->getImage();
    iftMImage *mimg = NULL;
    if (iftIsColorImage(img)) {
        mimg = iftImageToMImage(img, RGB_CSPACE);
    } else {
        mimg = iftImageToMImage(img, GRAY_CSPACE);
    }
    view->setGradient(mimg, -1);
}
