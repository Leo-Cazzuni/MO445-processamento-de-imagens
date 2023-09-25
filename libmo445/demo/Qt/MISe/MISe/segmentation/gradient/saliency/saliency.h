#ifndef SALIENCY_H
#define SALIENCY_H

#include <QWidget>

#include <segmentation/gradient/arcweightfunction.h>

namespace Ui {
class Saliency;
}

class Saliency : public ArcWeightFunction
{
    Q_OBJECT

public:
    explicit Saliency(QWidget *parent = nullptr);
    ~Saliency();

    const iftImage *getSaliencyImage();
    const iftImage *getWeightedSaliencyImage();
public slots:
    void generate() override;


private slots:
    void loadSaliency();
    void updateSaliencyPath();
    void updateArcWeightParams() override;

private:
    Ui::Saliency *ui;

    iftImage *saliencyImage;
    iftImage *weightedSaliencyImage;

    void preprocess() override;


};

#endif // SALIENCY_H
