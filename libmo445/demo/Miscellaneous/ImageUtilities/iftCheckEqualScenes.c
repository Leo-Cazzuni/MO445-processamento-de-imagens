#include "ift.h"

#define EPSILON 0.00001 

int main(int argc, char **argv) {
    if (argc != 3)
        iftError("iftCheckEqualScenes <scene01.scn> <scene02.scn>", "main");
    
    char msg[200];

    char filename1[200]; strcpy(filename1, argv[1]);
    char filename2[200]; strcpy(filename2, argv[2]);

    printf("- Reading %s\n", filename1);
    iftImage *img1 = iftReadImage(argv[1]);
    printf("- Reading %s\n", filename2);
    iftImage *img2 = iftReadImage(argv[2]);

    if (img1->xsize - img2->xsize) {
        sprintf(msg, "Different xsize: %d - %d\n", img1->xsize, img2->xsize);
        iftError(msg, "main");
    }

    if (img1->ysize != img2->ysize) {
        sprintf(msg, "Different ysize: %d - %d\n", img1->ysize, img2->ysize);
        iftError(msg, "main");
    }

    if (img1->zsize != img2->zsize) {
        sprintf(msg, "Different zsize: %d - %d\n", img1->zsize, img2->zsize);
        iftError(msg, "main");
    }

    if (fabs(img1->dx - img2->dx) > EPSILON) {
        sprintf(msg, "Different dx: %f - %f\n", img1->dx, img2->dx);
        iftError(msg, "main");
    }

    if (fabs(img1->dy - img2->dy) > EPSILON) {
        sprintf(msg, "Different dy: %f - %f\n", img1->dy, img2->dy);
        iftError(msg, "main");
    }

    if (fabs(img1->dz - img2->dz) > EPSILON) {
        sprintf(msg, "Different dz: %f - %f\n", img1->dz, img2->dz);
        iftError(msg, "main");
    }

    for (int p = 0; p < img1->n; p++) {
        if (img1->val[p] != img2->val[p]) {
            sprintf(msg, "voxel[%d] is different: %d - %d", p, img1->val[p], img2->val[p]);
            iftError(msg, "main");
        }
    }

    iftDestroyImage(&img1);
    iftDestroyImage(&img2);

    printf("Done... Images are equal!\n");

    return 0;
}