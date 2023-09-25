#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftImage  *img, *mask, *resImg;
  iftMImage *mimg;
  iftAdjRel *A;
  iftIGraph *igraph;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDynInicial, MemDynFinal;
  free(trash); 
  info = mallinfo();
  MemDynInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc!=5)
    iftError("Usage: iftMinTreeOperations <image.[pgm,ppm,scn]> <out.[pgm,ppm,scn]> <criterion (Height-0, Area-1, Volume-2)> <threshold>","main");


  img  = iftReadImageByExt(argv[1]);
  
  if (iftIs3DImage(img)){
    A      = iftSpheric(1.0);
  } else {
    A      = iftCircular(1.0);
  }
   
  mimg   = iftImageToMImage(img,GRAY_CSPACE);
  

  //Create a graph from the whole image.

  mask   = iftSelectImageDomain(mimg->xsize,mimg->ysize,mimg->zsize);
  igraph = iftImplicitIGraph(mimg,mask,A);
  printf("Graph created with %d nodes and %d adjacents per node\n",igraph->nnodes,A->n);
  
  //Copy the image brightness (feature) to the weight of each node of the 
  //graph (necessary for the MaxTree creation).

  iftIGraphCopyFeatureToWeight(igraph, 0);

  // Compute Watershed transform from gray-scale marker

  iftIGraphWaterGray(igraph,atoi(argv[3]),atoi(argv[4]));

  resImg = iftIGraphPathValue(igraph);

  if (iftIs3DImage(resImg)){
      iftWriteImageByExt(resImg,argv[2]);
  } else {
    if (iftIsColorImage(resImg))
      iftWriteImageByExt(resImg,argv[2]);
    else
      iftWriteImageByExt(resImg,argv[2]);
  }

  iftDestroyIGraph(&igraph);
  iftDestroyImage(&img);
  iftDestroyImage(&resImg);
  iftDestroyImage(&mask);
  iftDestroyMImage(&mimg);
  iftDestroyAdjRel(&A);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDynFinal = info.uordblks;
  if (MemDynInicial!=MemDynFinal)
    printf("\n\nDynamic memory was not completely deallocated (%d, %d)\n",
	   MemDynInicial,MemDynFinal);   

  return(0);
}

