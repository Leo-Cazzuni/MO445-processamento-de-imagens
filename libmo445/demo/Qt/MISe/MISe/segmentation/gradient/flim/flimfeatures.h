#ifndef FLIMFEATURES_H
#define FLIMFEATURES_H

#include <QWidget>

#include <segmentation/gradient/arcweightfunction.h>

#include <segmentation/altis/flim/flim.h>

namespace Ui {
class FLIMFeatures;
}

class FLIMFeatures : public ArcWeightFunction
{
    Q_OBJECT

public:
    explicit FLIMFeatures(QWidget *parent = nullptr);
    ~FLIMFeatures();

public slots:
    void generate() override;

private slots:
    void createNewFLIMNetwork();
    void RemoveFLIMNetwork();

private:
    Ui::FLIMFeatures *ui;

    FLIM *loadFLIM();
    QTemporaryDir *writeImageInTempDir();

    void preprocess() override;

    void createNewFLIMNetworkWithFLIMBuilder();
    void createNewFLIMNetworkLocatingIt();
};

#endif // FLIMFEATURES_H
