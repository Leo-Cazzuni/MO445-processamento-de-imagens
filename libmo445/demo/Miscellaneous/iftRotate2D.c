#include "ift.h"


int main(int argc, const char *argv[]) {

  if (argc != 4){
    iftError("iftRotate2D image.[ppm,png,pgm] <theta [0-180]> result.[ppm,png,pgm]","main");
  }
 
  iftImage *img      = iftReadImageByExt(argv[1]);
  iftImage *res      = iftRotateImage2D(img,atof(argv[2]));


  iftWriteImageByExt(res,argv[3]);
    
  iftDestroyImage(&img);
  iftDestroyImage(&res);
  
  return 0;
}




