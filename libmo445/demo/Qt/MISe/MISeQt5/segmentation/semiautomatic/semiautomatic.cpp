#include "semiautomatic.h"
#include "ui_semiautomatic.h"

#include <mainwindow.h>
#include <marker.h>

SemiAutomatic::SemiAutomatic(MainWindow *parent, View *view) :
    Segmentation(parent, view),
    ui(new Ui::SemiAutomatic)
{
    ui->setupUi(this);
    previousMarkers = nullptr;
    fst = nullptr;
    _name = "DIFT-Watershed";
    annotationVisible = true;
    connect(ui->pbRun, SIGNAL(clicked()), this, SLOT(execute()));
    connect(ui->pbReset, SIGNAL(clicked()), this, SLOT(reset()));
}

SemiAutomatic::~SemiAutomatic()
{
    iftDestroyImage(&previousMarkers);
    iftDestroyImageForest(&fst);
    delete ui;
}

void SemiAutomatic::loadForest(iftImageForest *forest, iftLabeledSet *seeds)
{
    if (this->fst != nullptr) {
        iftDestroyImageForest(&this->fst);
    }
    if (this->previousMarkers != nullptr) {
        iftDestroyImage(&previousMarkers);
    }
    this->fst = forest;
    view->setImageForest(forest);
    parent->setMarkers(seeds);
    this->previousMarkers = iftCopyImage(view->annotation->getMarkers());
}

void SemiAutomatic::notifyImageUpdate()
{
    iftDestroyImageForest(&fst);
    iftDestroyImage(&previousMarkers);
}

void SemiAutomatic::notifyGradientUpdate(ArcWeightFunction *)
{
    iftDestroyImageForest(&fst);
    iftDestroyImage(&previousMarkers);
}


void SemiAutomatic::showEvent(QShowEvent *event)
{
    Segmentation::showEvent(event);
}

void SemiAutomatic::execute()
{
    // Check if the forest is null or if another segmentation method has replaced the view label
    if (fst == nullptr || !iftAreImagesEqual(view->getLabel(), fst->label)) {
        createImageForest();
    }

    Segmentation::execute();
}

void SemiAutomatic::reset()
{
    QMessageBox::StandardButton bt_clicked = QMessageBox::warning(this, tr("Warning"),
                                                                 "This action is irreversible. "
                                                                 "Do you want to proceed?",
                                                                 QMessageBox::Yes | QMessageBox::No);

    if (bt_clicked == QMessageBox::Yes) {
        // TODO check memory leak
        if (fst)
            iftResetImageForest(fst);
        view->annotation->resetMarkers();
        view->destroyLabel();
        finishSegmentation();
        iftDestroyImage(&previousMarkers);
    }
}

iftImage *SemiAutomatic::generateLabel() {
    iftLabeledSet *new_seeds = nullptr;
    iftSet *deleted_seeds = nullptr;
    const iftImage *markers = view->annotation->getMarkers();

    if (previousMarkers == nullptr) {
        previousMarkers = iftCreateImage(markers->xsize, markers->ysize, markers->zsize);
        iftInitMarkersImage(previousMarkers);
    }

    updateProgress(0.05, "Loading seeds.");

    createDiffSeedSets(markers, &new_seeds, &deleted_seeds);

    updateProgress(0.15, "Performing watershed transform.");

    QElapsedTimer elapsedTimer; elapsedTimer.start();

    iftFastDiffWatershed(fst, new_seeds, deleted_seeds);

    qDebug() << elapsedTimer.elapsed();

    updateProgress(0.5);

    iftDestroyImage(&previousMarkers);
    previousMarkers = iftCopyImage(markers);

    iftImage *label = iftCopyImage(fst->label);

    updateProgress(0.6);

    labelInfo = &view->annotation->getMarkerInfoArray();

    return label;
}

void SemiAutomatic::createDiffSeedSets(const iftImage *markers, iftLabeledSet **new_seeds, iftSet **deleted_seeds)
{
    const iftImage *removal_markers = view->annotation->getRemovalMarkers();

    //#pragma omp parallel for
    for (int i = 0; i < markers->n; i++) {
        if (removal_markers->val[i] == 0) {
            if (markers->val[i] != UNMARKED && previousMarkers->val[i] == UNMARKED) {
                iftInsertLabeledSet(new_seeds, i, markers->val[i]);
            } else if (markers->val[i] == UNMARKED && previousMarkers->val[i] != UNMARKED) {
                // This happens when the user deletes the marker directly in the list of markers
                iftInsertSet(deleted_seeds, i);
            } else if (markers->val[i] != UNMARKED && previousMarkers->val[i] != UNMARKED && previousMarkers->val[i] != markers->val[i]) {
                iftInsertLabeledSet(new_seeds, i, markers->val[i]);
                iftInsertSet(deleted_seeds, i);
            }
        } else {
            //TODO test
            if (previousMarkers->val[i] >= 0) {
                iftInsertSet(deleted_seeds, i);
            }
            markers->val[i] = UNMARKED;
            removal_markers->val[i] = 0;
        }

    }
}

void SemiAutomatic::createImageForest()
{
    if (fst != nullptr)
        iftDestroyImageForest(&fst);
    if (previousMarkers != nullptr) {
        iftDestroyImage(&previousMarkers);
    }
    if (view->getGradient() == nullptr)
        parent->slotCalculateGradient();
    const iftMImage *mimg = view->getGradient();
    iftAdjRel *A = iftSpheric(1.0);
    iftAdjRel *B = iftSpheric(view->getGradientAdjRelRadius());
    iftImage *grad = MImageGradient(mimg, B);
    fst = iftCreateImageForest(grad, A);
    view->setImageForest(fst);
    previousMarkers = nullptr;
    iftDestroyAdjRel(&B);
}

