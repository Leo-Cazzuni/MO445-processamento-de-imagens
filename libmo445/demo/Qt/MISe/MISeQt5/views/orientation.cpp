#include "orientation.h"

Orientation::Orientation()
{
    dimension = 4;
    A = nullptr;
    C = nullptr;
    S = nullptr;
    Ainv = nullptr;
    Cinv = nullptr;
    Sinv = nullptr;

    for (int i = 0; i < 3; i++){
        offset[i].x = 0;
        offset[i].y = 0;
        offset[i].z = 0;
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

iftMatrix *Orientation::createRotationMatrix()
{
    iftMatrix *R = iftCreateMatrix(dimension,dimension);

    R->val[iftGetMatrixIndex(R, 0, 0)] = 0.0;
    R->val[iftGetMatrixIndex(R, 1, 0)] = -1.0;
    R->val[iftGetMatrixIndex(R, 2, 0)] = 0.0;

    R->val[iftGetMatrixIndex(R, 0, 1)] = 1.0;
    R->val[iftGetMatrixIndex(R, 1, 1)] = 0.0;
    R->val[iftGetMatrixIndex(R, 2, 1)] = 0.0;

    R->val[iftGetMatrixIndex(R, 0, 2)] = 0.0;
    R->val[iftGetMatrixIndex(R, 1, 2)] = 0.0;
    R->val[iftGetMatrixIndex(R, 2, 2)] = 1.0;

    R->val[iftGetMatrixIndex(R, 0, 3)] = 0.0;
    R->val[iftGetMatrixIndex(R, 1, 3)] = 0.0;
    R->val[iftGetMatrixIndex(R, 2, 3)] = 0.0;
    R->val[iftGetMatrixIndex(R, 3, 3)] = 1.0;

    return R;
}

void Orientation::rotateAxialSlice()
{
    iftMatrix *R = createRotationMatrix();
    iftMatrix *T = iftMultMatrices(R,A);
    iftDestroyMatrix(&R);
    iftDestroyMatrix(&A);
    A = iftCopyMatrix(T);
    iftDestroyMatrix(&T);
}

void Orientation::rotateCoronalSlice()
{
    iftMatrix *R = createRotationMatrix();
    iftMatrix *T = iftMultMatrices(R,C);
    iftDestroyMatrix(&R);
    iftDestroyMatrix(&C);
    C = iftCopyMatrix(T);
    iftDestroyMatrix(&T);
}

void Orientation::rotateSagittalSlice()
{
    iftMatrix *R = createRotationMatrix();
    iftMatrix *T = iftMultMatrices(R,S);
    iftDestroyMatrix(&R);
    iftDestroyMatrix(&S);
    S = iftCopyMatrix(T);
    iftDestroyMatrix(&T);
}

iftMatrix *Orientation::createHorizontalFlipMatrix()
{
    iftMatrix *R = iftCreateMatrix(dimension,dimension);

    R->val[iftGetMatrixIndex(R, 0, 0)] = -1.0;
    R->val[iftGetMatrixIndex(R, 1, 0)] = 0.0;
    R->val[iftGetMatrixIndex(R, 2, 0)] = 0.0;

    R->val[iftGetMatrixIndex(R, 0, 1)] = 0.0;
    R->val[iftGetMatrixIndex(R, 1, 1)] = 1.0;
    R->val[iftGetMatrixIndex(R, 2, 1)] = 0.0;

    R->val[iftGetMatrixIndex(R, 0, 2)] = 0.0;
    R->val[iftGetMatrixIndex(R, 1, 2)] = 0.0;
    R->val[iftGetMatrixIndex(R, 2, 2)] = 1.0;

    R->val[iftGetMatrixIndex(R, 0, 3)] = 0.0;
    R->val[iftGetMatrixIndex(R, 1, 3)] = 0.0;
    R->val[iftGetMatrixIndex(R, 2, 3)] = 0.0;
    R->val[iftGetMatrixIndex(R, 3, 3)] = 1.0;

    return R;
}

void Orientation::flipAxialSlice()
{
    iftMatrix *R = createHorizontalFlipMatrix();
    iftMatrix *T = iftMultMatrices(R,A);
    iftDestroyMatrix(&R);
    iftDestroyMatrix(&A);
    A = iftCopyMatrix(T);
    iftDestroyMatrix(&T);
    iftDestroyMatrix(&Ainv);
    Ainv = iftInvertMatrix(A);
}

void Orientation::flipCoronalSlice()
{
    iftMatrix *R = createHorizontalFlipMatrix();
    iftMatrix *T = iftMultMatrices(R,C);
    iftDestroyMatrix(&R);
    iftDestroyMatrix(&C);
    C = iftCopyMatrix(T);
    iftDestroyMatrix(&T);
    iftDestroyMatrix(&Cinv);
    Cinv = iftInvertMatrix(C);
}

void Orientation::flipSagittalSlice()
{
    iftMatrix *R = createHorizontalFlipMatrix();
    iftMatrix *T = iftMultMatrices(R,S);
    iftDestroyMatrix(&R);
    iftDestroyMatrix(&S);
    S = iftCopyMatrix(T);
    iftDestroyMatrix(&T);
    iftDestroyMatrix(&Sinv);
    Sinv = iftInvertMatrix(S);
}

void Orientation::setOrientationToRadiologists()
{
    iftDestroyMatrix(&A);
    iftDestroyMatrix(&Ainv);
    iftDestroyMatrix(&S);
    iftDestroyMatrix(&Sinv);
    iftDestroyMatrix(&C);
    iftDestroyMatrix(&Cinv);

    A = iftIdentityMatrix(dimension);
    C = iftCreateMatrix(dimension,dimension);
    S = iftCreateMatrix(dimension,dimension);

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
    iftMatrixElem(C, 0, 3) = 0;
    iftMatrixElem(C, 1, 3) = 0;
    iftMatrixElem(C, 2, 3) = 0;
    iftMatrixElem(C, 3, 3) = 1;

    iftMatrixElem(S, 0, 0) = 1;
    iftMatrixElem(S, 0, 1) = 0;
    iftMatrixElem(S, 0, 2) = 0;
    iftMatrixElem(S, 1, 0) = 0;
    iftMatrixElem(S, 1, 1) = -1;
    iftMatrixElem(S, 1, 2) = 0;
    iftMatrixElem(S, 2, 0) = 0;
    iftMatrixElem(S, 2, 1) = 0;
    iftMatrixElem(S, 2, 2) = 1;
    iftMatrixElem(S, 0, 3) = 0;
    iftMatrixElem(S, 1, 3) = 0;
    iftMatrixElem(S, 2, 3) = 0;
    iftMatrixElem(S, 3, 3) = 1;


    Ainv = iftInvertMatrix(A);
    Cinv = iftInvertMatrix(C);
    Sinv = iftInvertMatrix(S);
}

iftVoxel Orientation::mapPixelToVolume(iftVoxel u, int slice, char plane)
{
    iftVoxel v = {0,0,1,0},t;
    iftVoxel w;
    iftMatrix *R = nullptr;
    switch (plane){
        case AXIAL:
            R = iftCopyMatrix(Ainv);
            v = iftVoxel(iftVectorSub(u,offset[plane-1]));
            t = iftTransformVoxel(R, v);
            w.x = t.x;
            w.y = t.y;
            w.z = slice;
            break;
        case CORONAL:
            R = iftCopyMatrix(Cinv);
            v = iftVoxel(iftVectorSub(u,offset[plane-1]));
            t = iftTransformVoxel(R, v);
            w.x = t.y; //t.y + 1;
            w.y = slice;
            w.z = t.x; //t.x - 1;
           break;
        case SAGITTAL:
            R = iftCopyMatrix(Sinv);
            v = iftVoxel(iftVectorSub(u,offset[plane-1]));
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

iftImage* Orientation::applyOrientationOnSlice(iftImage *img, char plane)
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

    iftImageDomain new_dom = {0, 0, 1, 0};
    iftVoxel t = {img->xsize-1, img->ysize-1, img->zsize-1, 0};

    int sizes[3];
    sizes[0] = img->xsize;
    sizes[1] = img->ysize;
    sizes[2] = img->zsize;

    for (int j = 0; j <= 2; j++) {
            if ((iftMatrixElem(R, j, 0) == -1) || (iftMatrixElem(R, j, 0) == 1)) {
                new_dom.xsize = sizes[j];
                continue;
            }
            if ((iftMatrixElem(R, j, 1) == -1) || (iftMatrixElem(R, j, 1) == 1)) {
                new_dom.ysize = sizes[j];
                continue;
            }
        }


    iftImage *out_img = NULL;
    if (iftIsColorImage(img))
        out_img = iftCreateColorImage(new_dom.xsize, new_dom.ysize, new_dom.zsize, 0);
    else
        out_img = iftCreateImage(new_dom.xsize, new_dom.ysize, new_dom.zsize);
    iftCopyVoxelSize(img,out_img);

    offset[plane-1] = iftTransformVoxel(R, t);
    //TODO offset[plane-1].x = 1; ---> coronal slice problem
    offset[plane-1].x = (offset[plane-1].x < 0) ? abs(offset[plane-1].x): 0;
    offset[plane-1].y = (offset[plane-1].y < 0) ? abs(offset[plane-1].y): 0;
    offset[plane-1].z = 0;


    #pragma omp parallel for
    for (int p = 0; p < img->n; p++) {
        iftVoxel u = iftGetVoxelCoord(img, p);
        iftVoxel v = iftTransformVoxel(R, u);
        v = iftVoxel (iftVectorSum(v, offset[plane-1]));
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

