#include "marker.h"

#include <QMessageBox>

#include <views/view.h>

Marker::Marker(iftImageSequence *img)
{
    radius = img->ysize * 0.0125;
    sphere = false;
    markers = iftCreateImage(img->xsize, img->ysize, img->zsize);
    removalMarkers = iftCreateImage(img->xsize, img->ysize, img->zsize);
    brush = iftCircular(radius);
    annotationMode = HALT_ANNOTATION;
    nmarkers = 0;

    bb.begin.x = -1;
    bb.begin.y = -1;
    bb.begin.z = -1;

    bb.end.x = -1;
    bb.end.y = -1;
    bb.end.z = -1;

    bbIndex = 0;

    iftInitMarkersImage(markers);

    lastPoint = nullptr;
}

Marker::~Marker()
{
    iftDestroyAdjRel(&this->brush);
    iftDestroyImage(&markers);
    iftDestroyImage(&removalMarkers);
    if (lastPoint != nullptr)
        delete lastPoint;
}

const iftImage *Marker::getMarkers() const
{
    //iftImage *out = iftCopyImage(this->markers);
    iftImage *out = this->markers;
    return out;
}

void Marker::setMarkers(const iftImage *img)
{
    if (img == nullptr)
        return;
    iftImage *m = iftCopyImage(img);
    this->markers = m;
}

void Marker::resetMarkers()
{
    #pragma omp parallel for
    for (int i = 0; i < markers->n; i++) {
        markers->val[i] = UNMARKED;
        removalMarkers->val[i] = 0;
    }
}

void Marker::setBrushRadius(float new_radius)
{
    this->radius = new_radius;
    iftDestroyAdjRel(&this->brush);
    this->brush = sphere? iftSpheric(this->radius) : iftCircular(this->radius);
}

float Marker::getBrushRadius()
{
    return this->radius;
}

iftAdjRel *Marker::getBrush()
{
    iftAdjRel *A = iftCopyAdjacency(this->brush);
    return A;
}

void Marker::setSphericity(bool sphere)
{
    this->sphere = sphere;
    iftDestroyAdjRel(&this->brush);
    this->brush = sphere? iftSpheric(this->radius) : iftCircular(this->radius);
}

void Marker::setAnnotationMode(char mode)
{
    this->annotationMode = mode;
}

char Marker::getAnnotationMode()
{
    return this->annotationMode;
}

void Marker::paintMarkersOnQImage(Orientation *orientation, QImage *img, QPoint point, int markerLabel, int sliceType, int sliceNum, bool erasing)
{
    iftAdjRel *A = this->getBrush();
    iftVoxel u, v, w;
    u.x = point.x();
    u.y = point.y();

    QPoint *lastPoint = this->lastPoint;
    if (lastPoint == nullptr)
        lastPoint = &point;

    float dx = lastPoint->x() - point.x();
    float dy = lastPoint->y() - point.y();

    float step = abs(dx) >= abs(dy)? abs(dx) : abs(dy);

    dx = step==0? dx : dx/step;
    dy = step==0? dy : dy/step;

    // paint a line between the two consecutive points
    int k = 1;
    float x = point.x(), y = point.y();

    // workaround to paint a single point
    if (step == 0)
        step = 1;

    while (k <= step) {
        u.x = int(x);
        u.y = int(y);
        u.z = 0;

        // paint the adjacent relation
        for (int i = 0; i < A->n; i++){
            v = iftGetAdjacentVoxel(A,u,i);
            if ((v.x >= img->width()) || (v.x < 0) || (v.y >= img->height()) || (v.y < 0))
                continue;
            w = orientation->mapPixelToVolume(v, sliceNum + v.z, sliceType);

            if (erasing)
                this->eraseMarker(w);
            else
                this->paintMarker(w, markerLabel);

            iftColor c = iftYCbCrtoRGB(this->getMarkerColor(markerLabel), 255);
            img->setPixel(v.x, v.y, QColor(c.val[0], c.val[1], c.val[2]).lighter(125).rgb());
        }

        x += dx;
        y += dy;
        k++;
    }

    iftDestroyAdjRel(&A);
    this->setLastPoint(&point);
}

void selectConnectedComponent(int t, const iftImage *in, iftImage *out, ForestWrapper *forest = nullptr) {
    iftFIFO *fifo = iftCreateFIFO(in->n);
    iftAdjRel *A = iftSpheric(1.733);

    out->val[t] = 1 - out->val[t];
    iftInsertFIFO(fifo, t);

    while (!iftEmptyFIFO(fifo)) {
        int p = iftRemoveFIFO(fifo);
        iftVoxel u = iftGetVoxelCoord(in, p);
        for (int i = 1; i < A->n; i++) {
            iftVoxel v = iftGetAdjacentVoxel(A, u, i);
            int q = iftGetVoxelIndex(in, v);
            if (fifo->color[q] == IFT_WHITE) {
                if (iftValidVoxel(in, v)) {
                    if (in->val[p] == in->val[q]) {
                        out->val[q] = 1 - out->val[q];
                        // TODO: IFT_GRAY?
                        //if (fifo->color[q] != IFT)
                        iftInsertFIFO(fifo, q);
                    }
                }
            }
        }
    }

    if (forest != nullptr) {
        #pragma omp parallel for
        for (int i = 0; i < in->n; i++) {
            if (fifo->color[forest->root()->val[i]] != IFT_WHITE && in->val[i] == UNMARKED) {
                out->val[i] = 1 - out->val[i];
            }
        }
    }

    iftDestroyAdjRel(&A);
    iftDestroyFIFO(&fifo);
}

void selectConnectedComponent(int t, ForestWrapper *forest, iftImage *out) {
    iftFIFO *fifo = iftCreateFIFO(out->n);
    iftAdjRel *A = iftSpheric(1.733);

    out->val[t] = 1 - out->val[t];
    iftInsertFIFO(fifo, t);

    while (!iftEmptyFIFO(fifo)) {
        int p = iftRemoveFIFO(fifo);
        iftVoxel u = iftGetVoxelCoord(forest->label(), p);
        for (int i = 1; i < A->n; i++) {
            iftVoxel v = iftGetAdjacentVoxel(A, u, i);
            int q = iftGetVoxelIndex(forest->label(), v);
            if (fifo->color[q] == IFT_WHITE) {
                if (iftValidVoxel(forest->label(), v)) {
                    if (forest->root()->val[p] == forest->root()->val[q]) {
                        out->val[q] = 1 - out->val[q];
                        // TODO: IFT_GRAY?
                        //if (fifo->color[q] != IFT)
                        iftInsertFIFO(fifo, q);
                    }
                }
            }
        }
    }

    iftDestroyFIFO(&fifo);
    iftDestroyAdjRel(&A);

    //selectConnectedComponent(forest->root->val[t], forest->marker)
}

void Marker::selectPointForDeletion(iftVoxel u, const iftImage *, ForestWrapper *forest)
{
    int p = iftGetVoxelIndex(markers, u);

    if (markers->val[p] == UNMARKED) {
        if (forest != nullptr) {
            int root = forest->root()->val[p];

            if (markers->val[root] != UNMARKED)
                selectConnectedComponent(root, markers, removalMarkers, forest);
        }
    } else {
        selectConnectedComponent(p, markers, removalMarkers, forest);
    }

}

void Marker::paintMarker(iftVoxel v, int label)
{
    if (iftValidVoxel(markers,v)){
        int p = iftGetVoxelIndex(markers,v);
        markers->val[p] = label;
        removalMarkers->val[p] = 0;
    }
}

void iftFillRegionInImage(iftImage *img, iftBoundingBox bb, int value)
{
    int xBegin, xEnd;
    int yBegin, yEnd;
    int zBegin, zEnd;

    xBegin = iftMin(bb.begin.x,bb.end.x);
    xEnd = iftMax(bb.begin.x,bb.end.x);

    yBegin = iftMin(bb.begin.y,bb.end.y);
    yEnd = iftMax(bb.begin.y,bb.end.y);

    zBegin = iftMin(bb.begin.z,bb.end.z);
    zEnd = iftMax(bb.begin.z,bb.end.z);

    #pragma omp parallel for
    for (int z = zBegin; z <= zEnd; z++)
        for (int y = yBegin; y <= yEnd; y++)
            for (int x = xBegin; x <= xEnd; x++)
                iftImgVal(img, x, y, z) = value;

}

void Marker::paintBox(iftVoxel v, int label)
{
    if ((bbIndex == 1) && (iftValidVoxel(markers,v))){
        bb.end.x = v.x;
        bb.end.y = v.y;
        bb.end.z = v.z;
        iftFillRegionInImage(markers,bb,label);
        bbIndex = 0;
    } else if ((bbIndex == 0) && (iftValidVoxel(markers,v))){
        bb.begin.x = v.x;
        bb.begin.y = v.y;
        bb.begin.z = v.z;
        bbIndex = 1;
    }
}

void Marker::eraseMarker(iftVoxel v)
{
    if (iftValidVoxel(markers,v)){
        int p = iftGetVoxelIndex(markers,v);
        markers->val[p] = UNMARKED;
    }
}

void Marker::removeMarker(int markerLabel)
{
    // remotion of background
    if (markerLabel == 0) {
        #pragma omp parallel for
        for (int i = 0; i < markers->n; i++){
            if (markers->val[i] == markerLabel)
                markers->val[i] = UNMARKED;
        }
    } else {

        #pragma omp parallel for
        for (int i = 0; i < markers->n; i++){
            if (markers->val[i] == markerLabel)
                markers->val[i] = UNMARKED;
            //else if (markers->val[i] > markerLabel)
              //  markers->val[i] = markers->val[i] - 1;
        }

        auto it = std::find_if(markerInfoArray.begin(),  markerInfoArray.end(),
                          [markerLabel] (const MarkerInformation& m) -> bool { return markerLabel == m.label(); });
        markerInfoArray.erase(it);

        nmarkers--;
    }
}

void Marker::setLastPoint(QPoint *newPoint)
{
    if (lastPoint != nullptr)
        delete lastPoint;
    if (newPoint != nullptr)
        lastPoint = new QPoint(newPoint->x(), newPoint->y());
    else
        lastPoint = nullptr;
}

const QPoint *Marker::getLastPoint() const
{
    return lastPoint;
}

const iftImage *Marker::getRemovalMarkers() const
{
    return removalMarkers;
}

int Marker::getNextLabel()
{
    return markerInfoArray.maxLabel() + 1;
}

int Marker::getLabel(int index)
{
    return markerInfoArray[index].label();
}

iftColor Marker::getMarkerColor(int marker)
{
    for (MarkerInformation &markerInfo: markerInfoArray) {
        if (markerInfo.label() == marker) {
            return markerInfo.color();
        }
    }
    return {{0,0,0},0};
}

MarkerInformationVector &Marker::getMarkerInfoArray()
{
    return markerInfoArray;
}

void Marker::setMarkerName(int index, QString name)
{
    markerInfoArray[index].setName(name);
}

//TODO use this method in other places
int Marker::getIndexFromLabel(int label)
{
    for (int i = 0; i < markerInfoArray.size(); i++) {
        if (label == markerInfoArray[i].label()) {
            return i;
        }
    }
    return -1;
}

void Marker::setMarkerLabel(int old_label, int label)
{
    auto it = std::find_if(markerInfoArray.begin(),  markerInfoArray.end(),
                      [old_label] (const MarkerInformation& m) -> bool { return old_label == m.label(); });

    if (it != markerInfoArray.end()) {
        it->setLabel(label);
        #pragma omp parallel for
        for (int i = 0; i < markers->n; i++) {
            if (markers->val[i] == old_label)
                markers->val[i] = label;
        }

    }
}

iftColorTable *Marker::generateColorTable()
{
    if (nmarkers == 0)
        return nullptr;
    iftColorTable *t = iftCreateColorTable(markerInfoArray.maxLabel() + 1);
    //TODO this returns all values, it shouldnt
    for (MarkerInformation &markerInfo: markerInfoArray) {
        int l = markerInfo.label();
        t->color[l].val[0] = markerInfo.color().val[0];
        t->color[l].val[1] = markerInfo.color().val[1];
        t->color[l].val[2] = markerInfo.color().val[2];
    }
//    for (int i = 0; i < t->ncolors; i++){
//        t->color[i].val[0] = markerInfoArray[i].color.val[0];
//        t->color[i].val[1] = markerInfoArray[i].color.val[1];
//        t->color[i].val[2] = markerInfoArray[i].color.val[2];
//    }

    return t;
}

iftIntArray *Marker::generateMarkersVisibility()
{
    iftIntArray* visibility = iftCreateIntArray(markerInfoArray.maxLabel() + 1);
    for (MarkerInformation &markerInfo: markerInfoArray) {
        visibility->val[markerInfo.label()] = markerInfo.isVisible();
    }
    return visibility;
}

void Marker::setMarkerVisibility(int marker, int visibility){
    for (MarkerInformation &markerInfo: markerInfoArray) {
        if (markerInfo.label() == marker) {
            markerInfo.setVisible(visibility);
            return;
        }
    }
}

//TODO marker instead of object?
void Marker::setObjectColorInColorTable(int obj, iftColor color)
{
    for (MarkerInformation &markerInfo: markerInfoArray) {
        if (markerInfo.label() == obj) {
            markerInfo.setColor(color);
            return;
        }
    }
}

void Marker::destroyColorTable()
{
    markerInfoArray.clear();
    nmarkers = 0;
}

void Marker::addItemInColorTable(int label, iftColor *c)
{
    if (label == -1)
        label = getNextLabel();

    MarkerInformation mi(label);
    if (c != nullptr) {
        mi.setColor(*c);
    }
    markerInfoArray.append(mi);

    nmarkers++;
}
