#include "manualarcweight.h"
#include "ui_manualarcweight.h"

#include <QSettings>

ManualArcWeight::ManualArcWeight(QWidget *parent) :
    ArcWeightFunction(parent),
    ui(new Ui::ManualArcWeight)
{
    ui->setupUi(this);

    _name = "Activation image";

    connect(ui->pbFind, SIGNAL(clicked()), this, SLOT(locateMimg()));
    //connect(ui->leMimg, SIGNAL(selectionChanged()), this, SLOT(locateMimg()));
    connect(ui->sbAdj, SIGNAL(valueChanged(double)), this, SLOT(updateArcWeightParams()));
}

ManualArcWeight::~ManualArcWeight()
{
    delete ui;
}

void ManualArcWeight::generate()
{
    float adjRelRadius = ui->sbAdj->value();
    QString mimgPath = ui->leMimg->text();

    try {
        if (!QFile(mimgPath).exists()) {
            throw QString("Invalid format.");
        }
        iftMImage *mimg = iftReadMImage(mimgPath.toUtf8().data());
        const iftImage *img = view->getImage();
        if (mimg->xsize != img->xsize || mimg->ysize != img->ysize || mimg->zsize != img->zsize) {
            throw QString("Activation image dimension does not match the dimension of the current loaded image.");
        }

        view->setGradient(mimg, adjRelRadius);
    }  catch (QString error) {
        QMessageBox::warning((QWidget*) parent(),
                             tr("Warning"),
                             error + " Loading default image magnitude gradient.");
        loadDefault(adjRelRadius);
    }
}

void ManualArcWeight::locateMimg()
{
    QSettings settings;

    QString manual_arcweight_path  = QFileDialog::getOpenFileName(this,
                                                                tr("Import arc-weight"),
                                                                settings.value(DEFAULT_MIMG_LOC_KEY).toString(),
                                                                tr("Multidimensional image (*.mimg);;"));
    if (!manual_arcweight_path.isEmpty()) {
        settings.setValue(DEFAULT_MIMG_LOC_KEY,
                          QFileInfo(manual_arcweight_path).absolutePath());
        ui->leMimg->setText(manual_arcweight_path);
    }
}
