#include "arcweightfunction.h"

ArcWeightFunction::ArcWeightFunction(QWidget *parent) : QWidget(parent)
{
    view = View::instance();
}

ArcWeightFunction::ArcWeightFunction(const ArcWeightFunction &other) : QWidget(other.parentWidget())
{
    this->view = other.view;
    this->_name = other._name;
}

QString ArcWeightFunction::name()
{
    return _name;
}

void ArcWeightFunction::preprocess()
{

}

void ArcWeightFunction::updateArcWeightParams()
{
    emit update();
}

void ArcWeightFunction::loadDefault(float adjRelRadius)
{
    const iftImage *img = view->getImage();
    iftMImage *mimg = NULL;
    if (iftIsColorImage(img)) {
        mimg = iftImageToMImage(img, LABNorm_CSPACE);
    } else {
        mimg = iftImageToMImage(img, GRAYNorm_CSPACE);
    }
    view->setGradient(mimg, adjRelRadius);
}

