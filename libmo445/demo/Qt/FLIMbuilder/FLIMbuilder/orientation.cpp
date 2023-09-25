#include "orientation.h"

Orientation::Orientation()
{
    slice = 0;
    plane = 0;
    dimension = 3;

    for (int i = 0; i < 3; i++){
        offset[i].x = 0;
        offset[i].y = 0;
        offset[i].z = 0;
        offset[i].t = 0;
    }

    setOrientationToRadiologists();
}

Orientation::~Orientation()
{
    iftDestroyMatrix(&A);
    iftDestroyMatrix(&C);
    iftDestroyMatrix(&S);

    iftDestroyMatrix(&Ainv);
    iftDestroyMatrix(&Cinv);
    iftDestroyMatrix(&Sinv);
}


iftMatrix *Orientation::getAxialOrientationMatrix()
{
    return A;
}

iftMatrix *Orientation::getCoronalOrientationMatrix()
{
    return C;
}

iftMatrix *Orientation::getSagittalOrientationMatrix()
{
    return S;
}

iftMatrix *Orientation::getAxialInvertedOrientationMatrix()
{
    return Ainv;
}

iftMatrix *Orientation::getCoronalInvertedOrientationMatrix()
{
    return Cinv;
}

iftMatrix *Orientation::getSagittalInvertedOrientationMatrix()
{
    return Sinv;
}

void Orientation::setAxialOrientationMatrix(iftMatrix *m)
{
    iftDestroyMatrix(&A);
    A = iftCopyMatrix(m);
}

void Orientation::setCoronalOrientationMatrix(iftMatrix *m)
{
    iftDestroyMatrix(&C);
    C = iftCopyMatrix(m);
}

void Orientation::setSagittalOrientationMatrix(iftMatrix *m)
{
    iftDestroyMatrix(&S);
    S = iftCopyMatrix(m);
}

void Orientation::setOrientationToRadiologists()
{
    iftDestroyMatrix(&A);
    iftDestroyMatrix(&Ainv);
    iftDestroyMatrix(&S);
    iftDestroyMatrix(&Sinv);
    iftDestroyMatrix(&C);
    iftDestroyMatrix(&Cinv);

    this->A = iftIdentityMatrix(dimension);
    this->C = iftCreateMatrix(dimension,dimension);
    this->S = iftCreateMatrix(dimension,dimension);

    /*
     * Creating the transformation matrix for the coronal
     * and sagittal slices according to the preferences
     * of radiologists.
     *
     * Since the IFT deafult orientation of a volume is L2R
     * along the X-axis, A2P along the Y-axis and I2S along
     * the Z-axis, we need to change the orientation of the
     * coronal and sagittal slices to fit the phisian's way.
     *
     * C -> coronal transformation matrix
     * S -> sagittal transformation matrix
     */

    // R(col,row)
    iftMatrixElem(C, 0, 0) = 0;
    iftMatrixElem(C, 0, 1) = -1;
    iftMatrixElem(C, 0, 2) = 0;
    iftMatrixElem(C, 1, 0) = 1;
    iftMatrixElem(C, 1, 1) = 0;
    iftMatrixElem(C, 1, 2) = 0;
    iftMatrixElem(C, 2, 0) = 0;
    iftMatrixElem(C, 2, 1) = 0;
    iftMatrixElem(C, 2, 2) = 1;

    iftMatrixElem(S, 0, 0) = 1;
    iftMatrixElem(S, 0, 1) = 0;
    iftMatrixElem(S, 0, 2) = 0;
    iftMatrixElem(S, 1, 0) = 0;
    iftMatrixElem(S, 1, 1) = -1;
    iftMatrixElem(S, 1, 2) = 0;
    iftMatrixElem(S, 2, 0) = 0;
    iftMatrixElem(S, 2, 1) = 0;
    iftMatrixElem(S, 2, 2) = 1;


    this->Ainv = iftInvertMatrix(this->A);
    this->Cinv = iftInvertMatrix(this->C);
    this->Sinv = iftInvertMatrix(this->S);
}

iftVoxel Orientation::mapPixelToVolume(iftVoxel u, int slice, int plane)
{
    iftVoxel v = {0,0,1,0};
    iftVoxel t = {0,0,0,0};
    iftVoxel w = {0,0,0,0};
    iftMatrix *R = nullptr;
    switch (plane){
        case AXIAL:
            R = iftCopyMatrix(Ainv);
            v = iftVoxel(iftVoxelSub(u,offset[plane]));
            t = iftTransformVoxel(R, v);
            w.x = t.x;
            w.y = t.y;
            w.z = slice;
            break;
        case CORONAL:
            R = iftCopyMatrix(Cinv);
            v = iftVoxel(iftVoxelSub(u,offset[plane]));
            t = iftTransformVoxel(R, v);
            w.x = t.y;
            w.y = slice;
            w.z = t.x;
           break;
        case SAGITTAL:
            R = iftCopyMatrix(Sinv);
            v = iftVoxel(iftVoxelSub(u,offset[plane]));
            t = iftTransformVoxel(R, v);
            w.x = slice;
            w.y = t.x;
            w.z = t.y;
            break;
        default:
            return v;
    }
    iftDestroyMatrix(&R);
    return w;
}

iftVoxel Orientation::mapPixelToVolume(iftVoxel u)
{
    iftVoxel v = {0,0,1,0};
    iftVoxel t = {0,0,0,0};
    iftVoxel w = {0,0,0,0};
    iftMatrix *R = nullptr;
    switch (this->plane){
        case AXIAL:
            R = iftCopyMatrix(Ainv);
            v = iftVoxel(iftVoxelSub(u,offset[this->plane]));
            t = iftTransformVoxel(R, v);
            w.x = t.x;
            w.y = t.y;
            w.z = this->slice;
            break;
        case CORONAL:
            R = iftCopyMatrix(Cinv);
            v = iftVoxel(iftVoxelSub(u,offset[this->plane]));
            t = iftTransformVoxel(R, v);
            w.x = t.y;
            w.y = this->slice;
            w.z = t.x;
           break;
        case SAGITTAL:

            R = iftCopyMatrix(Sinv);
            v = iftVoxel(iftVoxelSub(u,offset[this->plane]));
            t = iftTransformVoxel(R, v);
            w.x = this->slice;
            w.y = t.x;
            w.z = t.y;
            break;
        default:
            return v;
    }
    iftDestroyMatrix(&R);
    return w;
}

iftImage* Orientation::applyOrientationOnSlice(iftImage *img, int plane)
{

    iftMatrix *R = nullptr;
    switch (plane){
        case AXIAL:
            R = iftCopyMatrix(A);
            break;
        case CORONAL:
            R = iftCopyMatrix(C);
            break;
        case SAGITTAL:
            R = iftCopyMatrix(S);
            break;
    }

    if (R == nullptr){
        printf("Error in applyOrientationSlice. Please, select a valid anatomical plane.\n");
        return nullptr;
    }

    iftImageDomain new_dom = {0, 0, 1,0};
    iftVoxel t = {img->xsize-1, img->ysize-1, img->zsize-1,0};

    int sizes[3];
    sizes[0] = img->xsize;
    sizes[1] = img->ysize;
    sizes[2] = img->zsize;

    for (int j = 0; j <= 2; j++) {
            if (((double(iftMatrixElem(R, j, 0)) < -0.999999) && (double(iftMatrixElem(R, j, 0)) > -1.000001)) ||
                ((double(iftMatrixElem(R, j, 0)) < 1.000001) && (double(iftMatrixElem(R, j, 0)) > 0.9999999))) {
                new_dom.xsize = sizes[j];
                continue;
            }
            if (((double(iftMatrixElem(R, j, 1)) < -0.999999) && (double(iftMatrixElem(R, j, 1)) > -1.000001)) ||
                ((double(iftMatrixElem(R, j, 1)) < 1.000001) && (double(iftMatrixElem(R, j, 1)) > 0.9999999))) {
                new_dom.ysize = sizes[j];
                continue;
            }
        }

    iftImage *out_img = nullptr;
    if (iftIsColorImage(img))
        out_img = iftCreateColorImage(new_dom.xsize, new_dom.ysize, new_dom.zsize, 0);
    else
        out_img = iftCreateImage(new_dom.xsize, new_dom.ysize, new_dom.zsize);

    offset[plane] = iftTransformVoxel(R, t);
    offset[plane].x = (offset[plane].x < 0) ? abs(offset[plane].x) : 0;
    offset[plane].y = (offset[plane].y < 0) ? abs(offset[plane].y) : 0;
    iftCopyVoxelSize(img,out_img);
    offset[plane].z = 0;

    #pragma omp parallel for
    for (int p = 0; p < img->n; p++) {
        iftVoxel u = iftGetVoxelCoord(img, p);
        u.t = 0;
        iftVoxel v = iftTransformVoxel(R, u);
        v.t = 0;
        v = iftVoxel(iftVoxelSum(v, offset[plane]));
        if ((iftValidVoxel(img,u)) && (iftValidVoxel(out_img,v))){
            int p = iftGetVoxelIndex(img,u);
            int q = iftGetVoxelIndex(out_img,v);
            out_img->val[q] = img->val[p];
            if (iftIsColorImage(img)) {
                out_img->Cb[q] = img->Cb[p];
                out_img->Cr[q] = img->Cr[p];
            }
        }
    }

    iftDestroyMatrix(&R);

    return out_img;

}

