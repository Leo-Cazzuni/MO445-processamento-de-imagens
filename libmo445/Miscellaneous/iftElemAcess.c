//
// Created by peixinho on 6/6/16.
//

#include "ift.h"

int main() {

    iftImage* img2d = iftCreateImage(5,5,1);
    //set the central pixel in image.
    iftImgVal2D(img2d, 2, 2) = 255;

    iftImage* img3d = iftCreateColorImage(5, 5, 5);

    //set the central voxel in 3D image
    iftImgVal(img3d, 2, 2, 2) = 1;

    //we can also set a voxel passing voxel struct
    iftVoxel v;
    v.x = v.y = v.z = 2;
    iftImgVoxelVal(img3d, v) = 1;

    //set Cb component passing voxel struct
    iftImgVoxelCb(img3d, v) = 127;

    iftMatrix* matrix = iftIdentityMatrix(5);

    //multiply the elements in main diagonal by scalar and update the matrix
    for (int i = 0; i < matrix->nrows; ++i) {
        iftMatrixElem(matrix, i, i) *= 10;
    }

    double sum = 0.0;
    //computes the elements sum of elements in first row.if
    for (int c = 0; c < matrix->ncols; ++c) {
        sum += iftMatrixElem(matrix, c, 0);
    }

    printf("Funcionou, confia?\n");

    iftDestroyImage(&img2d);
    iftDestroyImage(&img3d);
    iftDestroyMatrix(&matrix);

}
