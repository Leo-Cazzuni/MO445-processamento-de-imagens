#include "livewire.h"
#include "ui_livewire.h"
#include <QDebug>
#include <exception>
#include <segmentation/gradient/saliency/saliency.h>

LiveWire::LiveWire(MainWindow *parent, View *view) :
    Segmentation(parent, view),
    ui(new Ui::LiveWire)
{
    _name = "Livewire";
    ui->setupUi(this);
    started = false;
    tmpRenderImage[AXIAL] = nullptr;
    tmpRenderImage[SAGITTAL] = nullptr;
    tmpRenderImage[CORONAL] = nullptr;
    imagePixmap[AXIAL] = nullptr;
    imagePixmap[SAGITTAL] = nullptr;
    imagePixmap[CORONAL] = nullptr;
    path = NULL;
    kpoints = 0;
    saliency = nullptr;
    currentSlice = nullptr;
    imageTmp = nullptr;

    connect(ui->pbDone, SIGNAL(clicked()), this, SLOT(done()));
    connect(ui->pbStart, SIGNAL(clicked()), this, SLOT(start()));
    connect(ui->pbReset, SIGNAL(clicked()), this, SLOT(reset()));
    connect(ui->rbNoOrientaion, SIGNAL(toggled(bool)), this, SLOT(changeOrientationMethod()));
    connect(ui->rbForce, SIGNAL(toggled(bool)), this, SLOT(changeOrientationMethod()));
    connect(ui->rbSalience, SIGNAL(toggled(bool)), this, SLOT(changeOrientationMethod()));
}

LiveWire::~LiveWire()
{
    reset();
    iftDestroyImage(&currentSlice);
    delete ui;
}


int *OptimumPath(iftImage *img, int p1, int p2, int *old_path, LivewireOrientation orientation)
{
    iftGQueue *Q=NULL;
    iftImage *cost, *pred;
    int Cmax = iftMaximumValue(img);
    iftAdjRel *A = iftCircular(1.0);
    iftAdjRel *L = iftLeftSide(A,1.0);
    iftAdjRel *R = iftRightSide(A,1.0);

    cost = iftCreateImageFromImage(img);
    pred = iftCreateImageFromImage(img);
    Q    = iftCreateGQueue(iftRound(pow(Cmax,1.5))+1,img->n,cost->val);

    iftSetImage(cost,IFT_INFINITY_INT);
    if (old_path != NULL) {
        for (int i = 1; i <= old_path[0]; i++) {
            cost->val[old_path[i]] = 0;
        }
    }
    pred->val[p1] = IFT_NIL;
    cost->val[p1] = 0;
    cost->val[p2] = IFT_INFINITY_INT;
    iftInsertGQueue(&Q,p1);

    while (!iftEmptyGQueue(Q)){
        int p      = iftRemoveGQueue(Q);
        iftVoxel u = iftGetVoxelCoord(img,p);

        if (p == p2) {
            break;
        }

        for (int i=1; i < A->n; i++) {
            iftVoxel v = iftGetAdjacentVoxel(A,u,i);
            if (iftValidVoxel(img,v)){
                int q     = iftGetVoxelIndex(img,v);
                v         = iftGetAdjacentVoxel(L,u,i);
                if (!iftValidVoxel(img, v))
                    continue;
                int left  = iftGetVoxelIndex(img,v);
                v         = iftGetAdjacentVoxel(R,u,i);
                if (!iftValidVoxel(img, v))
                    continue;
                int right = iftGetVoxelIndex(img,v);
                int tmp, w = Cmax - abs(img->val[left]-img->val[right]);

                if (orientation == NONE ||
                        (orientation == CLOCKWISE && img->val[left] > img->val[right]) ||
                        (orientation == COUNTERCLOCKWISE && img->val[left] < img->val[right]))
                    tmp = cost->val[p] + w;
                else
                    tmp = cost->val[p] + iftRound(pow(w,1.5));


                if (tmp < cost->val[q]){
                    if (Q->L.elem[q].color == IFT_GRAY)
                        iftRemoveGQueueElem(Q,q);
                    cost->val[q] = tmp;
                    pred->val[q] = p;
                    iftInsertGQueue(&Q,q);
                }
            }
        }
    }

    int p = p2, pathsize=1;
    while (pred->val[p] != IFT_NIL){
        pathsize++;
        p  = pred->val[p];
    }

    int *path = iftAllocIntArray(pathsize+1);
    path[0]   = pathsize;
    p = p2;
    int i=1;
    while (pred->val[p] != IFT_NIL) {
        path[i] = p;
        p = pred->val[p];
        i++;
    }
    path[i] = p;

    iftDestroyGQueue(&Q);
    iftDestroyImage(&pred);
    iftDestroyImage(&cost);
    iftDestroyAdjRel(&A);
    iftDestroyAdjRel(&L);
    iftDestroyAdjRel(&R);

    return(path);
}

int *ConcatPaths(int *path1, int *path2)
{
    int *newpath = iftAllocIntArray(path1[0]+path2[0]+1);

    newpath[0] = path1[0]+path2[0];
    for (int i=1; i <= path1[0]; i++)
        newpath[i] = path1[i];
    for (int i=1; i <= path2[0]; i++)
        newpath[i+path1[0]] = path2[i];

    iftFree(path1);
    return(newpath);
}

bool IsClosedPath(iftImage *img, int *path) {
    // TODO efficient implementation


    //for (int i=1; i <= path[0]; i++)
      //  if (path[i] == p)
        //    return true;

    iftImage *border = iftCreateImageFromImage(img);
    for (int i = 1; i <= path[0]; i++) {
        iftVoxel v = iftGetVoxelCoord(img, path[i]);
        int p = iftGetVoxelIndex(border, v);
        border->val[p] = 1;
    }

    iftImage *basins_closed = iftCloseBasins(border, NULL, NULL);

    bool is = false;
    if (iftCountObjectSpels(border, 1) != iftCountObjectSpels(basins_closed, 1))
        is = true;

    iftDestroyImage(&basins_closed);
    iftDestroyImage(&border);

    return is;
}

iftImage *LiveWire::generateLabel()
{
    iftImage *border = iftCreateImageFromImage(view->getImage());
    for (int i = 1; i <= path[0]; i++) {
        iftVoxel v = iftGetVoxelCoord(currentSlice, path[i]);
        Orientation orientation;
        iftVoxel u = orientation.mapPixelToVolume(v, view->getCurrentAxialSliceNum(), slice);
        int p = iftGetVoxelIndex(border, u);
        border->val[p] = 1;
    }

    iftImage *label_livewire = iftCloseBasins(border, NULL, NULL);
    const iftImage *previous_label = view->getLabel();
    if (previous_label) {
        #pragma omp parallel for
        for (int i = 0; i < label_livewire->n; i++) {
            if (!label_livewire->val[i]) {
                label_livewire->val[i] = previous_label->val[i];
            }
        }
    }
    iftDestroyImage(&border);

    return label_livewire;
}

void LiveWire::notifyGradientUpdate(ArcWeightFunction *function)
{
    saliency = dynamic_cast<Saliency*>(function);
    ui->rbSalience->setEnabled(saliency != nullptr);
    if (!saliency) {
        ui->rbNoOrientaion->setChecked(true);
    }
}

void LiveWire::renderGraphics(QImage *image, QGraphicsPixmapItem *imagePixmap, int sliceType)
{
    if (started && kpoints && slice == sliceType) {
        origRenderImage[sliceType] = image;
        this->imagePixmap[sliceType] = imagePixmap;
    }
}

bool LiveWire::mouseClickGraphics(int x, int y, Qt::MouseButtons, Qt::KeyboardModifiers, int sliceType)
{
    if (started && x != lastPointClicked.x && y != lastPointClicked.y) {
        slice = sliceType;
        if (currentSlice == nullptr) {
            currentSlice = view->getCurrentSlice(imageTmp, slice);
            currentSliceNumber = view->getCurrentSliceNumber(slice);
        }
        if (kpoints > 0) {
            iftVoxel v = {x,y,0,0};
            iftVoxel u = lastPointClicked;

            int p = iftGetVoxelIndex(currentSlice, u);
            int q = iftGetVoxelIndex(currentSlice, v);
            if (kpoints == 1)
                path = OptimumPath(currentSlice, p, q, NULL, currentOrientation);
            else {
                //if (PathContainsPoint(path, q))
                int *newpath = OptimumPath(currentSlice, p, q, path, currentOrientation);
                path = ConcatPaths(path, newpath);
                iftFree(newpath);
            }
        } else {
            firstPointClicked = {x,y,0,0};
        }
        lastPointClicked = {x, y,0,0};
        kpoints++;
        if (path && IsClosedPath(currentSlice, path))
            done();
        return true;
    } else return false;
}

bool LiveWire::mouseMoveGraphics(int x, int y, int sliceType)
{
    if (started && slice == sliceType && kpoints) {
        Orientation orientation;
        iftVoxel v = {x,y,0,0};//orientation.mapPixelToVolume({x,y,0,0}, view->getCurrentAxialSliceNum(), sliceType);
        iftVoxel u = lastPointClicked;
        int p = iftGetVoxelIndex(currentSlice, u);
        int q = iftGetVoxelIndex(currentSlice, v);


        if (tmpRenderImage[sliceType])
            delete tmpRenderImage[sliceType];

        int *newpath = OptimumPath(currentSlice, p, q, path, currentOrientation);
        if (path) {
            newpath = ConcatPaths(newpath, path);
        }

        tmpRenderImage[sliceType] = new QImage(*origRenderImage[sliceType]);
        for (int i = 1; i <= newpath[0]; i++) {
            iftVoxel w = iftGetVoxelCoord(currentSlice, newpath[i]);
            tmpRenderImage[sliceType]->setPixelColor(w.x, w.y, Qt::red);
        }
        imagePixmap[sliceType]->setPixmap(QPixmap::fromImage(*tmpRenderImage[sliceType]));
        iftFree(newpath);
        return true;
    } else return false;

}

void LiveWire::start()
{
    if (saliency && ui->rbSalience->isChecked()) {
        try {
            imageTmp = saliency->getSaliencyImage();
        } catch (QString error) {
            QMessageBox::warning((QWidget*) parent,
                                 tr("Warning"),
                                 error + " Could not load saliency image.");
            return;
        }
    } else {
        imageTmp = view->getImage();
    }

    started = true;

    if (ui->rbNoOrientaion->isChecked())
        currentOrientation = NONE;
    else if (ui->rbClockwise->isChecked())
        currentOrientation = CLOCKWISE;
    else
        currentOrientation = COUNTERCLOCKWISE;

    ui->pbStart->setEnabled(false);
    ui->pbReset->setEnabled(true);
    ui->pbDone->setEnabled(true);
    ui->gbOrientation->setEnabled(false);
    ui->widgetOrientationMethod->setEnabled(false);
}

void LiveWire::reset()
{
    ui->pbStart->setEnabled(true);
    ui->pbReset->setEnabled(false);
    ui->pbDone->setEnabled(false);
    changeOrientationMethod();
    ui->widgetOrientationMethod->setEnabled(true);

    if (kpoints) {
        imagePixmap[slice]->setPixmap(QPixmap::fromImage(*origRenderImage[slice]));
    }

    if (tmpRenderImage[AXIAL])
        delete tmpRenderImage[AXIAL];
    tmpRenderImage[AXIAL] = nullptr;
    if (tmpRenderImage[SAGITTAL])
        delete tmpRenderImage[SAGITTAL];
    tmpRenderImage[SAGITTAL] = nullptr;
    if (tmpRenderImage[CORONAL])
        delete tmpRenderImage[CORONAL];
    tmpRenderImage[CORONAL] = nullptr;

    kpoints = 0;
    started = false;
    if (path)
        iftFree(path);
    path = nullptr;
    iftDestroyImage(&currentSlice);
}

void LiveWire::done()
{
    if (kpoints >= 2) {

        int p = iftGetVoxelIndex(currentSlice, firstPointClicked);
        int q = iftGetVoxelIndex(currentSlice, lastPointClicked);
        int *newpath = OptimumPath(currentSlice, q, p, NULL, currentOrientation);
        if (path) {
            path = ConcatPaths(path, newpath);
        }
        iftFree(newpath);

        tmpRenderImage[slice] = new QImage(*origRenderImage[slice]);
        for (int i = 1; i <= path[0]; i++) {
            iftVoxel w = iftGetVoxelCoord(currentSlice, path[i]);
            tmpRenderImage[slice]->setPixelColor(w.x, w.y, Qt::red);
        }
        imagePixmap[slice]->setPixmap(QPixmap::fromImage(*tmpRenderImage[slice]));

        QMessageBox::StandardButton bt_clicked = QMessageBox::question(nullptr, tr("Accept label"),
                             "Would you like to add this label map to your current label object?",
                             QMessageBox::Yes | QMessageBox::No);

        if (bt_clicked == QMessageBox::Yes) {
            execute();
        }
    }

    reset();
}

void LiveWire::changeOrientationMethod()
{
    bool orientationDirectionEnabled = ui->rbForce->isChecked() || ui->rbSalience->isChecked();
    ui->gbOrientation->setEnabled(orientationDirectionEnabled);
}
