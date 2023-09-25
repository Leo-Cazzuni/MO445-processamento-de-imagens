#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftImage  *img, *mask1, *mask2;
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
    iftError("Usage: iftOPFCluster <image.[pgm,ppm,scn]> <nsamples (e.g., 5000)> <k-adjacency (e.g., 100)> <height (e.g., 100 >","main");

  img  = iftReadImageByExt(argv[1]);

  if (iftIs3DImage(img)){
    A      = iftSpheric(sqrtf(3.0));
  } else {
    A      = iftCircular(sqrtf(2.0));
  }
   
  if (iftIsColorImage(img)){
    mimg   = iftImageToMImage(img,LABNorm_CSPACE);
  } else {
    mimg   = iftImageToMImage(img,GRAY_CSPACE);
  }

  mask1  = iftSelectImageDomain(mimg->xsize,mimg->ysize,mimg->zsize);
  mask2  = iftSelectNonBorderVoxels(mimg, mask1, atoi(argv[2]));

  igraph = iftKnnIGraph(mimg,mask2,atoi(argv[3]));
  printf("Graph created with %d nodes and %d adjacents per node\n",igraph->nnodes,atoi(argv[3]));
  iftIGraphDomes(igraph);

  iftIGraphDualWaterGray(igraph, HEIGHT, atoi(argv[4]));

  iftIGraphClusterVoxelsByMaxArcWeight(igraph, IFT_DECREASING);

  iftImage *pvalue    = iftIGraphPathValue(igraph);
  iftImage *label     = iftIGraphLabel(igraph);
  iftImage *aux       = iftSmoothRegionsByDiffusion(label,img,0.5,10);
  iftDestroyImage(&label);
  label               = iftSelectAndPropagateRegionsAboveArea(aux,100); 
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
  iftDestroyImage(&mask1);
  iftDestroyImage(&mask2);
  iftDestroyMImage(&mimg);
  iftDestroyAdjRel(&A);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}

