#ifndef ALTIS_H
#define ALTIS_H

#include <QWidget>
#include "../semiautomatic/semiautomatic.h"
#include "../segmentation.h"
#include "ui_altis.h"

#define STANDARD_VOXEL_SIZE 1.25

namespace Ui {
class ALTIS;
}

class ALTIS : public Segmentation
{
    Q_OBJECT

public:
    explicit ALTIS(MainWindow *parent, View *view, SemiAutomatic *semiAutomaticSeg);
    ~ALTIS();

    void notifyImageUpdate() override;
private slots:
    void correctSegmentation();
    void toggleAdvancedOptions(bool checked);
    void togglePleuralCorretion(int state);

protected:
    Ui::ALTIS *ui;

    virtual iftImage *calculateImageGradient();
private:
    SemiAutomatic *semiAutomaticSeg;
    iftImageForest *forest;
    iftLabeledSet  *seeds;

    double adjRelRadius;
    long   closingRadius;
    double otsuThreshold;
    double tracheaThreshold;
    long   dilationSquaredRadius;
    long   erosionSquaredRadius;

    bool   gaussianFilterApplied;
    bool   removeNoiseApplied;

    iftImage *enhanced;

    QList<iftImage*> generateLabel() override;

    // ALTIS PROCEDURES
    iftImage      *iftExtractVolumeOfInterest(iftImage *img, iftAdjRel *A, iftImage **grad, iftSet **S);
    iftImage      *iftSegmentRespiratorySystem(iftImage *grad, iftImage *voi, iftSet *S, iftAdjRel *A, iftImage *img);
    iftImage      *iftExternalInternalLungMarkers(iftImage *bin, iftSet *S, iftSet **Si, iftSet **Se);
    iftSet        *iftTracheaMarker(iftImage *bin, iftSet *Si);
    void           iftLabelLungsMarkers(iftImage *bin, iftSet *Si, iftLabeledSet **seeds);
    void           iftLabelTracheaMarkers(iftImage *bin, iftSet *St, iftLabeledSet **seeds);
    iftLabeledSet *iftLabelMarkers(iftImage *bin, iftSet *Si, iftSet *St, iftSet *Se, iftSet **forbidden);
    void           iftGaussianFilter(bool apply, iftImage **img);
    void           iftRemoveNoise(bool apply, iftImage **img, iftAdjRel *A);
};

#endif // ALTIS_H
