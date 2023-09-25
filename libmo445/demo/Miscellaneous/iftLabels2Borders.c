#include "ift.h"

void iftDrawBordersExcludingBackground(iftImage *img, iftImage *label, iftAdjRel *A, iftColor YCbCr, iftAdjRel *B, bool exclude_bkg, bool one_pixel_wide)
{
    iftVoxel u,v;
    int i,p,q;

    if ((img->xsize != label->xsize)||
        (img->ysize != label->ysize)||
        (img->zsize != label->zsize))
        iftError("Images must have the same domain","iftDrawBorders");


    if (!iftIsColorImage(img))
        iftSetCbCr(img,128);

    if (A->n > 1){
        for (p=0; p < img->n; p++) {
            u = iftGetVoxelCoord(label,p);
            for (i=0; i < A->n; i++) {
                v = iftGetAdjacentVoxel(A,u,i);
                if (iftValidVoxel(label,v)){
                    q = iftGetVoxelIndex(label,v);
                    if(one_pixel_wide) {
                        if ((exclude_bkg && label->val[p] > 0 && label->val[q] > 0 && label->val[p] > label->val[q])
                            || (!exclude_bkg && label->val[p] > label->val[q])) {
                            iftDrawPoint(img, u, YCbCr, B);
                            break;
                        }
                    } else {
                        if ((exclude_bkg && label->val[p] > 0 && label->val[q] > 0 && label->val[p] != label->val[q])
                            || (!exclude_bkg && label->val[p] != label->val[q])) {
                            iftDrawPoint(img, u, YCbCr, B);
                            break;
                        }
                    }
                }
            }
        }
    } else {
        for (p=0; p < img->n; p++)
            if (label->val[p] != 0){
                u = iftGetVoxelCoord(label,p);
                iftDrawPoint(img,u,YCbCr,B);
            }
    }
}
int main(int argc, char **argv) {
    if (argc != 7)
        iftError("Please provide the following parameters:\n<input.pgm/.scn> <output.pgm/.scn> <border value> <border thickness> <exclude background {0,1}> <\"1-pixel wide\" border {0,1}>\n\n", "main");

    char *filenameIn  = argv[1];
    char *filenameOut = argv[2];

    iftImage* label = iftReadImageByExt(filenameIn);
    iftAdjRel *A = NULL, *B = NULL;
    iftColor color;

    if(iftIs3DImage(label)) {
        A = iftSpheric(1.0);
        B = iftSpheric(iftMax(0.0, atof(argv[4])));
    } else {
        A = iftCircular(1.5);
        B = iftCircular(iftMax(0.0, atof(argv[4])));
    }

    iftImage *imgout = iftCreateImage(label->xsize, label->ysize, label->zsize);

    color.val[0] = atoi(argv[3]);
    color.val[1] = 0;
    color.val[2] = 0;

    iftDrawBordersExcludingBackground(imgout, label, A, color, B, atoi(argv[5]), atoi(argv[6]));

//  iftImage* imgout = iftObject2Border(label,A);

    iftWriteImageByExt(imgout,filenameOut);

    iftDestroyAdjRel(&A);
    iftDestroyAdjRel(&B);
    iftDestroyImage(&label);
    iftDestroyImage(&imgout);

    return 0;
}
