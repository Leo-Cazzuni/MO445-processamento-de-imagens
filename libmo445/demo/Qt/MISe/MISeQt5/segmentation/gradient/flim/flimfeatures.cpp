#include "flimfeatures.h"
#include "ui_flimfeatures.h"

#include <QProcess>
#include <QPushButton>
#include <QSettings>

FLIMFeatures::FLIMFeatures(QWidget *parent) :
    ArcWeightFunction(parent),
    ui(new Ui::FLIMFeatures)
{
    ui->setupUi(this);
    _name = "FLIM-based arc weights";

    connect(ui->pbCreate, SIGNAL(clicked()), this, SLOT(createNewFLIMNetwork()));
    connect(ui->pbRemove, SIGNAL(clicked()), this, SLOT(RemoveFLIMNetwork()));
    connect(ui->sbAdj, SIGNAL(valueChanged(double)), this, SLOT(updateArcWeightParams()));
    connect(ui->cbFLIM, SIGNAL(currentIndexChanged(int)), this, SLOT(updateArcWeightParams()));
    connect(ui->sbFactor, SIGNAL(valueChanged(double)), this, SLOT(updateArcWeightParams()));
}

FLIMFeatures::~FLIMFeatures()
{
    delete ui;
}

void FLIMFeatures::generate()
{    
    FLIM *flim = loadFLIM();
    float adjRelRadius = ui->sbAdj->value();
    float factor = ui->sbFactor->value();

    if (flim) {
        iftMImage *features = flim->extractFeatures();

        const iftImage *img = view->getImage();
        iftMImage *mimg = NULL;
        if (iftIsColorImage(img)) {
            mimg = iftImageToMImage(img, LABNorm_CSPACE);
        } else {
            mimg = iftImageToMImage(img, GRAY_CSPACE);
        }

        iftMImage *concat = iftExtendMImageByMImage(mimg, features);

        int bands = mimg->m + features->m;

        float k1 = (1 - factor) * (bands) / (mimg->m);
        float k2 = factor * (bands);

        #pragma omp parallel for
        for (ulong i = 0; i < concat->n; i++) {
            for (ulong j = 0; j < mimg->m; j++) {
                concat->val[i][j] *= k1;
            }
            for (ulong j = mimg->m; j < concat->m; j++) {
                concat->val[i][j] *= k2;
            }
        }

        view->setGradient(concat, adjRelRadius);

        iftDestroyMImage(&features);
        iftDestroyMImage(&mimg);
    } else {
        QMessageBox::warning((QWidget*) parent(),
                             tr("Warning"),
                             "Invalid FLIM network, loading default image magnitude gradient.");

        loadDefault(adjRelRadius);
    }
    delete flim;
}

FLIM *FLIMFeatures::loadFLIM()
{
    QString flim_path = ui->cbFLIM->currentText();
    QDir *network = new QDir(flim_path);
    if (network->exists()) {
        QString arch_path = network->filePath("arch.json");
        if (QFile::exists(arch_path)) {
            iftFLIMArch *arch = iftReadFLIMArch(arch_path.toUtf8().data());
            QTemporaryDir *i_dir = writeImageInTempDir();
            return new FLIM(arch, i_dir, network);
        } else {
            QMessageBox::critical((QWidget*) parent(), tr("Critical error"), "arch.json not found in FLIM's network path.");
        }
    } else {
        QMessageBox::critical((QWidget*) parent(), tr("Critical error"), "Invalid FLIM's network path.");
    }

    return nullptr;
}

QTemporaryDir *FLIMFeatures::writeImageInTempDir()
{
    QTemporaryDir *dir = new QTemporaryDir();
    QString filename = view->getFilename();

    QString url(dir->path() + "/" +filename);

    iftWriteImageByExt(view->getImage(), url.toUtf8().data());

    return dir;
}

#include <QtDebug>
void FLIMFeatures::preprocess()
{   
    QSettings settings;

    QStringList models = settings.value(DEFAULT_FLIM_MODEL_LIST_KEY).value<QStringList>();

    ui->cbFLIM->clear();
    if (!models.empty()) {
        for (QString path: models) {
            ui->cbFLIM->addItem(path);
        }
    } else {
        createNewFLIMNetwork();
    }
}

void FLIMFeatures::createNewFLIMNetworkWithFLIMBuilder()
{
    QProcess process;
    process.setParent(parent());//TODO
    // TODO emit a signal to disable window
    QSettings settings;
    process.setProgram("FLIMbuilder");
    process.start();
    if (!process.waitForFinished(-1)) {
        QString flimbuilder_path = settings.value(DEFAULT_FLIMBUILDER_LOC_KEY).toString();

        if (flimbuilder_path.isEmpty()) {
            flimbuilder_path = QFileDialog::getOpenFileName((QWidget*) parent(),
                                                             tr("Locate FLIMbuilder"));
            if (!flimbuilder_path.isEmpty()) {
                settings.setValue(DEFAULT_FLIMBUILDER_LOC_KEY, flimbuilder_path);
            }
        }

        process.setProgram(flimbuilder_path);
        process.start();
        if (!process.waitForFinished(-1)) {
            qDebug() << process.error();
            return;
        }

    }

    QString output(process.readAllStandardOutput());
    qDebug() << output.split("\n");
}

void FLIMFeatures::createNewFLIMNetworkLocatingIt()
{
    QSettings settings;

    QStringList models = settings.value(DEFAULT_FLIM_MODEL_LIST_KEY).value<QStringList>();

    QString flim_path = QFileDialog::getExistingDirectory((QWidget*) parent(),
                                                             tr("Open FLIM's network location"),
                                                             settings.value(DEFAULT_FLIM_MODEL_DIR_KEY).toString(),
                                                             QFileDialog::ShowDirsOnly
                                                             | QFileDialog::DontResolveSymlinks);
    if (!flim_path.isEmpty()) {
        settings.setValue(DEFAULT_FLIM_MODEL_DIR_KEY, flim_path);
        if (!models.contains(flim_path)) {
            models << flim_path;
            settings.setValue(DEFAULT_FLIM_MODEL_LIST_KEY, QVariant::fromValue(models));
            ui->cbFLIM->addItem(flim_path);
        }
        ui->cbFLIM->setCurrentText(flim_path);
    }
}

void FLIMFeatures::createNewFLIMNetwork()
{
    QMessageBox message(QMessageBox::Question, tr("Add a new FLIM network"), tr("How would you like to add a new FLIM network?"));
    QPushButton *bt_create = new QPushButton(QIcon::fromTheme("add"), tr("Create new FLIM network with FLIMbuilder (in development)"));
    bt_create->setEnabled(false);
    QPushButton *bt_locate = new QPushButton(QIcon(":/Images/icons/folder_open.svg"), tr("Locate an existing FLIM network"));
    message.addButton(bt_create, QMessageBox::ActionRole);
    message.addButton(bt_locate, QMessageBox::ActionRole);
    message.addButton(QMessageBox::Cancel);

    message.exec();

    QAbstractButton *bt_clicked = message.clickedButton();

    if (bt_clicked == bt_create) {
        createNewFLIMNetworkWithFLIMBuilder();
    } else if (bt_clicked == bt_locate) {
        createNewFLIMNetworkLocatingIt();
    }

    delete bt_create;
    delete bt_locate;
}

void FLIMFeatures::RemoveFLIMNetwork()
{
    QMessageBox::StandardButton bt_clicked = QMessageBox::warning(this, tr("Remove FLIM network from list"),
                         "This operation will remove the FLIM network from the list, "
                         "but not from the file system. "
                         "Do you want to proceed?",
                         QMessageBox::Yes | QMessageBox::No);

    if (bt_clicked == QMessageBox::Yes) {

        QSettings settings;
        QStringList models = settings.value(DEFAULT_FLIM_MODEL_LIST_KEY).value<QStringList>();

        QString selectedModel = ui->cbFLIM->currentText();
        models.removeAll(selectedModel);
        settings.setValue(DEFAULT_FLIM_MODEL_LIST_KEY, QVariant::fromValue(models));

        preprocess();

    }
}
