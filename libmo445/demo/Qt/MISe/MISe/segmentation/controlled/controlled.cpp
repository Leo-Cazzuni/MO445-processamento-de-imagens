#include "controlled.h"
#include "ui_controlled.h"

#include <mainwindow.h>

#include <segmentation/gradient/brain/brain.h>

Controlled::Controlled(MainWindow *parent, View *view) :
    Segmentation(parent, view),
    ui(new Ui::Controlled)
{
    ui->setupUi(this);
    _name = "Restricted IFT-Watershed";
    annotationVisible = true;
    connect(ui->pbRun, SIGNAL(clicked()), this, SLOT(execute()));
}

Controlled::~Controlled()
{
    delete ui;
}

iftImage *iftControlledDT(const iftMImage *mimg, iftAdjRel *A, iftLabeledSet *seeds, int threshold)
{
    iftImage *label   = iftCreateImage(mimg->xsize, mimg->ysize, mimg->zsize);
    iftImage *pathval = iftCreateImage(mimg->xsize, mimg->ysize, mimg->zsize);
    iftImage *root = iftCreateImage(mimg->xsize, mimg->ysize, mimg->zsize);

    iftDynamicSet **S = (iftDynamicSet **) iftAlloc(mimg->n, sizeof (*S));

    int max_val = iftMMaximumValue(mimg, -1);
    iftGQueue *Q = iftCreateGQueue(max_val * max_val * 3, mimg->n, pathval->val);

    for (ulong p = 0; p < mimg->n; p++)
    {
        pathval->val[p] = IFT_INFINITY_INT;
        root->val[p] = IFT_NIL;
    }

    for (iftLabeledSet *M = seeds; M != NULL; M = M->next)
    {
        int p = M->elem;
        label->val[p] = M->label;
        pathval->val[p] = 0;
        root->val[p] = p;
        iftInsertGQueue(&Q, p);
        S[p] = iftCreateDynamicSet(mimg->m);
        iftInsertDynamicSet(S[p], mimg, p);
    }

    while (!iftEmptyGQueue(Q))
    {
        int p = iftRemoveGQueue(Q);
        iftVoxel u = iftMGetVoxelCoord(mimg, p);

        if (root->val[p] == p) {
            pathval->val[p] = 0;
        } else /* roots have already been inserted */
            iftInsertDynamicSet(S[root->val[p] ], mimg, p);

        for (int i = 1; i < A->n; i++)
        {
            iftVoxel v = iftGetAdjacentVoxel(A, u, i);

            if (iftMValidVoxel(mimg, v))
            {
                int q = iftMGetVoxelIndex(mimg, v);

                if (Q->L.elem[q].color != IFT_BLACK) {
                    double arc_weight = iftDistDynamicSetMImage(S[root->val[p]], mimg, q);

                    int tmp = iftMax(pathval->val[p], arc_weight);

                    if (tmp < pathval->val[q] && tmp < threshold)
                    {
                        if (Q->L.elem[q].color == IFT_GRAY)
                            iftRemoveGQueueElem(Q, q);

                        label->val[q] = label->val[p];
                        pathval->val[q] = tmp;
                        root->val[q] = root->val[p];
                        iftInsertGQueue(&Q, q);
                    }
                }
            }
        }
    }

    iftDestroyImage(&pathval);
    iftDestroyImage(&root);
    for (iftLabeledSet *M = seeds; M != NULL; M = M->next) {
        int p = M->elem;
        if (S[p] != NULL) {
            iftDestroyDynamicSet(&S[p]);
        }
    }
    iftFree(S);
    iftDestroyGQueue(&Q);

    return label;
}

iftImage *iftControlledWatershed(iftImage *basins, iftLabeledSet *seeds, int h) {
    iftImage *label = iftCreateImageFromImage(basins);
    iftImage *cost  = iftCreateImageFromImage(basins);
    iftImage *root  = iftCreateImageFromImage(basins);
    iftImage *dist  = iftCreateImageFromImage(basins);
    //iftImage *pred  = iftCreateImageFromImage(basins);
    iftGQueue *Q = iftCreateGQueue(iftMaximumValue(basins) + 1, cost->n, cost->val);
    iftAdjRel *A = iftSpheric(1.0);

    for (int p = 0; p < cost->n; p++) {
        root->val[p]  = p;
        label->val[p] = 0;
        dist->val[p]  = 0;
        cost->val[p]  = IFT_INFINITY_INT;
    }

    iftLabeledSet *S = seeds;
    while (S) {
        int p = S->elem;
        label->val[p] = S->label;
        cost->val[p]  = 0;
        iftInsertGQueue(&Q, p);
        S = S->next;
    }

    while (!iftEmptyGQueue(Q)) {
        int p = iftRemoveGQueue(Q);
        iftVoxel u = iftGetVoxelCoord(basins, p);
        for (int i = 1; i < A->n; i++) {
            iftVoxel v = iftGetAdjacentVoxel(A, u, i);
            if (iftValidVoxel(basins, v)) {
                int q = iftGetVoxelIndex(basins, v);

                int tmp = iftMax(cost->val[p], basins->val[q]);

                if (tmp < cost->val[q] && tmp < h) {
                    if (Q->L.elem[q].color == IFT_GRAY)
                    {
                      iftRemoveGQueueElem(Q, q);
                    }
                    dist->val[q]     = dist->val[p] + 1;
                    root->val[q]     = root->val[p];
                    label->val[q]    = label->val[p];
                    cost->val[q]     = tmp;
                    iftInsertGQueue(&Q, q);
                }

            }
        }
    }

    iftDestroyImage(&dist);
    iftDestroyImage(&cost);
    iftDestroyImage(&root);
    iftDestroyGQueue(&Q);
    return label;
}

QList<iftImage*> Controlled::generateLabel()
{


    iftLabeledSet *seeds = nullptr;
    const iftImage *markers = view->annotation->getMarkers();

    updateProgress(0.15, "Performing watershed transform.");

    for (int i = 0; i < markers->n; i++) {
        if (markers->val[i] != UNMARKED) {
            iftInsertLabeledSet(&seeds, i, markers->val[i]);
        }
    }

    QList<iftImage*> labels;
    int tsize = view->getImageSequence()->tsize;
    float progress = 0.15;

    if(ui->cbPropagate->isChecked()) {
        for(int t = 0; t < tsize; t++) {
            labels.append(propagateMarkersThroughTime(seeds,t));
            progress+=0.55/tsize;
            updateProgress(progress, QString("Propagating %1th image.").arg(t+2));
        }
    }
    else {
        int t = parent->currentTime();
        labels.append(propagateMarkersThroughTime(seeds,t));
    }

    updateProgress(0.7);

    return labels;
}

iftImage *Controlled::propagateMarkersThroughTime(iftLabeledSet *seeds, int t)
{
    const iftImageSequence *imgs = view->getImageSequence();
    iftImage *img  = iftExtractVolumeFromImageSequence(imgs, t);

    iftAdjRel *A   = iftSpheric(1.0);
    iftImage *grad = nullptr;

    if (dynamic_cast<Brain*>(parent->currentArcWeightFunction) != nullptr) {
        grad = iftBrainGrad(img);
        iftDestroyImage(&img);
    } else {
        iftMImage *mimg = iftImageToMImage(img, GRAYNorm_CSPACE);
        iftAdjRel *B = iftSpheric(view->getGradientAdjRelRadius());
        grad = MImageGradient(mimg, B);
        iftDestroyMImage(&mimg);
    }

    int h = ui->sbThreshold->value();
    iftImage *label = iftControlledWatershed(grad, seeds, h);

    // deallocating memory
    iftDestroyImage(&img);
    iftDestroyImage(&grad);
    iftDestroyAdjRel(&A);

    return label;
}
