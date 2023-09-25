#include "view.h"

View *View::_instance = nullptr;

View::View()
{
    forest = nullptr;

    imgseq = nullptr;
    thumbnails = nullptr;

    currentSegMethod = nullptr;

    img = nullptr;
    label = nullptr;
    border = nullptr;
    projection = nullptr;
    normalized = nullptr;
    rendition = nullptr;
    annotation = nullptr;
    gradient = nullptr;
    gradientCache = nullptr;
    ctb = nullptr;
    minValue = 0;
    maxValue = 0;
    nlabels = 0;
    orientation = new Orientation;
}

View::~View()
{
    iftDestroyImage(&img);
    iftDestroyImageSequence(&imgseq);
    iftDestroyImage(&label);
    iftDestroyMImage(&gradient);
    iftDestroyImage(&gradientCache);
    iftDestroyImage(&border);
    iftDestroyImage(&normalized);
    iftDestroyImage(&projection);
    iftDestroyImageSequence(&thumbnails);
    iftDestroyColorTable(&ctb);
    if (rendition != nullptr)
        delete rendition;
    if (orientation != nullptr)
        delete orientation;
    if (annotation != nullptr)
        delete annotation;
    if (this->forest)
        delete this->forest;
}

View *View::instance()
{
    if (_instance == nullptr)
        _instance = new View();
    return _instance;
}

iftImage * View::getCurrentSlice(const iftImage *img, int sliceType)
{
    switch (sliceType) {
        case AXIAL:
            return iftGetXYSlice(img,currentAxialSliceNum);
        case CORONAL:
            return iftGetZXSlice(img,currentCoronalSliceNum);
        case SAGITTAL:
            return iftGetYZSlice(img,currentSagittalSliceNum);
    }
    return nullptr;
}

int View::getCurrentSliceNumber(int sliceType)
{
    switch (sliceType) {
        case AXIAL:
            return currentAxialSliceNum;
        case CORONAL:
            return currentCoronalSliceNum;
        case SAGITTAL:
            return currentSagittalSliceNum;
    }
    return -1;
}

iftImage *View::getCurrentAxialSlice()
{
    iftImage *i = nullptr;
    i = iftGetXYSlice(img,currentAxialSliceNum);
    return i;
}

iftImage *View::getCurrentCoronalSlice()
{
    iftImage *i = nullptr;
    i = iftGetZXSlice(img,currentCoronalSliceNum);
    return i;
}

iftImage *View::getCurrentSagittalSlice()
{
    iftImage *i = nullptr;
    i = iftGetYZSlice(img,currentSagittalSliceNum);
    return i;
}

iftImage *View::getCurrentNormalizedAxialSlice()
{
    iftImage *i = nullptr;
    i = iftGetXYSlice(normalized,currentAxialSliceNum);
    return i;
}

iftImage *View::getCurrentNormalizedCoronalSlice()
{
    iftImage *i = nullptr;
    i = iftGetZXSlice(normalized,currentCoronalSliceNum);
    return i;
}

iftImage *View::getCurrentNormalizedSagittalSlice()
{
    iftImage *i = nullptr;
    i = iftGetYZSlice(normalized,currentSagittalSliceNum);
    return i;
}

iftImage *View::getCurrentLabelAxialSlice()
{
    if (!this->isLabelEmpty()){
        iftImage *i = nullptr;
        i = iftGetXYSlice(label,currentAxialSliceNum);
        return i;
    }
    return nullptr;
}

iftImage *View::getCurrentLabelCoronalSlice()
{
    if (!this->isLabelEmpty()){
        iftImage *i = nullptr;
        i = iftGetZXSlice(label,currentCoronalSliceNum);
        return i;
    }
    return nullptr;
}

iftImage *View::getCurrentLabelSagittalSlice()
{
    if (!this->isLabelEmpty()){
        iftImage *i = nullptr;
        i = iftGetYZSlice(label,currentSagittalSliceNum);
        return i;
    }
    return nullptr;
}

iftImage *View::getCurrentMarkerAxialSlice()
{
    const iftImage *m = annotation->getMarkers();
    iftImage *i = nullptr;
    i = iftGetXYSlice(m,currentAxialSliceNum);
    return i;
}

iftImage *View::getCurrentBorderAxialSlice()
{
    return iftGetXYSlice(border, currentAxialSliceNum);
}

void normalize(iftImage *img, int max_val) {
    #pragma omp parallel for
    for (int i = 0; i < img->n; i++) {
        if (img->val[i])
            img->val[i] = img->val[i] * 255.0 / max_val;
    }
}

iftImage *View::getCurrentGradientAxialSlice()
{
    if (gradient) {
        iftImage *basins = iftGetXYSlice(gradientCache, currentAxialSliceNum);
//        iftMImage *mi = iftMGetXYSlice(gradient, currentAxialSliceNum);
//        iftAdjRel *A = iftSpheric(gradientAdjRelRadius);
//        iftImage *basins = iftMImageBasins(mi, A);
//        normalize(basins, gradientNormValue);
//        iftDestroyMImage(&mi);
//        iftDestroyAdjRel(&A);
        return basins;
    }

    return nullptr;
}

iftImage *View::getCurrentMarkerCoronalSlice()
{
    const iftImage *m = annotation->getMarkers();
    iftImage *i = nullptr;
    i = iftGetZXSlice(m,currentCoronalSliceNum);
    //iftDestroyImage(&m);
    return i;
}

iftImage *View::getCurrentBorderCoronalSlice()
{
    return iftGetZXSlice(border, currentCoronalSliceNum);
}

iftImage *View::getCurrentGradientCoronalSlice()
{
    if (gradient) {
        iftImage *basins = iftGetZXSlice(gradientCache, currentCoronalSliceNum);
//        iftMImage *mi = iftMGetZXSlice(gradient, currentCoronalSliceNum);
//        iftAdjRel *A = iftSpheric(gradientAdjRelRadius);
//        iftImage *basins = iftMImageBasins(mi, A);
//        normalize(basins, gradientNormValue);
//        iftDestroyMImage(&mi);
//        iftDestroyAdjRel(&A);
        return basins;
    }

    return nullptr;
}

iftImage *View::getCurrentMarkerSagittalSlice()
{
    const iftImage *m = annotation->getMarkers();
    iftImage *i = nullptr;
    i = iftGetYZSlice(m,currentSagittalSliceNum);
    //iftDestroyImage(&m);
    return i;
}

iftImage *View::getCurrentRemovalMarkersAxialSlice()
{
    return iftGetXYSlice(annotation->getRemovalMarkers(), currentAxialSliceNum);
}

iftImage *View::getCurrentRemovalMarkersCoronalSlice()
{
    return iftGetZXSlice(annotation->getRemovalMarkers(), currentCoronalSliceNum);
}

iftImage *View::getCurrentRemovalMarkersSagittalSlice()
{
    return iftGetYZSlice(annotation->getRemovalMarkers(), currentSagittalSliceNum);
}

iftImage *View::getCurrentBorderSagittalSlice()
{
    return iftGetYZSlice(border, currentSagittalSliceNum);
}

iftImage *View::getCurrentGradientSagittalSlice()
{
    if (gradient) {
        iftImage *basins = iftGetYZSlice(gradientCache, currentSagittalSliceNum);
//        iftMImage *mi = iftMGetYZSlice(gradient, currentSagittalSliceNum);
//        iftAdjRel *A = iftSpheric(gradientAdjRelRadius);
//        iftImage *basins = iftMImageBasins(mi, A);
//        normalize(basins, gradientNormValue);
//        iftDestroyMImage(&mi);
//        iftDestroyAdjRel(&A);
        return basins;
    }

    return nullptr;
}


void View::setCurrentAxialSliceNum(int numSlice)
{
    currentAxialSliceNum = numSlice;
}

int View::getCurrentAxialSliceNum()
{
    return currentAxialSliceNum;
}

int View::getMaxAxialSlice()
{
    return maxAxialSlice;
}

void View::setCurrentCoronalSliceNum(int numSlice)
{
    currentCoronalSliceNum = numSlice;
}

int View::getCurrentCoronalSliceNum()
{
    return currentCoronalSliceNum;
}

int View::getMaxCoronalSlice()
{
    return maxCoronalSlice;
}

void View::setCurrentSagittalSliceNum(int numSlice)
{
    currentSagittalSliceNum = img->xsize-1-numSlice;
}

int View::getCurrentSagittalSliceNum()
{
    return currentSagittalSliceNum;
}

int View::getMaxSagittalSlice()
{
    return maxSagittalSlice;
}

void View::createRendition()
{
    if (img == nullptr)
        return;
    if (rendition != nullptr)
        delete rendition;
    rendition = new Rendering(img,label);
}

void View::destroyRendition()
{
    if (rendition != nullptr)
        delete rendition;
}

void View::setRenditionLabel()
{
    if (label == nullptr)
        return;
    if (!rendition->isGraphicalContextLabelEmpty())
        rendition->destroyLabel();

    rendition->setGraphicalContextLabel(label);
    for (int i = 1; i <= nlabels; i++)
    {
        rendition->setObjectColor(i,this->ctb->color[i-1]);
        rendition->setObjectOpacity(i,1.0);
        rendition->setObjectVisibility(i,true);
    }
}

void View::performRendering()
{
    iftDestroyImage(&projection);
    projection = rendition->iftRender(normalized,currentAxialSliceNum,currentCoronalSliceNum,currentSagittalSliceNum);
}

const iftImage *View::getProjection() const
{
    return this->projection;
}

void View::incrementRenderingAngle(double tilt, double spin)
{
    double currentTilt, currentSpin;
    rendition->getViewDirections(&currentTilt,&currentSpin);
    currentTilt += tilt;
    if ((currentTilt >= 360.0) || (currentTilt <= -360.0))
        currentTilt = 0.0;
    currentSpin += spin;
    if ((currentSpin >= 360.0) || (currentSpin <= -360.0))
        currentSpin = 0.0;
    rendition->setViewDirections(currentTilt,currentSpin);
}

void View::setRenderingMode(char mode)
{
    rendition->setMode(mode);
}

void View::getTiltSpinAngle(double *tilt, double *spin)
{
    rendition->getViewDirections(tilt,spin);
}

void View::setImage(iftImage *i, QString filename)
{
    this->filename = filename;
    iftDestroyMImage(&this->gradient);
    iftDestroyImage(&this->img);
    this->img = iftCopyImage(i);

    iftDestroyImage(&normalized);
    normalized = iftNormalize(i,0,255);

    currentAxialSliceNum = this->img->zsize/2;
    currentCoronalSliceNum = this->img->ysize/2;
    currentSagittalSliceNum = this->img->xsize/2;

    maxAxialSlice = this->img->zsize-1;
    maxCoronalSlice = this->img->ysize-1;
    maxSagittalSlice = this->img->xsize-1;

    maxValue = iftMaximumValue(img);
    minValue = iftMinimumValue(img);

    maxNormalizedValue = iftMaximumValue(normalized);
    minNormalizedValue = iftMinimumValue(normalized);
}

void View::setImage(iftImage *i)
{
    iftDestroyMImage(&this->gradient);
    iftDestroyImage(&this->img);
    this->img = iftCopyImage(i);

    iftDestroyImage(&normalized);
    normalized = iftNormalize(i,0,255);

    currentAxialSliceNum = this->img->zsize/2;
    currentCoronalSliceNum = this->img->ysize/2;
    currentSagittalSliceNum = this->img->xsize/2;

    maxAxialSlice = this->img->zsize-1;
    maxCoronalSlice = this->img->ysize-1;
    maxSagittalSlice = this->img->xsize-1;

    maxValue = iftMaximumValue(img);
    minValue = iftMinimumValue(img);

    maxNormalizedValue = iftMaximumValue(normalized);
    minNormalizedValue = iftMinimumValue(normalized);
}

const iftImage *View::getImage() const
{
    return this->img;
}

const iftImage *View::getNormalizedImage() const
{
    return this->normalized;
}

bool View::isImageEmpty()
{
    if (img == nullptr)
        return true;
    else
        return false;
}

void View::destroyImage()
{
    iftDestroyImage(&this->img);
    if (rendition != nullptr)
        delete rendition;
}

QString View::getFilename()
{
    return filename;
}

void View::setImageSequence(iftImageSequence *imgseq, QString filename)
{
    this->filename = filename;
    iftDestroyImageSequence(&this->imgseq);
    this->imgseq = imgseq;

    // Create thumbnails
    iftDestroyImageSequence(&thumbnails);
    int height = 100;
    int width  = 100.0 * imgseq->xsize / imgseq->ysize;
    thumbnails = iftCreateImageSequence(width, height, 1, imgseq->tsize, 1);
//    #pragma omp parallel for
    for (int t = 0; t < imgseq->tsize; t++) {
        iftImage *img = iftExtractVolumeFromImageSequence(imgseq, t);
        iftImage *slice = iftGetXYSlice(img, img->zsize/2);
        float sx = 1.0 * width / slice->xsize;
        float sy = 1.0 * height / slice->ysize;
        iftImage *scaled = iftInterp2D(slice, sx, sy);
        iftImage *norm = iftNormalize(scaled, 0, 255);
        iftSetVolumeInImageSequence(thumbnails, norm, t);
        iftDestroyImage(&norm);
        iftDestroyImage(&scaled);
        iftDestroyImage(&slice);
        iftDestroyImage(&img);
    }

    if (annotation != nullptr)
        delete annotation;
    annotation = new Marker(imgseq);

    displayImageSequenceTime(0);
}

const iftImageSequence *View::getImageSequence() const
{
    return imgseq;
}

void View::displayImageSequenceTime(int time)
{
    iftImage *img = iftExtractVolumeFromImageSequence(imgseq, time);
    setImage(img);
    iftDestroyImage(&img);
}

iftImage *View::getThumbnail(int time)
{
    return iftExtractVolumeFromImageSequence(thumbnails, time);
}

//TODO move def to global
iftColorTable *iftMarkerColorTableToLabelColorTable(iftColorTable *mctb, int nlabels) {
    iftColorTable *lctb = nullptr;

    if (mctb->ncolors > 0) {
        lctb = (iftColorTable*) iftAlloc(1, sizeof(iftColorTable));
        lctb->ncolors = nlabels;
        lctb->color = (iftColor*) iftAlloc(lctb->ncolors, sizeof(iftColor));

        for (int i = 0; i < lctb->ncolors; i++) {
            lctb->color[i].val[0] = mctb->color[i+1].val[0];
            lctb->color[i].val[1] = mctb->color[i+1].val[1];
            lctb->color[i].val[2] = mctb->color[i+1].val[2];
        }
    }

    return lctb;
}

void View::setLabel(iftImage *label, iftColorTable *labelCtb)
{
    iftDestroyImage(&this->label);
    this->label = iftCopyImage(label);
    nlabels = iftMaximumValue(this->label);
    if (nlabels <= 0) {
        destroyLabel();
    } else {
        if (labelCtb == nullptr) {
            if (this->ctb == nullptr)
               this->ctb = iftCreateColorTable(nlabels);
            else
               addItemInColorTable();
        } else {
            iftDestroyColorTable(&this->ctb);
            this->ctb = iftMarkerColorTableToLabelColorTable(labelCtb, nlabels);
        }
        iftDestroyImage(&border);
        border=iftBorderImage(this->label,0);
    }
}

const iftImage *View::getLabel() const
{
    return this->label;
}

const iftImage *View::getBorder() const
{
    return this->border;
}

bool View::isLabelEmpty()
{
    if (this->label == nullptr)
        return true;
    else
        return false;
}

void View::destroyLabel()
{
    if (this->label != nullptr){
        iftDestroyImage(&this->label);
        destroyColorTable();
        rendition->destroyLabel();
    }
}

int View::getNumberOfLabels()
{
    return nlabels;
}

int View::getXsize()
{
    return img->xsize;
}

int View::getYsize()
{
    return img->ysize;
}

int View::getZsize()
{
    return img->zsize;
}

iftImage *_iftNormalize(const iftImage *img, int minval, int maxval, float perc) {
    int img_min_val;
    int img_max_val;

    iftMinMaxValues(img, &img_min_val, &img_max_val);

    if (img_min_val >= img_max_val)
        return iftCopyImage(img);

    int nbins             = img_max_val + 1;
    iftHist *hist = iftCalcGrayImageHist(img, nullptr, nbins, img_max_val, true);
    iftHist *acchist = iftCalcAccHist(hist);
    iftDestroyHist(&hist);


    iftImage *nimg = nullptr;

    // find the threshold/brightness to cut the outliers
    for (int i = nbins - 1; acchist->val[i] > perc; i--)
        img_max_val = i;

    if (img_min_val < img_max_val) {

        nimg = iftCopyImage(img);
        if (iftIsColorImage(img))
            iftCopyCbCr(img, nimg);

        #pragma omp parallel for
        for (int p = 0; p < img->n; p++)
            if (img->val[p] <= img_max_val) {
                nimg->val[p] = (int) ((maxval - minval) * ((double) img->val[p] - (double) img_min_val) /
                                     ((double) img_max_val - (double) img_min_val) + minval);
            } else {
                nimg->val[p] = maxval;
            }
    } else
        nimg = iftNormalize(img, minval, maxval);

    iftDestroyHist(&acchist);

    return nimg;
}

void View::setGradient(iftMImage *gradient, float adjRelRadius)
{
    this->gradient = gradient;
    this->gradientAdjRelRadius = adjRelRadius;
    if (gradient) {
        iftAdjRel *A = iftSpheric(adjRelRadius);
        iftImage *basins = MImageGradient(gradient, A);
        iftDestroyImage(&this->gradientCache);
        this->gradientCache = _iftNormalize(basins, 0, 255, 0.99);
        iftDestroyImage(&basins);
        iftDestroyAdjRel(&A);
    }
}

const iftMImage *View::getGradient() const
{
    return gradient;
}

float View::getGradientAdjRelRadius()
{
    return gradientAdjRelRadius;
}

// TODO
iftImage *View::mergeLabels(iftImage *label1, iftImage *label2)
{
    // label2 has priority

    iftVerifyImageDomains(label1,label2,"mergeLabels");

    iftImage *out = iftCreateImageFromImage(label1);

    for (int i = 0; i < label1->n; i++){
        if (label1->val[i] != 0)
            out->val[i] = label1->val[i];
        if (label2->val[i] != 0)
            out->val[i] = label2->val[i];
    }

    return out;
}

int View::iftGetImageIntensityAtVoxel(int x, int y, int z)
{
    iftVoxel u;
    u.x = x;
    u.y = y;
    u.z = z;

    if (iftValidVoxel(img,u)){
        int p = iftGetVoxelIndex(img,u);
        return img->val[p];
    } else
        return -1;
}

int View::iftGetImageIntensityAtVoxel(iftVoxel u)
{
    if (iftValidVoxel(img,u)){
        int p = iftGetVoxelIndex(img,u);
        return img->val[p];
    } else
        return -1;
}

bool View::existColorTable()
{
    if (ctb == nullptr)
        return false;
    return true;
}

iftColorTable *View::getColorTable()
{
    if (ctb == nullptr)
        return nullptr;
    iftColorTable *t = iftCreateColorTable(ctb->ncolors);
    for (int i = 0; i < t->ncolors; i++){
        t->color[i].val[0] = ctb->color[i].val[0];
        t->color[i].val[1] = ctb->color[i].val[1];
        t->color[i].val[2] = ctb->color[i].val[2];
    }

    return t;
}

void View::setObjectColorInColorTable(int obj, iftColor color)
{
    this->ctb->color[obj].val[0] = color.val[0];
    this->ctb->color[obj].val[1] = color.val[1];
    this->ctb->color[obj].val[2] = color.val[2];
    this->rendition->setObjectColor(obj+1,color);
}

void View::addItemInColorTable()
{
    iftColor color,ycbcr;
    iftColorTable *t = nullptr;
    int h,S,p;

    h = iftRandomInteger(0,359);
    S = iftRandomInteger(0,255);
    color.val[0] = h;
    color.val[1] = S;
    color.val[2] = 255;
    color = iftHSVtoRGB(color, 255);
    ycbcr = iftRGBtoYCbCr(color, 255); // 8 bits

    p = 0;
    if (ctb == nullptr)
        t = iftCreateColorTable(1);
    else {
        t = iftCreateColorTable(ctb->ncolors+1);
        p = ctb->ncolors;
        for (int i = 0; i < ctb->ncolors; i++){
            t->color[i].val[0] = ctb->color[i].val[0];
            t->color[i].val[1] = ctb->color[i].val[1];
            t->color[i].val[2] = ctb->color[i].val[2];
        }
        iftDestroyColorTable(&ctb);
    }
    t->color[p].val[0] = ycbcr.val[0];
    t->color[p].val[2] = ycbcr.val[1];
    t->color[p].val[1] = ycbcr.val[2];

    ctb = t;
}

void View::destroyColorTable()
{
    iftDestroyColorTable(&ctb);
}

void View::setSegmentationMethod(Segmentation *method)
{
    this->currentSegMethod = method;
}

Segmentation *View::currentSegmentationMethod()
{
    return this->currentSegMethod;
}

ForestWrapper *View::getImageForest()
{
    return forest;
}

void View::setImageForest(iftImageForest *forest)
{
    if (this->forest)
        delete this->forest;
    this->forest = new ForestWrapper(forest);
}

void View::setImageForest(iftDynTrees *forest)
{
    if (this->forest)
        delete this->forest;
    this->forest = new ForestWrapper(forest);
}

