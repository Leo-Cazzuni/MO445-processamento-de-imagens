#include "saliency.h"
#include "ui_saliency.h"

#include <QSettings>

Saliency::Saliency(QWidget *parent) :
    ArcWeightFunction(parent),
    ui(new Ui::Saliency)
{
    ui->setupUi(this);
    saliencyImage = nullptr;
    weightedSaliencyImage = nullptr;
    _name = "Salience map";
    connect(ui->pbLoad, SIGNAL(clicked()), this, SLOT(loadSaliency()));
    connect(ui->lePath, SIGNAL(textChanged(QString)), this, SLOT(updateArcWeightParams()));
    connect(ui->lePath, SIGNAL(textChanged(QString)), this, SLOT(updateSaliencyPath()));
    connect(ui->sbAdjRel, SIGNAL(valueChanged(double)), this, SLOT(updateArcWeightParams()));
    connect(ui->sbSaliencyFactor, SIGNAL(valueChanged(double)), this, SLOT(updateArcWeightParams()));
}

Saliency::~Saliency()
{
    iftDestroyImage(&saliencyImage);
    iftDestroyImage(&weightedSaliencyImage);
    delete ui;
}

const iftImage *Saliency::getSaliencyImage()
{
    if (!saliencyImage) {
        if (ui->lePath->text().isEmpty()) {
            loadSaliency();
        }

        QString path = ui->lePath->text();
        QFile file(path);
        if (!file.exists()) {
            throw "Invalid salience map image.";
        }
        saliencyImage = iftReadImageByExt(path.toUtf8().data());
        const iftImage *img = view->getImage();
        if (saliencyImage->xsize != img->xsize || saliencyImage->ysize != img->ysize || saliencyImage->zsize != img->zsize) {
            saliencyImage = nullptr;
            throw "Salience map dimension does not match the dimension of the current loaded image.";
        }
    }
    return saliencyImage;
}

const iftImage *Saliency::getWeightedSaliencyImage()
{
    float factor = ui->sbSaliencyFactor->value();

    if (!weightedSaliencyImage) {
        const iftImage *saliency  = getSaliencyImage();
        const iftImage *img       = view->getImage();
	
	if (saliency != nullptr){
	  weightedSaliencyImage = iftCreateImageFromImage(saliency);
	  int Imax       = iftMaximumValue(img);
	  iftImage *omap = iftNormalize(saliency,0,Imax);
	  iftImage *imap = iftNormalize(img,0,Imax);
	  
	  for (int p = 0; p < weightedSaliencyImage->n; p++) {
            weightedSaliencyImage->val[p] = factor * omap->val[p] + (1 - factor) * imap->val[p];
	  }

	  iftDestroyImage(&imap);
	  iftDestroyImage(&omap);
	  
	} else {
	  for (int p = 0; p < weightedSaliencyImage->n; p++) {
            weightedSaliencyImage->val[p] = img->val[p];
	  }
	}
    }
    
    return weightedSaliencyImage;
}

void Saliency::generate()
{
    float adjRelRadius = ui->sbAdjRel->value();
    float factor = ui->sbSaliencyFactor->value();

    try {
        getSaliencyImage();

        const iftImage *img = view->getImage();
        iftMImage *mimg = NULL;
        if (iftIsColorImage(img)) {
            mimg = iftImageToMImage(img, RGB_CSPACE);
        } else {
            mimg = iftImageToMImage(img, GRAY_CSPACE);
        }

        iftMImage *concat = iftExtendMImageByImage(mimg, saliencyImage);

        int bands = mimg->m;

        float k1 = (1 - factor) * (1 + bands) / bands;
        float k2 = factor * (1 + bands);

        #pragma omp parallel for
        for (ulong i = 0; i < concat->n; i++) {
            for (int j = 0; j < bands; j++) {
                concat->val[i][j] *= k1;
            }
            concat->val[i][bands] *= k2;
        }

        view->setGradient(concat, adjRelRadius);

        iftDestroyMImage(&mimg);
    } catch (QString error) {
        QMessageBox::warning((QWidget*) parent(),
                             tr("Warning"),
                             error + " Loading default image magnitude gradient.");
        loadDefault(adjRelRadius);
    }
}

void Saliency::loadSaliency()
{
    QSettings settings;
    QString saliency_path = QFileDialog::getOpenFileName((QWidget*) parent(),
                                                             tr("Open salience map output"),
                                                             settings.value(DEFAULT_SALIENCY_LOC_KEY).toString());

    if (!saliency_path.isEmpty()) {
        settings.setValue(DEFAULT_SALIENCY_LOC_KEY,
                          QFileInfo(saliency_path).absolutePath());
        ui->lePath->setText(saliency_path);
    }

}

void Saliency::updateSaliencyPath()
{
    iftDestroyImage(&saliencyImage);
    iftDestroyImage(&weightedSaliencyImage);
}

void Saliency::updateArcWeightParams()
{
    iftDestroyImage(&weightedSaliencyImage);
    ArcWeightFunction::updateArcWeightParams();
}

void Saliency::preprocess()
{

}
