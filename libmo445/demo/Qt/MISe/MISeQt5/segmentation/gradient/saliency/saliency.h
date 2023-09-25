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

    const iftImage *getSaliencyImage() throw();
public slots:
    void generate() override;


private slots:
    void loadSaliency();
    void updateSaliencyPath();

private:
    Ui::Saliency *ui;

    iftImage *saliencyImage;

    void preprocess() override;


};

#endif // SALIENCY_H
