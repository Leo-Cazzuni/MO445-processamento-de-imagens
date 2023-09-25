#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftImage  *img, *mask;
  iftMImage *mimg;
  iftAdjRel *A;
  iftIGraph *igraph;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc!=5)
    iftError("Usage: iftOPFSCluster <image.[pgm,ppm,scn]> <adj. radius (e.g., 5)> <kmax (e.g., 100)> <height (e.g. 100)>","main");

  img  = iftReadImageByExt(argv[1]);

  if (iftIs3DImage(img)){
    A      = iftSpheric(sqrtf(atof(argv[2])));
  } else {
    A      = iftCircular(sqrtf(atof(argv[2])));
  }
   
  if (iftIsColorImage(img)){
    mimg   = iftImageToMImage(img,LABNorm_CSPACE);
  } else {
    mimg   = iftImageToMImage(img,GRAY_CSPACE);
  }
  
  mask   = iftSelectImageDomain(mimg->xsize,mimg->ysize,mimg->zsize);

  igraph = iftSpatialKnnIGraph(mimg,mask,A,atoi(argv[3]));
  iftDestroyImage(&mask);
  iftIGraphDomes(igraph);
  iftIGraphDualWaterGray(igraph, HEIGHT, atoi(argv[4]));

  iftImage *pvalue = iftIGraphPathValue(igraph);
  iftImage *aux    = iftIGraphLabel(igraph);
  iftImage *label  = iftSmoothRegionsByDiffusion(aux,img,0.5,5);
  iftDestroyImage(&aux);

  if (!iftIs3DImage(img)){
    iftWriteImageP2(pvalue,"pvalue.pgm");
    iftDestroyAdjRel(&A);
    A = iftCircular(sqrtf(2.0));
    iftColor  RGB, YCbCr;
    iftAdjRel *B = iftCircular(0.0);    
    RGB.val[0] = 0;
    RGB.val[1] = 255;
    RGB.val[2] = 255;
    YCbCr      = iftRGBtoYCbCr(RGB,255);
    iftWriteImageP2(label,"label.pgm");
    iftDrawBorders(img,label,A,YCbCr,B);
    iftWriteImageP6(img,"regions.ppm");
    iftDestroyAdjRel(&B);
  } else {
    iftWriteImage(pvalue,"pvalue.scn");
    iftWriteImage(label,"label.scn");
  }

  iftDestroyIGraph(&igraph);
  iftDestroyImage(&img);
  iftDestroyImage(&pvalue);
  iftDestroyImage(&label);
  iftDestroyMImage(&mimg);
  iftDestroyAdjRel(&A);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDynamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}

