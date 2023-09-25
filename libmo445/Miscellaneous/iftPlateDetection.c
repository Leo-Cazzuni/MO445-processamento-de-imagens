#include "ift.h"

iftImage *iftPlateScore(iftImage *orig, iftImage *grad, iftAdjRel *A, iftVoxel *pos)
{
  iftImage *score=iftCreateImage(orig->xsize,orig->ysize,orig->zsize); 
  int p,q,i;
  iftVoxel u,v; 
  float val,max_score=0.0; 
  
  pos->x=pos->y=pos->z=0;
  iftMaximumValue(grad); 
  iftMaximumValue(orig); 

  for (p=0; p < orig->n; p++) {
    u   = iftGetVoxelCoord(orig,p);
    val = 0.0; 
    for (i=0; i < A->n; i++) {
      v = iftGetAdjacentVoxel(A,u,i);
      if (iftValidVoxel(orig,v)) {
	q = iftGetVoxelIndex(orig,v); 
	val += ((float)grad->val[q]/(float)grad->maxval)*(1.0-(float)orig->val[q]/(float)orig->maxval); 
      }
    }
    score->val[p] = 255.0*(val/A->n); 
    if (score->val[p] > max_score) {
      max_score = score->val[p]; 
      pos->x = u.x; 
      pos->y = u.y; 
      pos->z = u.z; 
    }
  }

  return(score);
}

int main(int argc, char *argv[]) 
{
  iftImage  *img[3]; 
  iftKernel *K;
  iftAdjRel *A;
  iftVoxel   pos; 
  iftColor   RGB, YCbCr; 

  if (argc!=4)
    iftError("Usage: iftPlateDetection <plate.pgm> <xsize> <ysize>","main");

  
  img[0] = iftReadImageP2(argv[1]); 
  K      = iftSobelXKernel2D(); 
  img[1] = iftLinearFilter(img[0],K);
  img[2] = iftAbs(img[1]); 
  iftDestroyKernel(&K);
  iftDestroyImage(&img[1]);
  A      = iftRectangular(atoi(argv[2]),atoi(argv[3]));
  img[1] = iftPlateScore(img[0],img[2],A,&pos);
  iftDestroyAdjRel(&A);
  iftWriteImageP2(img[1],"score.pgm");

  RGB.val[0] = 255;
  RGB.val[1] = 0;
  RGB.val[2] = 0;
  YCbCr      = iftRGBtoYCbCr(RGB);
  A          = iftCircular(3.0);
  iftDrawPoint(img[0],pos,YCbCr,A);
  iftWriteImageP6(img[0],"detection.ppm");


  iftDestroyImage(&img[0]); 
  iftDestroyImage(&img[1]); 
  iftDestroyImage(&img[2]); 

  return(0);
}
