#include "serverprocessing.h"
#include "ui_serverprocessing.h"

#include <QHttpPart>
#include <QNetworkReply>
#include <QNetworkAccessManager>

ServerProcessing::ServerProcessing(QWidget *parent) :
    ArcWeightFunction(parent),
    ui(new Ui::ServerProcessing)
{
    ui->setupUi(this);
    connect(ui->sbAdjRel, SIGNAL(valueChanged(double)), this, SLOT(updateArcWeightParams()));
    connect(ui->lnServer, SIGNAL(textChanged(QString)), this, SLOT(updateArcWeightParams()));
    processedImage = nullptr;
    _name = "Server processing";
}

ServerProcessing::~ServerProcessing()
{
    iftDestroyMImage(&processedImage);
    delete ui;
}

void ServerProcessing::generate()
{
    float adjRelRadius = ui->sbAdjRel->value();

    try {
        getProcessedImageThroughServer();

        view->setGradient(processedImage, adjRelRadius);
    } catch (QString error) {
        QMessageBox::warning((QWidget*) parent(),
                             tr("Warning"),
                             error + " Loading default image magnitude gradient.");
        loadDefault(adjRelRadius);
    }
}

void ServerProcessing::updateServerUrl()
{

}

void ServerProcessing::getProcessedImageThroughServer()
{
    QString img_tmp = "/tmp/mise_tmp.png";
    iftWriteImageByExt(View::instance()->getImage(), img_tmp.toUtf8().data());


    QString seeds_tmp = "/tmp/mise_tmp.txt";
    const iftImage *seed_image = View::instance()->annotation->getMarkers();
    iftLabeledSet *seed = NULL;
    for(int p = 0; p < seed_image->n; p++) {
        int label = seed_image->val[p];
        if(label >= 0)
            iftInsertLabeledSet(&seed, p, label);
    }
    iftWriteSeeds(seed, seed_image, seeds_tmp.toUtf8().data());

    QUrl testUrl(ui->lnServer->text());
    qDebug() << testUrl.host();
    qDebug() << testUrl.port();
    QNetworkAccessManager *mgr = new QNetworkAccessManager();
    QNetworkRequest request(testUrl);
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QFile *imgfile = new QFile(img_tmp);
    imgfile->open(QIODevice::ReadOnly );
    QHttpPart imgPart;
    imgPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"img\"; filename=\"" + imgfile->fileName() + ""));
    imgPart.setBodyDevice(imgfile);
    imgfile->setParent(multiPart);
    multiPart->append(imgPart);

    QFile *markerfile = new QFile(seeds_tmp);
    markerfile->open(QIODevice::ReadOnly );
    QHttpPart seedPart;
    seedPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"seeds\"; filename=\"" + markerfile->fileName() + ""));
    seedPart.setBodyDevice(markerfile);
    markerfile->setParent(multiPart);
    multiPart->append(seedPart);

    request.setUrl(testUrl);

    QNetworkReply *reply = mgr->post(request,multiPart);


    QEventLoop eventloop;
    connect(reply,SIGNAL(finished()),&eventloop,SLOT(quit()));
    eventloop.exec();

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray bts = reply->readAll();

        QFile file("test.mimg");
        file.open(QIODevice::WriteOnly);
        file.write(bts);
        file.close();

        processedImage = iftReadMImage("test.mimg");
    }
}

void ServerProcessing::preprocess()
{

}
