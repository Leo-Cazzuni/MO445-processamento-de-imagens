#include "ift.h"

/* Get slice at Po = (xo,yo,zo,1) and given viewing direction v =
   (vx,vy,vz). */

int main(int argc, const char *argv[]) {

    if (argc != 9)
        iftError("iftGetSlice <input.scn> <xo> <yo> <zo> <vx> <vy> <vz> <output.pgm>", "main");


    iftImage *img = iftReadImageByExt(argv[1]);


    iftPoint  Po; 
    
    Po.x = atof(argv[2]); Po.y = atof(argv[3]); Po.z = atof(argv[4]);

    iftVector v; 
    
    v.x = atof(argv[5]); v.y = atof(argv[6]); v.z = atof(argv[7]);

    iftPlane *cutplane = iftCreatePlane();
    iftSetPlaneOrient(cutplane,v.x,v.y,v.z);
    
    printf("normal of the cut plane: %f %f %f\n",cutplane->normal.x,cutplane->normal.y,cutplane->normal.z);
    cutplane->pos      = Po; 
    printf("position of the cut plane: %f %f %f\n",Po.x,Po.y,Po.z);
    int diagonal       = iftDiagonalSize(img);
    iftImage *slice    = iftGetSlice(img,cutplane,diagonal,diagonal); 
    iftWriteImageByExt(slice,argv[8]);
    iftDestroyPlane(&cutplane);
    iftDestroyImage(&slice);
    puts("Done...");
    iftDestroyImage(&img);

    return(0);
}
