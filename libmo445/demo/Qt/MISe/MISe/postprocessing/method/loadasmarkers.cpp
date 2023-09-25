#include "loadasmarkers.h"
#include "ui_loadasmarkers.h"

#include <views/view.h>

LoadAsMarkers::LoadAsMarkers(QWidget *parent) :
    Method(parent),
    ui(new Ui::LoadAsMarkers)
{
    ui->setupUi(this);
    _name = "Load as Markers";
}

LoadAsMarkers::~LoadAsMarkers()
{
    delete ui;
}

iftImage *LoadAsMarkers::process(iftImage *img)
{
    View * view = View::instance();
    for (int i = 0; i < img->n; i++) {
        if (img->val[i] == 0)
            img->val[i] = UNMARKED;
    }
    int max = iftMaximumValue(img);

    for (int i = 0; i <= max; i++){
        view->annotation->addItemInColorTable();
    }

    view->annotation->setMarkers(img);
    if (ui->cbDeleteLabel->isChecked())
        return NULL;
    else
        return img;
}
