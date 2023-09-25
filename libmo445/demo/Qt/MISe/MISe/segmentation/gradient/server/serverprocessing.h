#ifndef SERVERPROCESSING_H
#define SERVERPROCESSING_H

#include <QWidget>

#include <segmentation/gradient/arcweightfunction.h>

namespace Ui {
class ServerProcessing;
}

class ServerProcessing : public ArcWeightFunction
{
    Q_OBJECT

public:
    explicit ServerProcessing(QWidget *parent = nullptr);
    ~ServerProcessing();

public slots:
    void generate() override;

private slots:
    void updateServerUrl();

private:
    Ui::ServerProcessing *ui;

    void getProcessedImageThroughServer();

    iftMImage *processedImage;

    void preprocess() override;
};



#endif // SERVERPROCESSING_H
