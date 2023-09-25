#include "ift.h"

int main(int argc, const char *argv[]) {
    if (argc != 7)
        iftError("iftCreateCuboid <xsize> <ysize> <zsize> <perc> <value> <cuboid.[scn,pgm,png]>", "main");

    int xsize           = atoi(argv[1]);
    int ysize           = atoi(argv[2]);
    int zsize           = atoi(argv[3]);
    float perc          = atof(argv[4]);
    int val             = atoi(argv[5]);
    const char *out_img = argv[6];

    iftImage *img = iftCreateCuboid(xsize, ysize, zsize, perc, val);
    
    /* make a hole */

    /* for (int p=0; p < img->n; p++) { */
    /*   iftVoxel u = iftGetVoxelCoord(img,p); */
    /*   if ((abs(u.x - xsize/2)<xsize/8)&&(abs(u.y - ysize/2)<ysize/8)&&(abs(u.z - zsize/2)<zsize/8)) */
    /* 	img->val[p]=0; */
    /* } */
    
    iftWriteImageByExt(img, out_img);
    
    puts("Done...");

    iftDestroyImage(&img);
}
