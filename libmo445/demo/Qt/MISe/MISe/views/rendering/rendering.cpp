#include "rendering.h"

/*
 *  --------------- PRIVATE FUNCTIONS DECLARATIONS ---------------
 */
float iftPhongShading(iftGraphicalContext *gc, iftVector N, float dist);
void iftSliceRenderingAlongRay(iftGraphicalContext *gc, iftImage *img, int po, iftPoint P0, iftPoint P1, iftPoint Pn,
                               float *r, float *g, float *b, iftAdjRel *A, int a, int c, int s, char m);
void iftFastSliceRenderingAlongRay(iftGraphicalContext *gc, iftImage *img, iftPoint P0, iftPoint P1, iftPoint Pn,
                                   float *r, float *g, float *b, int a, int c, int s, char m);

iftObjectAttributes *iftCreateObjectAttributes(iftImage *label, int *nobjs);
iftSRBuffers *iftCreateSRBuffers(iftImage *label);
void iftResetSRBuffers(iftGraphicalContext *gc);

void iftDDA(iftPoint P1,iftPoint Pn,int *n,float *dx,float *dy,float *dz);
iftImage *iftDrawWireframe(iftGraphicalContext *gc, iftImage *img, iftImage *projection, iftVector normal);
/* ---------------------------------------------------------------------------------------------- */

Rendering::Rendering(iftImage *image, iftImage *label)
{
    iftFImage *scene = iftImageToFImage(image);

    gc = iftCreateGraphicalContext(scene,label);
    //printf("%p\n", gc->opacity);
    //fflush(stdout);

    if (label != nullptr)
        iftSetObjectNormalParallelized(gc,OBJECT_NORMAL);

    phongReflectionFactor = 1.0;

    iftSetViewDir(gc,0,0);

    mode = QUALITY_MODE;

    projectionMode = true;

    wireframe = true;

    normalizationValue = 255.0;

    indexbuffer = nullptr;


    iftDestroyFImage(&scene);
}

Rendering::Rendering(iftImage *image, iftImage *label, float ka, float kd, float ks)
{
    iftFImage *scene = iftImageToFImage(image);

    gc = iftCreateGraphicalContext(scene,label);
    iftDestroyFImage(&scene);

    if (label != nullptr)
        iftSetObjectNormalParallelized(gc,OBJECT_NORMAL);

    phongReflectionFactor = 1.0;

    ka = 0.1;
    kd = 0.7;
    ks = 0.2;
    if ((ka + kd + ks) == 1.0){
        iftSetAmbientReflection(gc, ka);
        iftSetDiffuseReflection(gc, kd);
        iftSetSpecularReflection(gc, ks);
    }

    iftSetViewDir(gc,0,0);

    mode = QUALITY_MODE;

    projectionMode = true;

    wireframe = true;

    normalizationValue = 255.0;

    indexbuffer = nullptr;
    // ToDo: Create a message box warning the user in case of
    // the light reflections don't sum up 1.0


    iftDestroyFImage(&scene);
}

Rendering::~Rendering()
{
    iftDestroyGraphicalContext(gc);
    iftDestroyImage(&indexbuffer);
}

void Rendering::setAmbientLightReflection(float ka)
{
    iftSetAmbientReflection(gc, ka);
}

void Rendering::setDiffuseLightReflection(float kd)
{
    iftSetDiffuseReflection(gc, kd);
}

void Rendering::setSpecularLightReflection(float ks)
{
    iftSetSpecularReflection(gc, ks);
}

void Rendering::setSceneOnGraphicalContext(iftImage *scene)
{
    iftFImage *scn = iftImageToFImage(scene);
    iftDestroyFImage(&(gc->scene));
    gc->scene = scn;
}

void Rendering::setViewDirections(double tilt, double spin)
{
    iftSetViewDir(gc,float(tilt),float(spin));
    this->tilt = tilt;
    this->spin = spin;
}

void Rendering::getViewDirections(double *tilt, double *spin)
{
    *tilt = this->tilt;
    *spin = this->spin;
}

void Rendering::setMode(char mode)
{
    if ((mode == FAST_MODE) || (mode == QUALITY_MODE))
        this->mode = mode;
}

#include <QDebug>
void Rendering::setGraphicalContextLabel(iftImage *lb)
{
    QElapsedTimer elapsedTime;
    elapsedTime.start();
    if (lb != nullptr){
        if (gc->label != nullptr)
            destroyLabel();
        gc->label       = iftCopyImage(lb);
        iftWriteOnLog("Coping label image to Graphics Context took",elapsedTime.elapsed());
        elapsedTime.start();
        iftAdjRel *A    = iftSpheric(1.0);
        gc->border      = iftObjectBorders(lb,A, true, true);
        iftWriteOnLog("Creating objects border on Graphics Context took",elapsedTime.elapsed());
        elapsedTime.start();
        iftDestroyAdjRel(&A);
        gc->object      = iftCreateObjectAttributes(lb,&gc->nobjs);
        iftWriteOnLog("Creating objects attributes on Graphics Context took",elapsedTime.elapsed());
        elapsedTime.start();
        gc->surf_render = iftCreateSRBuffers(lb);
        iftWriteOnLog("Creating SR Buffers on Graphics Context took",elapsedTime.elapsed());
        elapsedTime.start();
        iftSetObjectNormalParallelized(gc, OBJECT_NORMAL);
        //iftSetObjectNormal(gc,OBJECT_NORMAL);
        iftWriteOnLog("Setting object normal took",elapsedTime.elapsed());
    }
}

void Rendering::setObjectColor(int obj, iftColor YCbCr)
{
    iftColor RGB = iftYCbCrtoRGB(YCbCr,int(normalizationValue));
    float r,g,b;
    r = RGB.val[0] / normalizationValue;
    g = RGB.val[1] / normalizationValue;
    b = RGB.val[2] / normalizationValue;
    iftSetObjectColor(this->gc,obj,r,g,b);
}

void Rendering::setObjectOpacity(int obj, float opac)
{
    iftSetObjectOpacity(this->gc,obj,opac);
}

void Rendering::setObjectVisibility(int obj, bool visible)
{
    if (visible)
        iftSetObjectVisibility(this->gc,obj,1);
    else
        iftSetObjectVisibility(this->gc,obj,0);
}

bool *Rendering::getObjectsVisibility()
{
    bool *out;
    out = (bool*)calloc(gc->nobjs+1,sizeof(bool));
    if (out){
        for (int i = 1; i <= gc->nobjs; i++){
            if (gc->object[i].visibility)
                out[i] = true;
            else
                out[i] = false;
        }
    }
    return out;
}

bool Rendering::isGraphicalContextLabelEmpty()
{
    if (gc->label == nullptr)
        return true;
    else
        return false;
}

void Rendering::destroyLabel()
{
    if (gc->label != nullptr)
        iftDestroyImage(&gc->label);
    if (gc->border != nullptr)
        iftDestroyImage(&gc->border);
    if (gc->object != nullptr) {
        iftFree(gc->object);
        gc->object = nullptr;
    }
    if (gc->surf_render != nullptr) {
        iftFree(gc->surf_render);
        gc->surf_render = nullptr;
    }
    gc->nobjs = 0;
}

char Rendering::getMode()
{
    return mode;
}

iftImage *Rendering::getIndexBuffer()
{
    iftImage *ib = iftCopyImage(indexbuffer);
    return ib;
}

iftVoxel Rendering::getVoxelFromIndexBuffer(int x, int y)
{
    iftVoxel u = {x,y,1,0}, v;
    if (iftValidVoxel(indexbuffer,u)){
        int p = iftGetVoxelIndex(indexbuffer,u);
        v = iftGetVoxelCoord(indexbuffer,indexbuffer->val[p]);
        return v;
    } else {
        v.x = -1;
        v.y = -1;
        v.z = -1;
        return v;
    }
}

iftVoxel Rendering::getVoxelFromIndexBuffer(iftVoxel u)
{
    iftVoxel v;
    if (iftValidVoxel(indexbuffer,u)){
        int p = iftGetVoxelIndex(indexbuffer,u);
        v = iftGetVoxelCoord(gc->label,indexbuffer->val[p]);
        if (iftValidVoxel(gc->label,v))
            return v;
    }
    v.x = -1;
    v.y = -1;
    v.z = -1;
    return v;
}

void Rendering::toogleWireFrameVisibility()
{
    wireframe = !wireframe;
}

void Rendering::toogleProjectionMode()
{
    projectionMode = !projectionMode;
}

iftImage *Rendering::iftRender(iftImage *img, int axialSlice, int coronalSlice, int sagittalSlice)
{

    int stride = this->mode;
    int diagonal;
    iftPoint  P0,P1,Pn;
    iftImage *projection = nullptr;    
    iftVector n;
    iftGraphicalContext *aux = this->gc;
    iftAdjRel *A = iftSpheric(1.0);

    diagonal = iftDiagonalSize(img);

    n.x = 0;
    n.y = 0;
    n.z = 1;
    n = iftTransformVector(gc->viewdir->Rinv,n);

    if (gc->label != nullptr)
        iftResetSRBuffers(gc);

    projection = iftCreateImage(diagonal,diagonal,1);
    for (int i = 0; i < projection->n; i++)
        projection->val[i] = 0;
    iftSetCbCr(projection,((int)normalizationValue)/2+1);

#pragma omp parallel for shared(projection,aux,n,A) private(P0,P1,Pn)//val is private
    for (int po=0; po < projection->n; po+=stride) {

        float red=0.0, green=0.0, blue=0.0;
        iftColor  RGB, YCbCr;

        P0.x   =  iftGetXCoord(projection,po);
        P0.y   =  iftGetYCoord(projection,po);
        P0.z   = -diagonal/2.0;
        P0     =  iftTransformPoint(aux->viewdir->Tinv,P0);

        if (iftIntersectionPoints(aux,P0,n,&P1,&Pn)){
            if (this->mode == FAST_MODE)
                iftFastSliceRenderingAlongRay(aux,img,P0,P1,Pn,&red,&green,&blue,axialSlice,coronalSlice,sagittalSlice,this->projectionMode);
            else
                iftSliceRenderingAlongRay(aux,img,po,P0,P1,Pn,&red,&green,&blue,A,axialSlice,coronalSlice,sagittalSlice,this->projectionMode);
            RGB.val[0]     = (int)(normalizationValue*red);
            RGB.val[1]     = (int)(normalizationValue*green);
            RGB.val[2]     = (int)(normalizationValue*blue);
            YCbCr          = iftRGBtoYCbCr(RGB,(int)normalizationValue);
            projection->val[po] = YCbCr.val[0];
            projection->Cb[po]  = YCbCr.val[1];
            projection->Cr[po]  = YCbCr.val[2];
        }
    }
    iftDestroyAdjRel(&A);

    if (wireframe)
        iftDrawWireframe(gc,img,projection,n);

    if ((this->mode == QUALITY_MODE) && (gc->label != nullptr)){
        iftDestroyImage(&indexbuffer);
        indexbuffer = iftCreateImageFromImage(projection);
        for (int p=0; p < projection->n; p++)
            indexbuffer->val[p] = gc->surf_render[p].voxel;
    }

    return projection;
}

/*
 *
 *  ---------------  PRIVATE FUNCTIONS  ---------------
 *
 */

float iftPhongShading(iftGraphicalContext *gc, iftVector N, float dist)
{
    float cos_theta;
    float cos_2theta, pow, phong_val=0.0;

    cos_theta   = iftVectorInnerProduct(gc->viewdir->V, N);

    /* |angle| <= 90° */

    if (cos_theta >= IFT_EPSILON){

        cos_2theta = 2*cos_theta*cos_theta - 1;

        if (cos_2theta <= IFT_EPSILON){  /* |angle| >= 45° */
            pow = 0.;
        }else {
            pow = 1.;
            for (int k=0; k < gc->phong->ns; k++)
                pow = pow*cos_2theta;
        }

        phong_val = gc->phong->ka + gc->phong->Idist[(int)dist]*(gc->phong->kd*cos_theta + gc->phong->ks*pow);
    }

    return(phong_val);
}

/**
 * @brief Returns the RGB values of a voxel on a surface of an object according Phong's Reflection Model
 *
 * @author Azael de Melo e Sousa
 *
 * This method returns the RGB values of a spedific voxel over the surface of an object, following Phong's Reflection Model. From a given
 * transformed pixel of the projection plane, a ray is casted along the volume and if it hits the object, the ambient, diffusal e specular luminances
 * are computed and converted to RGB values.
 *
 * @param <gc> Pointer to Graphical Context data structure
 * @param <img> Pointer to an image stored in a iftImage data structure
 * @param <P0> Transformed point of the projection plane
 * @param <P1> First intersection point of the volume by Ray Casting
 * @param <Pn> Last intersection point of the volume by Ray Casting
 * @param <voxel> Voxel on the volume that matches the pixel on the projection (index buffer)
 * @param <r> Value of the red visible spectrum
 * @param <g> Value of the green visible spectrum
 * @param <b> Value of the blue visible spectrum
 * @param <A> Adjacency Relation data structure
 * @param <a> Axial slice in case of intersection of the ray against it
 * @param <c> Coronal slice in case of intersection of the ray against it
 * @param <s> Sagittal slice in case of intersection of the ray against it
 * @param <m> Projection Mode, if true, the anatomical planes will be drwan on the projection plane
 * @return Same image stored in a QImage data structure
 */
void iftSliceRenderingAlongRay(iftGraphicalContext *gc, iftImage *img, int po, iftPoint P0, iftPoint P1, iftPoint Pn,
                               float *r, float *g, float *b, iftAdjRel *A, int a, int c, int s, char m)
{
    int i,k;
    int n, val, p;
    iftIntArray *obj_flag = iftCreateIntArray(gc->nobjs+1);
    float dx = 0,dy = 0,dz = 0,phong_val,dist;
    double opac, acum_opacity;
    iftPoint u;
    iftVoxel v,w;
    iftVector N;
    char flag;
    bool isInsideObj, isInsideSlice;

    iftDDA(P1, Pn,&n, &dx, &dy, &dz);

    u.x = P1.x;
    u.y = P1.y;
    u.z = P1.z;

    val = 0;

    *r = *g = *b = 0.0;

    phong_val = 0;
    isInsideObj = false;
    isInsideSlice = false;

    for (k=1; k <= gc->nobjs; k++)
            obj_flag->val[k]=0;

    for (k=0, acum_opacity=1.0; (k < n) && (acum_opacity > IFT_EPSILON); k++) {

        v.x = iftRound(u.x);
        v.y = iftRound(u.y);
        v.z = iftRound(u.z);

        flag=0;
        if (gc->label != nullptr){
            for (i=0; (i < A->n)&&(flag==0); i++) {
                w = iftGetAdjacentVoxel(A,v,i);
                if(iftFValidVoxel(gc->scene, w)){
                    p = iftFGetVoxelIndex(gc->scene,w);
                    if ((gc->border->val[p]!=0)&&(obj_flag->val[gc->label->val[p]]==0)){
                        flag=1;
                        if (gc->object[gc->label->val[p]].visibility != 0){
                            if (gc->object[gc->label->val[p]].opacity > IFT_EPSILON){
                                dist = iftPointDistance(u,P0);
                                gc->surf_render[po].depth = dist;
                                gc->surf_render[po].voxel = p;
                                gc->surf_render[po].object = gc->label->val[p];
                                N.x  = gc->phong->normal[gc->normal->val[p]].x;
                                N.y  = gc->phong->normal[gc->normal->val[p]].y;
                                N.z  = gc->phong->normal[gc->normal->val[p]].z;
                                opac = gc->object[gc->label->val[p]].opacity;
                                phong_val = opac * iftPhongShading(gc,N,dist) * acum_opacity;
                                *r += phong_val * gc->object[gc->label->val[p]].red;
                                *g += phong_val * gc->object[gc->label->val[p]].green;
                                *b += phong_val * gc->object[gc->label->val[p]].blue;
                                acum_opacity = acum_opacity * (1.0 -  opac);
                                obj_flag->val[gc->label->val[p]]=1;
                                isInsideObj = true;
                            }
                        }
                    }
                }
            }
        }


        if ((iftValidVoxel(img,v)) && (m) && (!isInsideObj)){
            if ((v.x == s) || (v.y == c) || (v.z == a)){
                p = iftGetVoxelIndex(img,v);
                if (img->val[p] != 0.0){
                    val = img->val[p];
                    *r += val/255.0;
                    *g += val/255.0;
                    *b += val/255.0;
                    iftFree(obj_flag);
                    isInsideSlice = true;
                    return;
                }
            }
        }

        u.x = u.x + dx;
        u.y = u.y + dy;
        u.z = u.z + dz;

    }

    iftFree(obj_flag);

    if ((isInsideObj == false) && (isInsideSlice == false))
        *r = *g = *b = 0.0; /* BACKGROUND COLOR */

    if (*r<0) *r=0;
    if (*g<0) *g=0;
    if (*b<0) *b=0;
    if (*r>1) *r=1;
    if (*g>1) *g=1;
    if (*b>1) *b=1;
}

void iftFastSliceRenderingAlongRay(iftGraphicalContext *gc, iftImage *img, iftPoint P0, iftPoint P1, iftPoint Pn,
                                   float *r, float *g, float *b, int a, int c, int s, char m)
{
    int k;
    int n, val, p;
    iftIntArray *obj_flag = iftCreateIntArray(gc->nobjs+1);
    float dx = 0,dy = 0,dz = 0,phong_val,dist;
    iftPoint u;
    iftVoxel v;
    iftVector N;
    char flag;
    bool isInsideObj, isInsideSlice;

    iftDDA(P1, Pn,&n, &dx, &dy, &dz);

    u.x = P1.x;
    u.y = P1.y;
    u.z = P1.z;

    val = 0;

    *r = *g = *b = 0.0;

    phong_val = 0;
    isInsideObj = false;
    isInsideSlice = false;

    for (k=1; k <= gc->nobjs; k++)
            obj_flag->val[k]=0;

    for (k=0; (k < n); k++) {

        v.x = iftRound(u.x);
        v.y = iftRound(u.y);
        v.z = iftRound(u.z);

        flag=0;
        if (gc->label != nullptr){
            if(iftFValidVoxel(gc->scene, v)){
                p = iftFGetVoxelIndex(gc->scene,v);
                if ((gc->border->val[p]!=0)&&(obj_flag->val[gc->label->val[p]]==0)){
                    flag=1;
                    if (gc->object[gc->label->val[p]].visibility != 0){
                        dist = iftPointDistance(u,P0);
                        N.x  = gc->phong->normal[gc->normal->val[p]].x;
                        N.y  = gc->phong->normal[gc->normal->val[p]].y;
                        N.z  = gc->phong->normal[gc->normal->val[p]].z;
                        phong_val = iftPhongShading(gc,N,dist);
                        *r += phong_val * gc->object[gc->label->val[p]].red;
                        *g += phong_val * gc->object[gc->label->val[p]].green;
                        *b += phong_val * gc->object[gc->label->val[p]].blue;
                        obj_flag->val[gc->label->val[p]]=flag;
                        isInsideObj = true;
                        break;
                    }
                }
            }
        }

        if ((iftValidVoxel(img,v)) && (m) && (!isInsideObj)){
            if ((v.x == a) || (v.y == c) || (v.z == s)){
                p = iftGetVoxelIndex(img,v);
                if (img->val[p] != 0){
                    val = img->val[p];
                    *r = val/255.0;
                    *g = val/255.0;
                    *b = val/255.0;
                    iftFree(obj_flag);
                    isInsideSlice = true;
                    return;
                }
            }
        }



        u.x = u.x + dx;
        u.y = u.y + dy;
        u.z = u.z + dz;

    }

    iftFree(obj_flag);
    if ((isInsideObj == false) && (isInsideSlice == false))
        *r = *g = *b = 1.0;

    if (*r<0) *r=0;
    if (*g<0) *g=0;
    if (*b<0) *b=0;
    if (*r>1) *r=1;
    if (*g>1) *g=1;
    if (*b>1) *b=1;
}

iftObjectAttributes *iftCreateObjectAttributes(iftImage *label, int *nobjs)
{
    iftObjectAttributes *object;
    *nobjs = iftMaximumValue(label);

    object = (iftObjectAttributes *)iftAlloc(*nobjs+1,sizeof(iftObjectAttributes));


    /* background */

    object[0].opacity    = 0;
    object[0].red        = 0;
    object[0].green      = 0;
    object[0].blue       = 0;
    object[0].visibility = 0;

    /* default for objects */

    for (int i=1; i <= *nobjs; i++){
        object[i].opacity    = 1;
        object[i].red        = 1;
        object[i].green      = 1;
        object[i].blue       = 1;
        object[i].visibility = 1;
    }

    return(object);
}

iftSRBuffers *iftCreateSRBuffers(iftImage *label)
{
    iftSRBuffers *surf_render;
    int n = iftDiagonalSize(label);

    n = n*n;
    surf_render            = (iftSRBuffers *) iftAlloc(n,sizeof(iftSRBuffers));

    for (int p=0; p < n; p++) {
        surf_render[p].depth    = IFT_INFINITY_FLT;
        surf_render[p].opacity  = 1.0;
        surf_render[p].voxel    = IFT_NIL;
        surf_render[p].object   = IFT_NIL;
    }

    return(surf_render);
}

void iftResetSRBuffers(iftGraphicalContext *gc)
{
    int n = iftDiagonalSize(gc->label);

    n = n*n;
    for (int p=0; p < n; p++) {
        gc->surf_render[p].depth    = IFT_INFINITY_FLT;
        gc->surf_render[p].opacity  = 1.0;
        gc->surf_render[p].voxel    = IFT_NIL;
        gc->surf_render[p].object   = IFT_NIL;
    }
}

void iftDDA(iftPoint P1,iftPoint Pn,int *n,float *dx,float *dy,float *dz)
{
    float Dx=(Pn.x-P1.x), Dy=(Pn.y-P1.y), Dz=(Pn.z-P1.z);

    if (iftVectorsAreEqual(P1, Pn)) {
        *n = 1;
    }else{ /* process points from P1 to Pn */
        if ((fabs(Dx) >= fabs(Dy))&&(fabs(Dx) >= fabs(Dz))) { /* Dx is the maximum projection of vector P1Pn */
            *n  = (int)(fabs(Dx)+1);
            *dx = iftSign(Dx);
            *dy = (*dx)*Dy/Dx;
            *dz = (*dx)*Dz/Dx;
        }else{
            if ((fabs(Dy) >= fabs(Dx))&&(fabs(Dy) >= fabs(Dz))) { /* Dy is the maximum projection of vector P1Pn */
                *n  = (int)(fabs(Dy)+1);
                *dy = iftSign(Dy);
                *dx = (*dy)*Dx/Dy;
                *dz = (*dy)*Dz/Dy;
            } else { /* Dz is the maximum projection of vector P1Pn */
                *n  = (int)(fabs(Dz)+1);
                *dz = iftSign(Dz);
                *dx = (*dz)*Dx/Dz;
                *dy = (*dz)*Dy/Dz;
            }
        }
    }
}

bool iftVoxelAtColoredImageIsBlack(iftImage *projection, int q)
{
    if ((projection->Cb == nullptr) || (projection->Cr == nullptr))
        return projection->val[q];

    //According to the IFT implementation, when a voxel is black on the YCbCr color space,
    //the values for Y, Cb and Cr are 16, 128, 128 respectivelly
    if ((projection->val[q] <= 16) && (projection->Cb[q] == 128) && (projection->Cr[q] == 128))
       return true;
    return false;
}

void iftDefineWireframeVertices(iftPoint p[][4], iftImage *img)
{
    /* Plane 0 */
    p[0][0].x = 0;
    p[0][0].y = 0;
    p[0][0].z = 0;

    p[0][1].x = img->xsize;
    p[0][1].y = 0;
    p[0][1].z = 0;

    p[0][2].x = img->xsize;
    p[0][2].y = img->ysize;
    p[0][2].z = 0;

    p[0][3].x = 0;
    p[0][3].y = img->ysize;
    p[0][3].z = 0;

    /* Plane 1 */
    p[1][0].x = 0;
    p[1][0].y = 0;
    p[1][0].z = img->zsize;

    p[1][1].x = img->xsize;
    p[1][1].y = 0;
    p[1][1].z = img->zsize;

    p[1][2].x = img->xsize;
    p[1][2].y = img->ysize;
    p[1][2].z = img->zsize;

    p[1][3].x = 0;
    p[1][3].y = img->ysize;
    p[1][3].z = img->zsize;

    /* Plane 2 */
    p[2][0].x = 0;
    p[2][0].y = 0;
    p[2][0].z = 0;

    p[2][1].x = img->xsize;
    p[2][1].y = 0;
    p[2][1].z = 0;

    p[2][2].x = img->xsize;
    p[2][2].y = 0;
    p[2][2].z = img->zsize;

    p[2][3].x = 0;
    p[2][3].y = 0;
    p[2][3].z = img->zsize;

    /* Plane 3 */
    p[3][0].x = 0;
    p[3][0].y = img->ysize;
    p[3][0].z = 0;

    p[3][1].x = img->xsize;
    p[3][1].y = img->ysize;
    p[3][1].z = 0;

    p[3][2].x = img->xsize;
    p[3][2].y = img->ysize;
    p[3][2].z = img->zsize;

    p[3][3].x = 0;
    p[3][3].y = img->ysize;
    p[3][3].z = img->zsize;

    /* Plane 4 */
    p[4][0].x = 0;
    p[4][0].y = 0;
    p[4][0].z = 0;

    p[4][1].x = 0;
    p[4][1].y = 0;
    p[4][1].z = img->zsize;

    p[4][2].x = 0;
    p[4][2].y = img->ysize;
    p[4][2].z = img->zsize;

    p[4][3].x = 0;
    p[4][3].y = 0;
    p[4][3].z = img->zsize;

    /* Plane 5 */
    p[5][0].x = img->xsize;
    p[5][0].y = 0;
    p[5][0].z = 0;

    p[5][1].x = img->xsize;
    p[5][1].y = 0;
    p[5][1].z = img->zsize;

    p[5][2].x = img->xsize;
    p[5][2].y = img->ysize;
    p[5][2].z = img->zsize;

    p[5][3].x = img->xsize;
    p[5][3].y = 0;
    p[5][3].z = img->zsize;
}

iftImage *iftDrawWireframe(iftGraphicalContext *gc, iftImage *img, iftImage *projection, iftVector normal)
{
    /*
     * This function draws the wireframe on the projection plane.
     * Using the cubic faces stores in the graphical context, we can
     * compute which faces are visible and which are not.
     *
       __________
      /         /|
     /    2    / | 1
    /_________/  |
    |         | 5|
  4 |    0    |  /
    |         | /
    |_________|/
            3

    faces  |  normal
    -----------------
      0    | (0,0,-1)
      1    | (0,0,1)
      2    | (0,-1,0)
      3    | (0,1,0)
      4    | (-1,0,0)
      5    | (1,0,0)

      * This method can be improved by avoiding reprinting the same line m
      * ore than once. If the software is

    */

    int i,j,k,q,n,max;
    float dx,dy,dz;
    iftPoint p[6][4];
    iftPoint p0, p1, p2;
    iftVoxel u;

    max = 255;

    iftDefineWireframeVertices(p,img);


    for (i = 0; i < 6; i++){
        for (j = 0; j < 4; j++){
            p1 = iftTransformPoint(gc->viewdir->T,p[i][j]);
            p2 = iftTransformPoint(gc->viewdir->T,p[i][(j+1)%4]);

            iftDDA(p1,p2,&n,&dx,&dy,&dz);

            p0.x = p1.x;
            p0.y = p1.y;

            for (k=0; k < n; k++){

                u.x = iftRound(p0.x);
                u.y = iftRound(p0.y);
                u.z = 0;

                if (iftValidVoxel(projection,u)){
                    q = iftGetVoxelIndex(projection,u);
                    if (iftVectorInnerProduct(normal,gc->face[i].normal) < 0)
                        projection->val[q] = max;
                    else if (iftVoxelAtColoredImageIsBlack(projection,q)){
                        projection->val[q] = max;
                    }
                }

                p0.x += dx;
                p0.y += dy;
            }
        }
    }

    return projection;
}
