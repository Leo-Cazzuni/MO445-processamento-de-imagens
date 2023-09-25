#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftImage  *img, *mask1, *mask2;
  iftMImage *mimg;
  iftAdjRel *A;
  iftIGraph *igraph;
  iftDataSet *Z; 
  iftKnnGraph *graph;
  float df;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc!=6)
    iftError("Usage: iftOPFSClusterBestK <image.[pgm,ppm,scn]> <adj. radius> <kmax> <nsamples for best K> <spacing for best K>","main");

  img  = iftReadImageByExt(argv[1]);

  if (iftIs3DImage(img)){
    A      = iftSpheric(atof(argv[2]));
    mimg   = iftImageToMImage(img,GRAY_CSPACE);
  }else{
    A = iftCircular(atof(argv[2]));
    mimg   = iftImageToMImage(img,LAB_CSPACE);
  }
  
  mask1   = iftSelectImageDomain(mimg->xsize,mimg->ysize,mimg->zsize);

  mask2 = iftSelectNonBorderVoxels(mimg, mask1, atoi(argv[4]),atof(argv[5])); 
  
  Z = iftMImageToDataSetInRegion(mimg,mask2); 
  iftSetStatus(Z,IFT_TRAIN);
  graph = iftCreateKnnGraph(Z,atoi(argv[3]));
  iftBestkByKnnGraphCut(graph,iftNormalizedCut);
  df = graph->maxarcw[graph->k];


  /*
  iftVoxel uo, uf;
  uo.z = uf.z = 0;
  uo.x = 200; uo.y = 70;
  uf.x = 300; uf.y = 200;
  mask  = iftSelectRegionOfInterest(mimg->xsize,mimg->ysize,mimg->zsize,uo,uf);
  */
  
  igraph = iftSpatialIGraph(mimg,mask1,A,df);
  iftDestroyImage(&mask1);
  iftIGraphPDF(igraph);

  float *marker = iftIGraphWeightMarkerByHeight(igraph,100);
  iftIGraphInfRec(igraph,marker);
  iftIGraphAddWeight(igraph,-100);
  free(marker);

  iftImage *pdf    = iftIGraphPathValue(igraph);
  iftImage *aux    = iftIGraphLabel(igraph);
  iftImage *label  = iftSmoothRegionsByDiffusion(aux,img,0.5,5);
  iftDestroyImage(&aux);

  if (!iftIs3DImage(img)){
    iftWriteImageP2(pdf,"pdf.pgm");
    iftDestroyAdjRel(&A);
    A = iftCircular(sqrtf(2.0));
    iftColor  RGB, YCbCr;
    iftAdjRel *B = iftCircular(1.0);    
    RGB.val[0] = 0;
    RGB.val[1] = 255;
    RGB.val[2] = 255;
    YCbCr      = iftRGBtoYCbCr(RGB,255);
    iftWriteImageP2(label,"label.pgm");
    iftDrawBorders(img,label,A,YCbCr,B);
    iftWriteImageP6(img,"regions.ppm");
    iftDestroyAdjRel(&B);
  } else {
    iftWriteImage(pdf,"pdf.scn");
    iftWriteImage(label,"label.scn");
  }

  iftDestroyIGraph(&igraph);
  iftDestroyImage(&img);
  iftDestroyImage(&pdf);
  iftDestroyImage(&label);
  iftDestroyMImage(&mimg);
  iftDestroyAdjRel(&A);
  iftDestroyDataSet(&Z);
  iftDestroyImage(&mask2);
  iftDestroyKnnGraph(&graph);


  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}

