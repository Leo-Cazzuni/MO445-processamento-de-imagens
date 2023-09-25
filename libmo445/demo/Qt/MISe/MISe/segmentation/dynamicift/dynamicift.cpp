#include "dynamicift.h"
#include "ui_dynamicift.h"
#include "global.h"

#include <mainwindow.h>

DynamicIFT::DynamicIFT(MainWindow *parent, View *view)
    : Segmentation(parent, view),
      ui(new Ui::DynamicIFT)
{
    ui->setupUi(this);

    connect(ui->pbRun, SIGNAL(clicked()), this, SLOT(execute()));
    connect(ui->pbReset, SIGNAL(clicked()), this, SLOT(reset()));
    forest = nullptr;
    annotationVisible = true;
    previousMarkers = nullptr;
    _name = "DIFT-DynamicTrees";
}

DynamicIFT::~DynamicIFT()
{
    iftDestroyDynTrees(&forest);
    iftDestroyImage(&previousMarkers);
    delete ui;
}

void DynamicIFT::notifyImageUpdate()
{
    //iftDestroyDynamicForest(&forest);
    iftDestroyDynTrees(&forest);
    iftDestroyImage(&previousMarkers);
}

void DynamicIFT::notifyGradientUpdate(ArcWeightFunction *)
{
    iftDestroyDynTrees(&forest);
    iftDestroyImage(&previousMarkers);
}


#include <QDebug>
QList<iftImage*>DynamicIFT::generateLabel()
{
    iftLabeledSet *new_seeds = nullptr;
    iftSet *deleted_seeds = nullptr;
    const iftImage *markers = view->annotation->getMarkers();

    if (previousMarkers == nullptr) {
        previousMarkers = iftCreateImage(markers->xsize, markers->ysize, markers->zsize);
        iftInitMarkersImage(previousMarkers);
    }

    updateProgress(0.05, "Loading seeds.");

    iftDiffCreateSeedSets(markers, previousMarkers, &new_seeds, &deleted_seeds);

//    iftRemMarkersFromDynamicForest(forest, deleted_seeds);
//    iftAddMarkersToDynamicForest(forest, new_seeds);

    updateProgress(0.15, "Performing DynamicIFT");

    QElapsedTimer elapsedTimer; elapsedTimer.start();
    iftExecDiffDynTrees(forest, &new_seeds, &deleted_seeds);
    qDebug() << elapsedTimer.elapsed();

    updateProgress(0.5);

    iftDestroyImage(&previousMarkers);
    previousMarkers = iftCopyImage(markers);

    iftImage *label = iftCopyImage(forest->label);

    updateProgress(0.6);

    labelInfo = &view->annotation->getMarkerInfoArray();

//    iftMImage *pmap  = NULL;
//    iftImage *_label = NULL;
//    label = iftCreateImageFromImage(label);
//    float threshold = 0.96;
//    for (int i =0; i < 5; i++) {
//        iftLabeledSet *seeds = NULL;

//        if (i == 0) {
//            for (int j = 0; j < markers->n; j++) {
//                if (markers->val[j] != UNMARKED) {
//                    iftInsertLabeledSet(&seeds, j, markers->val[j]);
//                }
//            }

//        } else {
//            for (int j = 0; j < markers->n; j++) {
//                if (label->val[j] > 0) {
//                    iftInsertLabeledSet(&seeds, j, 1);
//                } else if (label->val[j] < 0) {
//                    iftInsertLabeledSet(&seeds, j, 0);
//                }
//            }
//        }

//        iftDestroyImage(&_label);
//        _label = iftDynamicSetRootPolicy(forest->img, forest->A, seeds, 0, false);


//        iftMImage *wmap = iftDTRootWeightsMap(forest->img, forest->A, seeds, 0, false);
//        iftMImage *pmap = wmap;
//        float _max = iftMMaximumValue(wmap, 0);
//        for (int p = 0; p < wmap->n; p++) {
//            pmap->val[p][0] = (_max - wmap->val[p][0]) / (_max + wmap->val[p][0]);
//            if (pmap->val[p][0] > threshold && _label->val[p] && label->val[p] == 0) {
//                label->val[p] = i+1;
//            }

//            if (pmap->val[p][0] > threshold && _label->val[p] == 0 && label->val[p] == 0) {
//                label->val[p] = -i-1;
//            }

//        }

//    }

//    for (int j = 0; j < markers->n; j++) {
//        if (label->val[j] < 0) {
//            label->val[j] = 0;
//        } else if (label->val[j] > 0) {
//            label->val[j] = 1;
//        }

//    }

    return {label};
}

void DynamicIFT::showEvent(QShowEvent *event)
{
    Segmentation::showEvent(event);
}

void DynamicIFT::execute()
{
    // Check if the forest is null or if another segmentation method has replaced the view label
    if (forest == nullptr || !iftAreImagesEqual(view->getLabel(), forest->label)) {
        createImageForest();
    }

    Segmentation::execute();
}

void DynamicIFT::reset()
{
    QMessageBox::StandardButton bt_clicked = QMessageBox::warning(this, tr("Warning"),
                                                                 "This action is irreversible. "
                                                                 "Do you want to proceed?",
                                                                 QMessageBox::Yes | QMessageBox::No);

    if (bt_clicked == QMessageBox::Yes) {
        if (forest) {
            //TODO iftResetDynamicForest(forest);
            iftResetGQueue(forest->Q);
            #pragma omp parallel for
            for (ulong p = 0; p < forest->img->n; p++)
            {
                forest->cost->val[p]             = IFT_INFINITY_INT;
                forest->root->val[p]             = p;
                forest->label->val[p]            = 0;
                forest->pred->val[p]             = IFT_NIL;
                forest->treesize[p]              = 0;
                for (ulong b = 0; b < forest->img->m; b++) {
                    forest->cumfeat->val[p][b] = 0;
                }
            }
        }
        view->annotation->resetMarkers();
        view->destroyLabel();
        finishSegmentation();
        iftDestroyImage(&previousMarkers);
    }
}

void DynamicIFT::createImageForest()
{
    iftAdjRel *A = iftSpheric(1.0);

    if (previousMarkers != nullptr) {
        iftDestroyImage(&previousMarkers);
    }

    if (view->getGradient() == nullptr)
        parent->slotCalculateGradient();

    //TODO remove
    iftMImage *mimg = const_cast<iftMImage*>(view->getGradient());
    if (forest) {
        iftDestroyAdjRel(&forest->A);
        iftDestroyDynTrees(&forest);
    }
    forest = iftCreateDynTrees(mimg, A);
    view->setImageForest(forest);
}

//TODO move to annotation
void DynamicIFT::iftDiffCreateSeedSets(const iftImage *markers, iftImage *previousMarkers, iftLabeledSet **new_seeds, iftSet **deleted_seeds)
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
