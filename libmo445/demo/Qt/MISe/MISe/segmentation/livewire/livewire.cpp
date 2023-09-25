#include "livewire.h"
#include "ui_livewire.h"
#include <QDebug>
#include <exception>
#include <segmentation/gradient/brain/brain.h>
#include <segmentation/gradient/saliency/saliency.h>
#include <views/slice/sliceview.h>

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
    slice = -1;
    kpoints = 0;
    currentSlice = nullptr;
    imageTmp = nullptr;
    saliencyImage = nullptr;

    connect(ui->pbDone, SIGNAL(clicked()), this, SLOT(done()));
    connect(ui->pbUndo, SIGNAL(clicked()), this, SLOT(undo()));
    connect(ui->pbStart, SIGNAL(clicked()), this, SLOT(start()));
    connect(ui->pbReset, SIGNAL(clicked()), this, SLOT(reset()));


    QAction *foo = new QAction(this);
    foo->setShortcut(Qt::CTRL | Qt::Key_Z);

    connect(foo, SIGNAL(triggered()), this, SLOT(undo()));
    this->addAction(foo);
}

LiveWire::~LiveWire()
{
    reset();
    iftDestroyImage(&currentSlice);
    delete ui;
}


int *OptimumPath(iftImage *img, const iftImage *saliencyImage, int p1, int p2, int *old_path, LivewireOrientation orientation)
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

    if (saliencyImage == nullptr)
        saliencyImage = img;

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

    bool path_found = false;
    while (!iftEmptyGQueue(Q)){
        int p      = iftRemoveGQueue(Q);
        iftVoxel u = iftGetVoxelCoord(img,p);

        if (p == p2) {
            path_found = true;
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
                int tmp, w;

                w = Cmax - abs(img->val[left]-img->val[right]);

                if ((orientation == NONE) ||
		    ((orientation == CLOCKWISE) &&
		     (saliencyImage->val[right] > saliencyImage->val[left])) ||
                    ((orientation == COUNTERCLOCKWISE &&
		      saliencyImage->val[left] > saliencyImage->val[right]))){
		  tmp = cost->val[p] + iftRound(pow(w,1.5));
		} else {               
		  tmp = cost->val[p] + w;
		}
		
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

    int *path;
    if (!path_found) {
        path = iftAllocIntArray(1);
    } else {
        int p = p2, pathsize=1;
        while (pred->val[p] != IFT_NIL){
            pathsize++;
            p  = pred->val[p];
        }

        path = iftAllocIntArray(pathsize+1);
        path[0]   = pathsize;
        p = p2;
        int i=1;
        while (pred->val[p] != IFT_NIL) {
            path[i] = p;
            p = pred->val[p];
            i++;
        }
        path[i] = p;
    }

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

int *CopyPath(int *path) {
    int *newpath = iftAllocIntArray(path[0]+1);
    newpath[0] = path[0];
    for (int i=1; i <= path[0]; i++) {
        newpath[i] = path[i];
    }
    return newpath;
}

int *DisconnectPaths(int *src, int *removal)
{
    int *newpath = iftAllocIntArray(src[0]-removal[0]+1);

    for (int i=1; i <= removal[0]; i++) {
        for (int j=1; j <= src[0]; j++)
            if (removal[i] == src[j]) {
                src[j] = -1;
                break;
            }
    }

    int k = 1;
    for (int i=1; i <= src[0]; i++) {
        if (src[i] >= 0) {
            newpath[k] = src[i];
            k++;
        }
    }
    newpath[0] = src[0]-removal[0];

    iftFree(src);
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

QList<iftImage*> LiveWire::generateLabel()
{
    iftImage *border = iftCreateImageFromImage(view->getImage());
    iftImage *mask    = iftCreateImageFromImage(view->getImage());
    iftSet   *border_set = NULL;
    iftAdjRel *A = iftCircular(1.42);
    for (int p = 0; p < currentSlice->n; p++) {
        iftVoxel u = iftGetVoxelCoord(currentSlice, p);
        iftVoxel w = view->orientation->mapPixelToVolume(u, currentSliceNumber, slice);
        int q = iftGetVoxelIndex(mask, w);
        mask->val[q] = 1;
        for (int i = 0; i < A->n; i++) {
            iftVoxel v = iftGetAdjacentVoxel(A, u, i);
            if (!iftValidVoxel(currentSlice, v)) {
                iftInsertSet(&border_set, q);
                break;
            }
        }
    }
    iftDestroyAdjRel(&A);
    for (int i = 1; i <= path[0]; i++) {
        iftVoxel v = iftGetVoxelCoord(currentSlice, path[i]);
        v.z = 1;
        iftVoxel u = view->orientation->mapPixelToVolume(v, currentSliceNumber, slice);
        int p = iftGetVoxelIndex(border, u);
        border->val[p] = 1;
    }

    iftImage *label_livewire = iftCloseBasins(border, border_set, mask);
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
    iftDestroyImage(&mask);
    iftDestroySet(&border_set);

    return {label_livewire};
}

void LiveWire::notifyGradientUpdate(ArcWeightFunction *function)
{
    this->currentArcWeightFunction = function;
}

void LiveWire::renderGraphics(iftImage *orig, iftImage *image, QGraphicsPixmapItem *imagePixmap, int sliceType)
{
    if (started && kpoints && slice == sliceType) {
        origImage[sliceType] = orig;
        origRenderImage[sliceType] = image;
        this->imagePixmap[sliceType] = imagePixmap;
        paintPath(path);
    }
}

bool LiveWire::mouseClickGraphics(int x, int y, Qt::MouseButtons bts, Qt::KeyboardModifiers, int sliceType)
{
    static int bla = 0;
    if (slice < 0) {
        slice = sliceType;
    }
    if (started && (x != lastPointClicked.x || y != lastPointClicked.y) && slice == sliceType) {
        qDebug() << bla++;
        if (bts.testFlag(Qt::RightButton))
            done();
        else if (bts.testFlag(Qt::MiddleButton))
            undo();
        else {
            slice = sliceType;
            if (currentSlice == nullptr) {
                iftImage *tmp = view->getCurrentSlice(imageTmp, slice);
                currentSlice = view->orientation->applyOrientationOnSlice(tmp, sliceType);
                iftDestroyImage(&tmp);
                currentSliceNumber = view->getCurrentSliceNumber(slice);

            }
            if (kpoints > 0) {
                iftVoxel v = {x,y,1,0};
                iftVoxel u = lastPointClicked;

                int p = iftGetVoxelIndex(currentSlice, u);
                int q = iftGetVoxelIndex(currentSlice, v);
                if (kpoints == 1) {
                    path = OptimumPath(currentSlice, saliencyImage, p, q, NULL, currentOrientation);
                    segments.append(CopyPath(path));
                } else {
                    //if (PathContainsPoint(path, q))
                    int *newpath = OptimumPath(currentSlice, saliencyImage, p, q, path, currentOrientation);
                    path = ConcatPaths(path, newpath);
                    segments.append(newpath);
                }
                paintPath(path);
            } else {
                firstPointClicked = {x,y,1,0};
            }
            lastPointClicked = {x, y,1,0};
            kpoints++;
            ui->pbUndo->setEnabled(true);
            //if (path && IsClosedPath(currentSlice, path))
            //    done();
        }
        return true;
    } else return false;
}

bool LiveWire::mouseMoveGraphics(int x, int y, int sliceType)
{
    if (started && slice == sliceType && kpoints) {
        iftVoxel v = {x,y,1,0};
        iftVoxel u = lastPointClicked;
        int p = iftGetVoxelIndex(currentSlice, u);
        int q = iftGetVoxelIndex(currentSlice, v);

        int *newpath = OptimumPath(currentSlice, saliencyImage, p, q, path, currentOrientation);
        if (path) {
            newpath = ConcatPaths(newpath, path);
        }

        paintPath(newpath);

        iftFree(newpath);
        return true;
    } else return false;

}

void LiveWire::start()
{
    Saliency *saliency = dynamic_cast<Saliency*>(currentArcWeightFunction);
    //ui->rbSalience->setEnabled(saliency != nullptr);

    saliencyImage = nullptr;
    if (saliency) {
        try {
            view->getGradient();
        } catch (QString error) {
            QMessageBox::warning((QWidget*) parent,
                                 tr("Warning"),
                                 error + " Could not load saliency image.");
            return;
        }
        saliencyImage = saliency->getSaliencyImage();
    } else if (ui->cbImageAsSaliency->isChecked()){
//        QMessageBox::warning((QWidget*) parent,
//                             tr("Warning"),
//                             tr("Loading the image as saliency image since none was specified."));
        saliencyImage = view->getImage();
    }

    iftImage *grad = nullptr;
    if (view->getGradient() == nullptr)
        parent->slotCalculateGradient();
    const iftMImage *mimg = view->getGradient();
    // TODO refactor
    if (dynamic_cast<Brain*>(parent->currentArcWeightFunction) != nullptr) {
        const iftImage *img = view->getImage();
        grad = iftBrainGrad(img);
    } else {
        iftAdjRel *B = iftSpheric(view->getGradientAdjRelRadius());
        grad = MImageGradient(mimg, B);
        iftDestroyAdjRel(&B);
    }
    iftDestroyImage(&imageTmp);
    imageTmp = grad;

    started = true;

    if (ui->rbNoOrientaion->isChecked())
        currentOrientation = NONE;
    else if (ui->rbCounterClockwise->isChecked())
        currentOrientation = CLOCKWISE;
    else
        currentOrientation = COUNTERCLOCKWISE;

    ui->pbStart->setEnabled(false);
    ui->pbReset->setEnabled(true);
    ui->pbDone->setEnabled(true);
    ui->pbUndo->setEnabled(false);
    ui->gbOrientation->setEnabled(false);
}

void LiveWire::reset()
{
    ui->pbStart->setEnabled(true);
    ui->pbReset->setEnabled(false);
    ui->pbDone->setEnabled(false);
    ui->pbUndo->setEnabled(false);
    ui->gbOrientation->setEnabled(true);

    if (kpoints) {
        SliceView::updateSlicePixmap(imagePixmap[slice], origRenderImage[slice]);
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

    slice = -1;
    kpoints = 0;
    started = false;
    if (path)
        iftFree(path);
    for (int *segment: segments) {
        iftFree(segment);
    }
    segments.clear();
    path = nullptr;
    iftDestroyImage(&currentSlice);
}

void LiveWire::undo()
{
    if (started) {
        if (kpoints <= 1)
            reset();
        else {
            int *last_segment = segments.last();
            int last_point_index = last_segment[last_segment[0]];
            lastPointClicked = iftGetVoxelCoord(currentSlice, last_point_index);
            path = DisconnectPaths(path, last_segment);
            iftFree(last_segment);

            segments.removeLast();
            kpoints--;
            paintPath(path);
        }
    }
}

void LiveWire::done()
{
    if (kpoints >= 2) {

        int p = iftGetVoxelIndex(currentSlice, firstPointClicked);
        int q = iftGetVoxelIndex(currentSlice, lastPointClicked);
        int *newpath = OptimumPath(currentSlice, saliencyImage, q, p, NULL, currentOrientation);
        if (path) {
            path = ConcatPaths(path, newpath);
        }
        segments.append(newpath);

        paintPath(path);

        QMessageBox::StandardButton bt_clicked = QMessageBox::question(nullptr, tr("Accept label"),
                             "Would you like to add this label map to your current label object?",
                             QMessageBox::Yes | QMessageBox::No);

        if (bt_clicked == QMessageBox::Yes) {
            execute();
        }
    }

    reset();
}

void LiveWire::paintPath(int *path)
{
    if (path) {
        iftDestroyImage(&tmpRenderImage[slice]);
        tmpRenderImage[slice] = iftCopyImage(origRenderImage[slice]);
        for (int i = 1; i <= path[0]; i++) {
            iftColor yCbCr = iftRGBtoYCbCr({{255,0,0},1},255);
            tmpRenderImage[slice]->val[path[i]] = yCbCr.val[0];
            tmpRenderImage[slice]->Cb[path[i]] = yCbCr.val[1];
            tmpRenderImage[slice]->Cr[path[i]] = yCbCr.val[2];
        }
        SliceView::updateSlicePixmap(imagePixmap[slice], tmpRenderImage[slice]);
    }
}
