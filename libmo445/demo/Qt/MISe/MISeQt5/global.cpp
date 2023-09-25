#include "global.h"

//Transforms iftImage into QImage (in GrayScale)
QImage *iftImageToQImage(iftImage *img)
{
    QImage *dst = new QImage(img->xsize,img->ysize,QIMAGE_FORMAT);
    QColor color;

    #pragma omp parallel for private(color)
    for (int i=0;i<img->n;i++){
        iftColor rgb;

        if (iftIsColorImage(img)) {
            iftColor yCbBr = {{img->val[i], img->Cb[i], img->Cr[i]}, 1};
            rgb = iftYCbCrtoRGB(yCbBr, 255);
        } else {
            rgb.val[0] = img->val[i];
            rgb.val[1] = img->val[i];
            rgb.val[2] = img->val[i];
        }

        color.setRed(rgb.val[0]);
        color.setGreen(rgb.val[1]);
        color.setBlue(rgb.val[2]);

        iftVoxel u = iftGetVoxelCoord(img,i);
        dst->setPixel(u.x,u.y,color.rgb());
    }

    return dst;
}

QImage *iftImageToQImage(iftImage *img, iftImage *label, iftColorTable *t, bool *obj_visibility, iftImage *markers, iftColorTable *tm)
{
    QImage *dst = new QImage(img->xsize,img->ysize,QImage::Format_RGB888);
    QColor color;
    iftColor colorLabel, colorOnTable;

    #pragma omp parallel for private(color, colorLabel, colorOnTable)
    for (int i=0;i<img->n;i++){
        colorLabel.val[0] = img->val[i];
        colorLabel.val[1] = img->val[i];
        colorLabel.val[2] = img->val[i];
        //alpha = 255;
        if (label != NULL){
            if ((label->val[i] > 0) && (obj_visibility[label->val[i]])){
                colorOnTable.val[0] = img->val[i];
                colorOnTable.val[1] = t->color[label->val[i]-1].val[1];
                colorOnTable.val[2] = t->color[label->val[i]-1].val[2];                
                colorLabel = iftYCbCrtoRGB(colorOnTable,255);
            }
        }
        if (markers != NULL)
            if (markers->val[i] > 0){
                colorOnTable.val[0] = tm->color[markers->val[i]-1].val[0];
                colorOnTable.val[1] = tm->color[markers->val[i]-1].val[1];
                colorOnTable.val[2] = tm->color[markers->val[i]-1].val[2];
                colorLabel = iftYCbCrtoRGB(colorOnTable,255);
            }

        color.setRed(colorLabel.val[0]);
        color.setGreen(colorLabel.val[1]);
        color.setBlue(colorLabel.val[2]);
        color.setAlpha(img->val[i]);

        iftVoxel u = iftGetVoxelCoord(img,i);
        dst->setPixel(u.x,u.y,color.rgb());
    }

    return dst;
}

QImage *iftImageToQImage(iftImage *img, iftImage *label, iftImage *border, iftColorTable *t, bool *obj_visibility, iftImage *markers, iftImage *removal_markers, iftColorTable *tm, iftIntArray *markers_visibility)
{
    QImage *dst = new QImage(img->xsize,img->ysize,QImage::Format_RGB888);
    QColor color;
    iftColor colorLabel, colorOnTable;

    #pragma omp parallel for private(color, colorLabel, colorOnTable)
    for (int i=0;i<img->n;i++){
        if (iftIsColorImage(img)) {
            iftColor yCbBr = {{img->val[i], img->Cb[i], img->Cr[i]}, 1};
            colorLabel = iftYCbCrtoRGB(yCbBr, 255);
        } else {
            colorLabel.val[0] = img->val[i];
            colorLabel.val[1] = img->val[i];
            colorLabel.val[2] = img->val[i];
        }
        //alpha = 255;
        if (label != nullptr){
            if ((border->val[i] > 0) && (obj_visibility[label->val[i]])){
                colorOnTable.val[0] = t->color[border->val[i]-1].val[0];
                colorOnTable.val[1] = t->color[border->val[i]-1].val[1];
                colorOnTable.val[2] = t->color[border->val[i]-1].val[2];
                colorLabel = iftYCbCrtoRGB(colorOnTable,255);
            }else if ((label->val[i] > 0) && (obj_visibility[label->val[i]])){

//                if (toggle_markers->val[i] != 0) {
//                    colorOnTable.val[0] = 255 - img->val[i];
//                    colorOnTable.val[1] = 255 - t->color[label->val[i]-1].val[1];
//                    colorOnTable.val[2] = 255 - t->color[label->val[i]-1].val[2];
//                    colorLabel = iftYCbCrtoRGB(colorOnTable,255);
//                } else {
                    colorOnTable.val[0] = img->val[i];
                    colorOnTable.val[1] = t->color[label->val[i]-1].val[1];
                    colorOnTable.val[2] = t->color[label->val[i]-1].val[2];
                    colorLabel = iftYCbCrtoRGB(colorOnTable,255);
//                }
            }
        }
        if (markers != nullptr) {
            if ((markers->val[i] >= 0) && (markers_visibility->val[markers->val[i]])){

//                if (toggle_markers->val[i] != 0) {
//                    colorOnTable.val[0] = 255 - tm->color[markers->val[i]].val[0];
//                    colorOnTable.val[1] = 255 - tm->color[markers->val[i]].val[1];
//                    colorOnTable.val[2] = 255 - tm->color[markers->val[i]].val[2];
//                    colorLabel = iftYCbCrtoRGB(colorOnTable,255);
//                } else {
                    colorOnTable.val[0] = tm->color[markers->val[i]].val[0];
                    colorOnTable.val[1] = tm->color[markers->val[i]].val[1];
                    colorOnTable.val[2] = tm->color[markers->val[i]].val[2];
                    colorLabel = iftYCbCrtoRGB(colorOnTable,255);
//                }
            }
        }

        if (removal_markers != nullptr && removal_markers->val[i] != 0) {
            color.setRed(255 - colorLabel.val[0]);
            color.setGreen(255 - colorLabel.val[1]);
            color.setBlue(255 - colorLabel.val[2]);
        } else {
            color.setRed(colorLabel.val[0]);
            color.setGreen(colorLabel.val[1]);
            color.setBlue(colorLabel.val[2]);
        }
        color.setAlpha(img->val[i]);

        iftVoxel u = iftGetVoxelCoord(img,i);
        dst->setPixel(u.x,u.y,color.rgb());
    }

    return dst;
}

QImage *iftColoredImageToColoredQImage(iftImage *img)
{
    if ((img->Cb == nullptr) || (img->Cr == nullptr))
        return nullptr;

    QImage *dst = new QImage(img->xsize,img->ysize,QIMAGE_FORMAT);
    iftColor RGB, YCbCr;
    QColor color;

    for (int i=0;i<img->n;i++){
        YCbCr.val[0] = img->val[i];
        YCbCr.val[1] = img->Cb[i];
        YCbCr.val[2] = img->Cr[i];
        RGB = iftYCbCrtoRGB(YCbCr,255);
        color.setRed(RGB.val[0]);
        color.setGreen(RGB.val[1]);
        color.setBlue(RGB.val[2]);
        iftVoxel u = iftGetVoxelCoord(img,i);
        dst->setPixel(u.x,u.y,color.rgb());
    }

    return dst;
}

//Transforms iftImage and its label into a colored QImage
QImage *iftLabeledImageToColoredQImage(iftImage *img, iftImage *label, iftColorTable *t, bool *obj_visibility)
{

    QImage *dst = new QImage(img->xsize,img->ysize,QIMAGE_FORMAT);
    QColor color;
    iftColor colorLabel, colorOnTable;

    #pragma omp parallel for private(color, colorLabel, colorOnTable)
    for (int i=0;i<img->n;i++){
        if ((label->val[i] > 0) && (obj_visibility[label->val[i]])){
            colorOnTable.val[0] = img->val[i];
            colorOnTable.val[1] = t->color[label->val[i]-1].val[1];
            colorOnTable.val[2] = t->color[label->val[i]-1].val[2];
            colorLabel = iftYCbCrtoRGB(colorOnTable,255);
            color.setRed(colorLabel.val[0]);
            color.setGreen(colorLabel.val[1]);
            color.setBlue(colorLabel.val[2]);
        } else{
            color.setRed(img->val[i]);
            color.setGreen(img->val[i]);
            color.setBlue(img->val[i]);
        }
        iftVoxel u = iftGetVoxelCoord(img,i);
        dst->setPixel(u.x,u.y,color.rgb());
    }

    return dst;
}

QImage *iftLabeledImageToColoredQImage(iftImage *img, iftImage *label, iftColorTable *t)
{
    QImage *dst = new QImage(img->xsize,img->ysize,QIMAGE_FORMAT);
    QColor color;
    iftColor colorLabel, colorOnTable;

    #pragma omp parallel for private(color, colorLabel, colorOnTable)
    for (int i=0;i<img->n;i++){
        if ((label->val[i] > 0) && (t != nullptr)){
            colorOnTable.val[0] = img->val[i];
            colorOnTable.val[1] = t->color[label->val[i]-1].val[1];
            colorOnTable.val[2] = t->color[label->val[i]-1].val[2];
            colorLabel = iftYCbCrtoRGB(colorOnTable,255);
            colorLabel = iftYCbCrtoRGB(t->color[label->val[i]-1],255);
            color.setRed(colorLabel.val[0]);
            color.setGreen(colorLabel.val[1]);
            color.setBlue(colorLabel.val[2]);
        } else{
            color.setRed(img->val[i]);
            color.setGreen(img->val[i]);
            color.setBlue(img->val[i]);
        }
        iftVoxel u = iftGetVoxelCoord(img,i);
        dst->setPixel(u.x,u.y,color.rgb());
    }

    return dst;
}

//Transforms QImage into iftImage
iftImage *iftQImageToImage(QImage *img)
{
    iftImage *dst=nullptr;
    uchar *u;

    dst=iftCreateImage(img->height(),img->width(),1);

    for (int i=0; i<img->height();i++){
        u = img->scanLine(i);
        for (int j=0;j<img->width();j++,u++){
            if (img->valid(i,j))
            {
                iftVoxel v;
                v = iftGetVoxelCoord(dst,i*img->width()+j);
                if (iftValidVoxel(dst,v)){
                    int q = iftGetVoxelIndex(dst,v);
                    dst->val[q] = *u;
                }
            }
        }
    }

    return dst;
}

QColor iftColorToQColor(iftColor YCbCr)
{
    QColor color;
    iftColor colorLabel;
    colorLabel = iftYCbCrtoRGB(YCbCr,255);
    color.setRed(colorLabel.val[0]);
    color.setGreen(colorLabel.val[1]);
    color.setBlue(colorLabel.val[2]);
    return color;
}

//Return min or max if value outrange them
int minmax(int value, int min, int max)
{
   if (value < min)
      return min;
   if (value > max)
      return max;
   return value;
}

void iftWriteOnLog(QString operation_detail, qint64 elapsed)
{
    QString msg;
    if (elapsed == 0)
        msg = operation_detail;
    else
        msg = operation_detail + " --- " + QString::number(elapsed) + " milliseconds.";
    QFile file(QDir::homePath()+"/mivlog.txt");
    file.open(QIODevice::Append);
    QTextStream mylog(&file);
    mylog << msg << Qt::endl;
    file.close();
}

void iftInitMarkersImage(iftImage *markers)
{
    #pragma omp parallel for
    for (int i = 0; i < markers->n; i++) {
        markers->val[i] = UNMARKED;
    }
}

int iftGetNormalIndex(iftVector normal){
    int gamma,alpha,index;

    if ((normal.x == 0.0) && (normal.y == 0.0) && (normal.z == 0.0)) {
        return(0);
    }

    gamma = (int)(asin(normal.z) * 180.0 / IFT_PI); /* [-90,90] */
    alpha = (int)(atan2(normal.y,normal.x) * 180.0 / IFT_PI); /* [-180,180] */
    if (alpha < 0)
        alpha += 360;
    index = ((gamma+90)*360) + alpha + 1;

    return(index);
}

void iftSetObjectNormalParallelized(iftGraphicalContext *gc, char normal_type){
    iftAdjRel *A = NULL;
    iftImage  *dist = NULL;
    iftSet    *S=NULL;

    if (gc->label == NULL)
        iftError("Object labels are required", "iftSetObjectNormal");

    if (gc->normal != NULL)
        iftDestroyImage(&gc->normal);

    if (gc->opacity != NULL)
        iftDestroyFImage(&gc->opacity);

    A             = iftSpheric(1.0);
    S             = iftObjectBorderSet(gc->label,A);
    iftDestroyAdjRel(&A);

    int *pixels = new int[gc->label->n];

    int n = 0;
    while (S != NULL) {
        pixels[n] = iftRemoveSet(&S);
        n++;
    }

    /* estimate object-based normal vectors and set opacity scene for the shell */

    gc->normal  = iftCreateImage(gc->label->xsize,gc->label->ysize,gc->label->zsize);
    gc->opacity = iftCreateFImage(gc->label->xsize,gc->label->ysize,gc->label->zsize);

    if (normal_type == OBJECT_NORMAL){
        /* extract shell inside the object and estimate normals for the
         border voxels */

        A             = iftSpheric(sqrtf(3.0));
        dist          = iftShellDistTrans(gc->label,A,IFT_INTERIOR,5);
        iftDestroyAdjRel(&A);

        // if the following value is lower, the rendering result is worse but quicker
        A   = iftSpheric(5.0);

        #pragma omp parallel for shared(gc)
        for (int i = 0; i < n; i++) {

            int p = pixels[i];

            gc->opacity->val[p] = 1.0;

            iftVoxel u = iftGetVoxelCoord(dist,p);
            iftVector N;
            N.x = N.y = N.z = 0.0;

            for (int i=1; i < A->n; i++) {
                iftVoxel v = iftGetAdjacentVoxel(A,u,i);
                if (iftValidVoxel(dist,v)){
                    int q = iftGetVoxelIndex(dist,v);
                    if ((gc->label->val[p]==gc->label->val[q])&&
                        (dist->val[q] != IFT_INFINITY_INT)){
                        N.x  += dist->val[q]*A->dx[i];
                        N.y  += dist->val[q]*A->dy[i];
                        N.z  += dist->val[q]*A->dz[i];
                    }
                }
            }

            /* force normal to point outward the object */

            N   = iftNormalizeVector(N);
            N.x = -N.x; N.y = -N.y; N.z = -N.z;
            gc->normal->val[p] = iftGetNormalIndex(N);
        }

    } else { /* SCENE_NORMAL */
        A   = iftSpheric(5.0);

        #pragma omp parallel for
        for (int i = 0; i < n; i++) {

            int p = pixels[i];

            gc->opacity->val[p] = 1.0;

            iftVoxel u = iftFGetVoxelCoord(gc->scene,p);
            iftVector  N;
            N.x = N.y = N.z = 0.0;
            for (int i=1; i < A->n; i++) {
                iftVoxel v = iftGetAdjacentVoxel(A,u,i);
                if (iftFValidVoxel(gc->scene,v)){
                    int q = iftFGetVoxelIndex(gc->scene,v);
                    if (gc->label->val[q]==gc->label->val[p]){
                        N.x  += gc->scene->val[q]*A->dx[i];
                        N.y  += gc->scene->val[q]*A->dy[i];
                        N.z  += gc->scene->val[q]*A->dz[i];
                    }
                }
            }

            /* it assumes scenes with objects brighter than the
           background, THEN forces normal to point outward the
           object */

            N = iftNormalizeVector(N);
            N.x = -N.x; N.y = -N.y; N.z = -N.z;

            gc->normal->val[p] = iftGetNormalIndex(N);
        }
    }

    delete[] pixels;
    iftDestroyAdjRel(&A);
    iftDestroyImage(&dist);
}

bool iftAreImagesEqual(const iftImage *img1, const iftImage *img2)
{
    if (img1 == nullptr || img2 == nullptr)
        return false;

    if (img1->n != img2->n) {
        return false;
    }

    bool are_equal = true;
    #pragma omp parallel for
    for (int i = 0; i < img1->n; i++) {
        if (img1->val[i] != img2->val[i])
            are_equal = false;
    }

    return are_equal;
}

int _BucketFIFOPQueue(iftGQueue *Q, int bucket){
  int elem=IFT_NIL, next;

  elem = Q->C.first[bucket];
  next = Q->L.elem[elem].next;
  if(next == IFT_NIL) { /* there was a single element in the list */
    Q->C.first[bucket] = Q->C.last[bucket] = IFT_NIL;
  }
  else {
    Q->C.first[bucket] = next;
    Q->L.elem[next].prev = IFT_NIL;
  }
  Q->L.elem[elem].color = IFT_BLACK;
  return elem;
}

int _FastFindMinBucketPQueue(iftGQueue *Q){
  int current;

  current = Q->C.minvalue;
  /** moves to next element **/
  if(Q->C.first[current] == IFT_NIL){
    do{
      current++;
    }while((current<Q->C.nbuckets)&&(Q->C.first[current] == IFT_NIL));

    if(current < Q->C.nbuckets)
      Q->C.minvalue = current;
    else
      iftError("PQueue is empty","_FastFindMinBucketPQueue");
  }
  return current;
}

int iftFastRemoveGQueue(iftGQueue *Q, int *nadded) {
    int bucket;
    (*nadded)--;
    bucket = _FastFindMinBucketPQueue(Q);
    return _BucketFIFOPQueue(Q, bucket);
}

/* Faster version to be used with watershed when
   values are in fixed range [0, nbuckets-1]   */
void iftFastInsertGQueue(iftGQueue *Q, int elem, int *nadded){
  int bucket;

  (*nadded)++;
  bucket = Q->L.value[elem];

  if(bucket < Q->C.minvalue)
    Q->C.minvalue = bucket;
  if(bucket > Q->C.maxvalue)
    Q->C.maxvalue = bucket;

  if(Q->C.first[bucket] == IFT_NIL){
    Q->C.first[bucket]   = elem;
    Q->L.elem[elem].prev = IFT_NIL;
  }else {
    Q->L.elem[Q->C.last[bucket]].next = elem;
    Q->L.elem[elem].prev = Q->C.last[bucket];
  }
  Q->C.last[bucket]     = elem;
  Q->L.elem[elem].next  = IFT_NIL;
  Q->L.elem[elem].color = IFT_GRAY;
}


void iftFastRemoveGQueueElem(iftGQueue *Q, int elem, int *nadded){
  int prev,next,bucket;

  (*nadded)--;
  bucket = Q->L.value[elem];
  prev = Q->L.elem[elem].prev;
  next = Q->L.elem[elem].next;

  /* if elem is the first element */
  if (Q->C.first[bucket] == elem) {
    Q->C.first[bucket] = next;
    if (next == IFT_NIL) /* elem is also the last one */
      Q->C.last[bucket] = IFT_NIL;
    else
      Q->L.elem[next].prev = IFT_NIL;
  }
  else{   /* elem is in the middle or it is the last */
    Q->L.elem[prev].next = next;
    if (next == IFT_NIL) /* if it is the last */
      Q->C.last[bucket] = prev;
    else
      Q->L.elem[next].prev = prev;
  }
  Q->L.elem[elem].color = IFT_BLACK;
}

void iftFastDiffWatershed(iftImageForest *fst, iftLabeledSet *seed, iftSet *removal_markers)
{
  iftAdjRel *A = fst->A;
  iftGQueue *Q = fst->Q;
  iftVoxel   u, v;
  int        i, p, q, tmp;
  iftSet    *Frontier = NULL;
  iftLabeledSet *S;
  //iftBMap   *processed = fst->processed;
  iftImage  *pathval = fst->pathval, *pred = fst->pred, *label = fst->label;
  iftImage  *root = fst->root, *basins = fst->img, *marker = fst->marker;

  int nadded = 0;

  //iftFillBMap(processed, 0);

  if (removal_markers != NULL)
  {
    Frontier = iftTreeRemoval(fst, removal_markers);
    while (Frontier != NULL)
    {
      p = iftRemoveSet(&Frontier);
      iftFastInsertGQueue(Q, p, &nadded);
    }
  }

  S = seed;
  while (S != NULL)
  {
    p = S->elem;

    if (Q->L.elem[p].color == IFT_GRAY)
    {
      /* p is also a frontier voxel, but the priority is it as a seed. */
      iftFastRemoveGQueueElem(Q, p, &nadded);
    }

    label->val[p]   = S->label;
    pathval->val[p] = 0;
    root->val[p]    = p;
    pred->val[p]    = IFT_NIL;
    marker->val[p]  = S->marker;
    iftFastInsertGQueue(Q, p, &nadded);
    S = S->next;
  }

  /* Image Foresting Transform */
  while (nadded > 0)
  {
    p = iftFastRemoveGQueue(Q, &nadded);
    u = iftGetVoxelCoord(basins, p);
    //iftBMapSet1(processed, p);

    for (i = 1; i < A->n; i++)
    {
      v = iftGetAdjacentVoxel(A, u, i);
      if (iftValidVoxel(basins, v))
      {
        q = iftGetVoxelIndex(basins, v);

        if (Q->L.elem[q].color != IFT_BLACK) {

            tmp = iftMax(pathval->val[p], basins->val[q]);

            /* if pred[q]=p then p and q belong to a tie-zone */
            if ((tmp < pathval->val[q]) || ((pred->val[q] == p)))
            {
              if (Q->L.elem[q].color == IFT_GRAY)
              {
                iftFastRemoveGQueueElem(Q, q, &nadded);
              }
              pred->val[q]     = p;
              root->val[q]     = root->val[p];
              label->val[q]    = label->val[p];
              marker->val[q]   = marker->val[p];
              pathval->val[q]  = tmp;
              iftFastInsertGQueue(Q, q, &nadded);
            }
        }
      }
    }
  }
  iftResetGQueue(Q);
}

iftMImage *iftMGetZXSlice(const iftMImage *mimg, int ycoord)
{
    iftMImage *slice;
    iftVoxel  u;
    int       p,q;

    if ( (ycoord < 0) || (ycoord >= mimg->ysize))
        iftError("Invalid y coordinate", "iftMGetZXSlice");

    slice = iftCreateMImage(mimg->zsize,mimg->xsize, 1, mimg->m);

    u.y   = ycoord;
    q = 0;
    for (u.x = 0; u.x < mimg->xsize; u.x++)
        for (u.z = 0; u.z < mimg->zsize; u.z++)
        {
            p = iftMGetVoxelIndex(mimg,u);
            for (ulong b = 0; b < mimg->m; b++) {
                slice->val[q][b] = mimg->val[p][b];
            }
            q++;
        }
    iftMCopyVoxelSize(mimg,slice);

    return(slice);
}

iftMImage *iftMGetXYSlice(const iftMImage *mimg, int zcoord)
{
    iftMImage *slice;
    iftVoxel  u;
    int       p,q;

    if ( (zcoord < 0) || (zcoord >= mimg->zsize))
        iftError("Invalid z coordinate", "iftMGetXYSlice");

    slice = iftCreateMImage(mimg->xsize,mimg->ysize, 1, mimg->m);


    u.z   = zcoord;
    q     = 0;
    for (u.y = 0; u.y < mimg->ysize; u.y++)
        for (u.x = 0; u.x < mimg->xsize; u.x++)
        {
            p = iftMGetVoxelIndex(mimg,u);
            for (ulong b = 0; b < mimg->m; b++) {
                slice->val[q][b] = mimg->val[p][b];
            }
            q++;
        }
    iftMCopyVoxelSize(mimg,slice);

    return(slice);
}

iftMImage *iftMGetYZSlice(const iftMImage *mimg, int xcoord)
{
    iftMImage *slice;
    iftVoxel  u;
    int       p,q;

    if ( (xcoord < 0) || (xcoord >= mimg->xsize))
        iftError("Invalid x coordinate", "iftMGetYZSlice");

    slice = iftCreateMImage(mimg->ysize,mimg->zsize, 1, mimg->m);


    u.x   = xcoord;
    q     = 0;
    for (u.z = 0; u.z < mimg->zsize; u.z++)
        for (u.y = 0; u.y < mimg->ysize; u.y++)
        {
            p = iftMGetVoxelIndex(mimg,u);
            for (ulong b = 0; b < mimg->m; b++) {
                slice->val[q][b] = mimg->val[p][b];
            }
            q++;
        }
    iftMCopyVoxelSize(mimg,slice);

    return(slice);
}

iftImage *MImageGradient(const iftMImage *img, iftAdjRel *A)
{
    int Imax = 255;
    iftFImage *gradI = iftCreateFImage(img->xsize,img->ysize,img->zsize);
    float Gmin=IFT_INFINITY_FLT, Gmax=IFT_INFINITY_FLT_NEG;

    for (ulong p=0; p < img->n; p++){
        iftVoxel u  = iftMGetVoxelCoord(img,p);
        float value = 0.0;
        for (int i=1; i < A->n; i++) {
            iftVoxel v = iftGetAdjacentVoxel(A,u,i);
            if (iftMValidVoxel(img,v)){
            int q = iftMGetVoxelIndex(img,v);
                for (ulong b=0; b < img->m; b++)
                    value += (img->val[q][b]-img->val[p][b])*(img->val[q][b]-img->val[p][b]);
            }
        }
        value = sqrtf(value)/A->n;
        if (value > Gmax) Gmax = value;
        if (value < Gmin) Gmin = value;
        gradI->val[p] = value;
    }

    for (ulong p=0; p < img->n; p++){
        gradI->val[p] = (Imax*(gradI->val[p]-Gmin)/(Gmax-Gmin));
    }

    iftImage *out = iftCreateImage(gradI->xsize,gradI->ysize,gradI->zsize);
    for (ulong p=0; p < img->n; p++){
        out->val[p] = iftRound(gradI->val[p]);
    }
    iftDestroyFImage(&gradI);

    return(out);
}

iftDynTrees *iftCreateDynTrees(iftMImage *img, iftAdjRel *A)
{
  iftDynTrees *dt = (iftDynTrees *)calloc(1,sizeof(iftDynTrees));

  dt->img      = img; dt->A = A;
  dt->label    = iftCreateImage(img->xsize,img->ysize,img->zsize);
  dt->root     = iftCreateImage(img->xsize,img->ysize,img->zsize);
  dt->cost     = iftCreateImage(img->xsize,img->ysize,img->zsize);
  dt->pred     = iftCreateImage(img->xsize,img->ysize,img->zsize);
  dt->cumfeat  = iftCreateMImage(img->xsize,img->ysize,img->zsize,img->m);
  dt->treesize = iftAllocIntArray(img->n);
  dt->Q        = iftCreateGQueue(CMAX+1, img->n, dt->cost->val);

  /* compute the maximum feature distance */

  dt->maxfeatdist = 0.0;
  for (ulong p=0; p < img->n; p++){
    iftVoxel u = iftMGetVoxelCoord(img,p);
    for (int i=1; i < A->n; i++) {
      iftVoxel v = iftGetAdjacentVoxel(A,u,i);
      if (iftMValidVoxel(img,v)){
    int q = iftMGetVoxelIndex(img,v);
    float dist = 0.0;
    for (ulong b=0; b < img->m; b++){
      dist += (img->val[q][b]-img->val[p][b])*
        (img->val[q][b]-img->val[p][b]);
    }
    if (dist > dt->maxfeatdist)
      dt->maxfeatdist = dist;
      }
    }
  }

  /* initialize maps */

  for (ulong p=0; p < img->n; p++){
    dt->cost->val[p] = IFT_INFINITY_INT;
    dt->pred->val[p] = IFT_NIL;
  }

  return(dt);
}

void iftDestroyDynTrees(iftDynTrees **dt)
{
  iftDynTrees *aux = *dt;

  if (aux != NULL) {
    iftDestroyGQueue(&aux->Q);
    iftDestroyImage(&aux->cost);
    iftDestroyImage(&aux->pred);
    iftDestroyImage(&aux->label);
    iftDestroyImage(&aux->root);
    iftDestroyMImage(&aux->cumfeat);
    iftFree(aux->treesize);
    iftFree(aux);
    *dt = NULL;
  }
}

void iftExecDiffDynTrees(iftDynTrees *dt, iftLabeledSet **S, iftSet **M)
{
  iftSet *F = iftDiffDynTreeRemoval(dt, M); /* remove trees and return their
                       frontier set, which is empty
                       when the marking set M is
                       empty */

  iftMImage *img      = dt->img;
  iftAdjRel *A        = dt->A;
  iftImage  *cost     = dt->cost;
  iftImage  *label    = dt->label;
  iftImage  *pred     = dt->pred;
  iftImage  *root     = dt->root;
  iftGQueue *Q        = dt->Q;
  iftMImage *cumfeat  = dt->cumfeat;
  int       *treesize = dt->treesize;

  /* Remove seeds from F, if it is the case since seeds have higher
     priority */

  /* if (F != NULL) { /\* non-empty set *\/ */
  /*   iftLabeledSet *seed = *S; */
  /*   while (seed != NULL){ */
  /*     int p = seed->elem; */
  /*     iftRemoveSetElem(&F,p); */
  /*     seed  = seed->next; */
  /*   } */
  /* } */

  /* Reinitialize maps for seed voxels and insert seeds and frontier
     in the queue */

  while (*S != NULL) {
    int lambda;
    int p         = iftRemoveLabeledSet(S,&lambda);
    cost->val[p]  = 0;
    label->val[p] = lambda;
    pred->val[p]  = IFT_NIL;
    root->val[p]  = p;
    for (ulong b=0; b < img->m; b++)
      cumfeat->val[p][b] = 0;
    treesize[p] = 0;
    iftUnionSetElem(&F,p);
  }

  while (F != NULL){
    int p = iftRemoveSet(&F);
    iftInsertGQueue(&Q,p);
  }

  /* Execute the Image Foresting Transform of DDT */

  while (!iftEmptyGQueue(Q)){
    int p      = iftRemoveGQueue(Q);
    iftVoxel u = iftMGetVoxelCoord(img,p);

    /* set / update dynamic tree of p */

    int r = root->val[p];
    /* if (pred->val[p] == IFT_NIL) { /\* p is a root voxel *\/ */
    /*   for (int b=0; b < img->m; b++) */
    /* 	cumfeat->val[r][b] = img->val[r][b]; */
    /*   treesize[r] = 1; */
    /* } else { */
    for (ulong b=0; b < img->m; b++)
      cumfeat->val[r][b] += img->val[p][b];
    treesize[r] += 1;
    /* } */

    /* visit the adjacent voxels for possible path extension */

    for (int i=1; i < A->n; i++) {
      iftVoxel v = iftGetAdjacentVoxel(A,u,i);

      if (iftMValidVoxel(img,v)){
    int q   = iftMGetVoxelIndex(img,v);
    if (Q->L.elem[q].color != IFT_BLACK){
      float arcw = 0;
      for (ulong b=0; b < img->m; b++){
        arcw += (img->val[q][b] - cumfeat->val[r][b]/treesize[r])*
          (img->val[q][b] - cumfeat->val[r][b]/treesize[r]);
      }
      arcw = CMAX * (arcw / dt->maxfeatdist);
      int tmp = iftMax(cost->val[p], iftMin(arcw,CMAX));
      if (tmp < cost->val[q])  {
        if (Q->L.elem[q].color == IFT_GRAY)
          iftRemoveGQueueElem(Q, q);
        cost->val[q]  = tmp;
        label->val[q] = label->val[p];
        root->val[q]  = root->val[p];
        pred->val[q]  = p;
        iftInsertGQueue(&Q,q);
      } else {
        if ( (label->val[q] != label->val[p]) &&
         (pred->val[q] == p) ) {
          iftDiffDynSubtreeUpdate(dt,q);
        }
      }
    }
      }
    }
  }

  iftResetGQueue(Q);
}

iftSet *iftDiffDynTreeRemoval(iftDynTrees *dt, iftSet **M)
{
  iftSet /**F = NULL, *R = NULL,*/ *F = NULL, *T = NULL, *Q = NULL;
  iftAdjRel *B = NULL;
  iftBMap *_F = iftCreateBMap(dt->img->n);
  iftBMap *R = iftCreateBMap(dt->img->n);

  if (iftIs3DMImage(dt->img))
    B = iftSpheric(1.74);
  else
    B = iftCircular(1.42);

  /* Find the roots of the trees that contain elements in the marking
     set. If the root has not been inserted yet in a root set R, do it
     for its entire marker and reset their cost and predecessor
     information. Set R must contain the roots of all trees marked for
     removal. Set T starts being equal to R and next it stores
     elements from the trees rooted in R. */

  while(*M != NULL) {
    int p = iftRemoveSet(M);
    int r = dt->root->val[p];
    /* insert in R and T all roots in the marker of r for tree
       removal */
    iftInsertSet(&Q,r);
    while (Q != NULL) {
      int r = iftRemoveSet(&Q);
      if (dt->cost->val[r] != IFT_INFINITY_INT){ /* r is not in R and T yet */
    dt->cost->val[r]=IFT_INFINITY_INT;
    //iftInsertSet(&R,r); iftInsertSet(&T,r);
    iftBMapSet1(R, r); iftInsertSet(&T,r);
      }
      iftVoxel u = iftMGetVoxelCoord(dt->img,r);
      for (int i=1; i < B->n; i++) {
        iftVoxel v = iftGetAdjacentVoxel(B,u,i);
        if (iftMValidVoxel(dt->img,v)){
          int s = iftMGetVoxelIndex(dt->img, v);
      /* s belongs to the same marker of r and it has not been
         inserted in R and T yet. */
          if ((dt->root->val[s]==s)&&
          (dt->cost->val[s] != IFT_INFINITY_INT))
            iftInsertSet(&Q,s);
        }
      }
    }
  }

  /* Visit all nodes in each tree with root in R, while removing the
     tree. It also identifies the frontier voxels of trees that have
     not been marked for removal. */

  while (T != NULL) {
    int      p = iftRemoveSet(&T);
    iftVoxel u = iftMGetVoxelCoord(dt->img,p);
    for (int i=1; i < dt->A->n; i++) {
      iftVoxel v = iftGetAdjacentVoxel(dt->A,u,i);
      if (iftMValidVoxel(dt->img,v)){
    int q = iftMGetVoxelIndex(dt->img, v);
    /* If q belongs to a tree for removal, then reset its cost and
       predecessor information, inserting it in T to pursue tree
       removal. */
    if (dt->pred->val[q]==p){
      dt->cost->val[q]=IFT_INFINITY_INT;
      dt->pred->val[q]=IFT_NIL;
      iftInsertSet(&T,q);
    } else {
      /* If q belongs to a tree that was not marked for removal,
         then q is a frontier voxel. */
      //if (!iftSetHasElement(R, dt->root->val[q])){
      if (!iftBMapValue(R, dt->root->val[q])){
          if (!iftBMapValue(_F, q)) {
            iftBMapSet1(_F,q);
            iftInsertSet(&F, q);
          }
      }
    }
      }
    }
  }

  iftDestroyBMap(&_F);
  iftDestroyBMap(&R);
  iftDestroyAdjRel(&B);

  return(F);
}

void iftDiffDynSubtreeUpdate(iftDynTrees *dt, int q)
{
  iftSet *T = NULL; /* Set to visit the nodes of the subtree rooted in
               q */
  /* If the subtree contains a single node, its root q, then update
     its cost, label and root information. */
  /* if (dt->Q->L.elem[q].color == IFT_GRAY){  */
  /*   iftRemoveGQueueElem(dt->Q, q); */
  /*   int p      = dt->pred->val[q]; */
  /*   int r      = dt->root->val[p]; */
  /*   float arcw = 0; */
  /*   for (int b=0; b < dt->img->m; b++){ */
  /*     arcw += (dt->img->val[q][b] - dt->cumfeat->val[r][b]/dt->treesize[r])* */
  /* 	      (dt->img->val[q][b] - dt->cumfeat->val[r][b]/dt->treesize[r]); */
  /*   } */
  /*   arcw              = CMAX * (arcw / dt->maxfeatdist);  */
  /*   dt->cost->val[q]  = iftMax(dt->cost->val[p], iftMin(arcw,CMAX)); */
  /*   dt->label->val[q] = dt->label->val[p]; */
  /*   dt->root->val[q]  = dt->root->val[p]; */
  /* } else { */
  /* the subtree contains one or more elements from a previous execution */
  iftInsertSet(&T,q);
  while (T != NULL) {
    int q = iftRemoveSet(&T);

    /* update properties of the previous tree of q */
    /* int r = dt->root->val[q]; */
    /* for (int b=0; b < dt->img->m; b++) */
    /* 	dt->cumfeat->val[r][b] -= dt->img->val[q][b]; */
    /* dt->treesize[r] -= 1; */

    /* update properties of the new tree of q */
    int p      = dt->pred->val[q];
    int r      = dt->root->val[p];
    for (ulong b=0; b < dt->img->m; b++)
        dt->cumfeat->val[r][b] += dt->img->val[q][b];
    dt->treesize[r] += 1;
    float arcw = 0;
    for (ulong b=0; b < dt->img->m; b++){
      arcw += (dt->img->val[q][b] - dt->cumfeat->val[r][b]/dt->treesize[r])*
    (dt->img->val[q][b] - dt->cumfeat->val[r][b]/dt->treesize[r]);
    }
    arcw              = CMAX * (arcw / dt->maxfeatdist);
    dt->cost->val[q]  = iftMax(dt->cost->val[p], iftMin(arcw,CMAX));
    dt->label->val[q] = dt->label->val[p];
    dt->root->val[q]  = dt->root->val[p];

    /* Insert the childree of q in T to pursue the tree update
       process */
    p          = q;
    iftVoxel u = iftMGetVoxelCoord(dt->img,p);
    for (int i=1; i < dt->A->n; i++) {
      iftVoxel v = iftGetAdjacentVoxel(dt->A,u,i);
      if (iftMValidVoxel(dt->img,v)){
    int q = iftMGetVoxelIndex(dt->img, v);
    if (dt->pred->val[q]==p){
      iftInsertSet(&T,q);
    }
      }
    }
  }
/* } */
}
