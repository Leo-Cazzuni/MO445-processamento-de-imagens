#include "ift.h"
#include "suputils.h"

iftImage *iftExtractSIBSuperpixels(iftImage *img, int nsuperpixels, float alpha, float beta, int niters, int *nseeds, int *finalniters) {
  iftImage  *mask1, *seeds, *label;
  iftMImage *mimg;
  iftAdjRel *A;
  iftIGraph *igraph;
  timer     *t1=NULL,*t2=NULL;

  /* Compute IFTSLIC superpixels */

  if (iftIs3DImage(img)){
    A      = iftSpheric(sqrtf(3.0));
  } else {
    //A      = iftCircular(sqrtf(2.0));
    A      = iftCircular(1.0);
  }

  if (iftIsColorImage(img)){
    mimg   = iftImageToMImage(img,LABNorm_CSPACE);
  } else {
    mimg   = iftImageToMImage(img,GRAY_CSPACE);
  }

  t1 = iftTic();

  /* entire image domain */

  mask1  = iftSelectImageDomain(mimg->xsize,mimg->ysize,mimg->zsize);

  /* minima of a basins manifold in that domain */
  igraph = iftImplicitIGraph(mimg,mask1,A);
  //iftIGraphBasins(igraph);

  /* seeds for ift slic */
  seeds  = iftGridSampling(mimg,mask1,nsuperpixels);
  //seeds  = iftAltMixedSampling(mimg,mask1,nsuperpixels);
  *nseeds = iftNumberOfElements(seeds);

  //seeds  = iftSelectNonBorderVoxels(mimg, mask1, atoi(argv[2]));
  //seeds  = iftSelectNSamplesFromMask(mimg,mask2,atoi(argv[2]));
  
  iftDestroyImage(&mask1);
  iftDestroyMImage(&mimg);
  
  *finalniters = iftIGraphISF_Mean(igraph,seeds,alpha,beta,niters);

  //iftImage *aux    = iftIGraphLabel(igraph);
  label   = iftIGraphLabel(igraph);
  //label    = iftSmoothRegionsByDiffusion(aux,img,0.5,5);

  /*
  // Show seeds
  iftImage *showSeeds = iftCreateImage(seeds->xsize, seeds->ysize, seeds->zsize);
  for (int p=0; p< showSeeds->n; p++) {
    if(seeds->val[p] >0)
      showSeeds->val[p] = 255;
  }
  iftWriteImageP2(showSeeds,"sib_seeds.pgm");
  iftDestroyImage(&showSeeds);
  */

  //iftDestroyImage(&aux);
  iftDestroyImage(&seeds);
  //iftDestroyImage(&basins);
  iftDestroyIGraph(&igraph);
  iftDestroyAdjRel(&A);

  t2 = iftToc();

  printf("SIB ok in %f ms\n",iftCompTime(t1,t2));

  return label;
}

int main(int argc, char *argv[])
{
  iftImage  *img, *label; //*mask2
  iftImage  *gt_borders = NULL, *gt_regions = NULL, *border = NULL;
  iftAdjRel *A;
  iftColor     RGB, YCbCr;
  
  int nautoiters, nseeds;
  char labelfilename[256];
  char borderfilename[256];

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc<7 || argc>9)
    iftError("Usage: iftSIB <image.[pgm,ppm,scn]> <nsamples> <alpha (e.g., [0.005-0.2])> <beta (e.g., 12)> <niters (e.g., 10)> <output_label> <ground-truth border image (OPTIONAL)> <ground-truth region image (OPTIONAL)>","main");

  img   = iftReadImageByExt(argv[1]);

  if (iftIs3DImage(img)){
    if (argc == 9) {
      gt_borders = iftReadImageByExt(argv[7]);
      gt_regions = iftReadImageByExt(argv[8]);
    }
  } else {
    if (argc == 9){
      gt_borders = iftReadImageByExt(argv[7]);
      gt_regions = iftReadImageByExt(argv[8]);
    }
  }


  label = iftExtractSIBSuperpixels(img, atoi(argv[2]), atof(argv[3]), atof(argv[4]), atoi(argv[5]), &(nseeds), &(nautoiters));
  

  A = iftCircular(0.0);
  border   = iftBorderImage(label);
  //printf("ok\n");
  // Write output image
  if (!iftIs3DImage(img)){
    if (argc == 9){
      RGB.val[0] = 255;
      RGB.val[1] = 0;
      RGB.val[2] = 255;
      YCbCr      = iftRGBtoYCbCr(RGB, 255);
      iftDrawBorders(img,gt_borders,A,YCbCr,A);
    }
    RGB.val[0] = 0;
    RGB.val[1] = 255;
    RGB.val[2] = 255;
    YCbCr      = iftRGBtoYCbCr(RGB, 255);
    sprintf(labelfilename, "%s.pgm", argv[6]);
    //printf("ok write image %s\n", labelfilename);
    iftWriteImageP2(label,labelfilename);
    iftDrawBorders(img,border,A,YCbCr,A);
    sprintf(borderfilename, "%s.ppm", argv[6]);
    iftWriteImageP6(img, borderfilename);
  } else {
    sprintf(labelfilename, "%s.scn", argv[6]);
    iftWriteImage(label,labelfilename);
    RGB.val[0] = iftMaximumValue(img);
    RGB.val[1] = RGB.val[0];
    RGB.val[2] = RGB.val[0];
    YCbCr      = iftRGBtoYCbCr(RGB, RGB.val[0]);
    iftDrawBorders(img,border,A,YCbCr,A);
    sprintf(borderfilename, "%s-border.scn", argv[6]);
    iftWriteImage(img,borderfilename);
  }

  // Print number of iterations and superpixels
  printf("Number of iterations: %d \n", nautoiters);
  printf("Number of superpixels: %d \n", nseeds);

  // Compute metrics
  if (argc==9){
    float br, ue, comp, topology;
    br       = iftBoundaryRecall(gt_borders, border, 2.0);
    ue       = iftUnderSegmentation(gt_regions, label, 0.05);
    comp     = iftCompactness2D(label);
    topology = iftTopologyMeasure(label);
    printf("BR: %f \n", br);
    printf("UE: %f \n", ue);
    printf("Comp: %f \n", comp);
    printf("Top: %f \n", topology);
    iftDestroyImage(&gt_borders);
    iftDestroyImage(&gt_regions);
    iftDestroyImage(&border);
  }


  // Free
  iftDestroyImage(&img);
  iftDestroyImage(&label);
  iftDestroyAdjRel(&A);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}
