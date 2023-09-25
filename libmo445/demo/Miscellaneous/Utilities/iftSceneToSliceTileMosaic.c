#include "ift.h"

iftImage *iftRelabelUniqueRegions(iftImage *label) {
    int max_lb;
    int *new_labels = NULL;
    iftImage *relabeled = NULL;

    max_lb = iftMaximumValue(label);

    relabeled = iftCreateImage(label->xsize, label->ysize, label->zsize);

    new_labels = iftAllocIntArray(max_lb + 1);

    for (int p = 0; p < label->n; p++) {
        if (label->val[p] > 0)
            new_labels[label->val[p]] = 1;
    }

    int new_lb = 1;
    for (int lb = 1; lb <= max_lb; lb++) {
        if (new_labels[lb] > 0)
            new_labels[lb] = new_lb++;
    }

    for (int p = 0; p < label->n; p++) {
        if (label->val[p] > 0)
            relabeled->val[p] = new_labels[label->val[p]];
    }

    iftCopyVoxelSize(label, relabeled);

    return relabeled;
}

int main(int argc, char *argv[])
{
    int xslices = 0;
    iftImage  *img = NULL, *mosaic = NULL, *tmp = NULL;
    timer     *t1=NULL,*t2=NULL;

    /*--------------------------------------------------------*/

    int MemDinInicial, MemDinFinal;
    MemDinInicial = iftMemoryUsed();

    /*--------------------------------------------------------*/

    if (argc!=5)
        iftError(
                "Usage: %s <image original 2D/3D image name> <number of slices per row> <output image name(.png,.pgm)> <relabel regions {0,1}>",
                "main", argv[0]);

    img = iftReadImageByExt(argv[1]);

    t1     = iftTic();

    xslices = iftMax(1, iftMin(atoi(argv[2]), img->zsize));
    mosaic  = iftCreateImage(img->xsize*xslices, img->ysize*((int)ceil(img->zsize/(float)xslices)), 1);

    if(atoi(argv[4]) != 0) {
        iftImage *relabeled = iftRelabelUniqueRegions(img);
        iftDestroyImage(&img);
        img = relabeled;
    }

    // Rescaling to 0-255
    tmp = iftLinearStretch(img, iftMinimumValue(img), iftMaximumValue(img), 0, 255);
    iftDestroyImage(&img);
    img = tmp;

    for(int z = 0; z < img->zsize; z++) {
        iftVoxel orig, target;
        iftImage *slice = iftGetXYSlice(img, z);

        orig.x      = orig.y = orig.z = 0;
        target.x    = img->xsize*(z % xslices);
        target.y    = img->ysize*(z / xslices);
        target.z    = 0;

        iftInsertROIByPosition(slice, orig, mosaic, target);

        iftDestroyImage(&slice);
    }

    t2     = iftToc();
    fprintf(stdout,"Mosaicing in %f ms\n",iftCompTime(t1,t2));

    iftWriteImagePNGWithAlpha(mosaic, mosaic, argv[3]);

    iftDestroyImage(&img);
    iftDestroyImage(&mosaic);

    /* ---------------------------------------------------------- */

    MemDinFinal = iftMemoryUsed();
    if (MemDinInicial!=MemDinFinal)
        printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
               MemDinInicial,MemDinFinal);

    return(0);

}

