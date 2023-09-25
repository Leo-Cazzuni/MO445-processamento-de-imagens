#include "ift.h"

iftImage *iftGetMeanPositionCenters(iftImage *label) {
  int num_initial_regions, ndim, index_label;
  float dist;
  iftVoxel u;
  iftFloatArray *featp, *featq, *best_dist;
  iftMatrix *feats;
  iftIntArray *num_pixels, *centers;
  iftImage *new_label;
  ndim = 3;
  num_initial_regions = iftMaximumValue(label);
  printf("num_initial_regions %d\n", num_initial_regions);
  // compute mean location
  feats = iftCreateMatrix(ndim, num_initial_regions);
  num_pixels = iftCreateIntArray(num_initial_regions);
  for (int p = 0; p < label->n; ++p) {
    u = iftGetVoxelCoord(label, p);
    index_label = label->val[p] - 1;
    iftMatrixElem(feats, 0, index_label) += u.x;
    iftMatrixElem(feats, 1, index_label) += u.y;
    iftMatrixElem(feats, 2, index_label) += u.z;
    num_pixels->val[index_label]++;
  }
  best_dist = iftCreateFloatArray(num_initial_regions);
  for (int i = 0; i < num_initial_regions; ++i) {
    iftMatrixElem(feats, 0, i) /= num_pixels->val[i];
    iftMatrixElem(feats, 1, i) /= num_pixels->val[i];
    iftMatrixElem(feats, 2, i) /= num_pixels->val[i];
    best_dist->val[i] = IFT_INFINITY_FLT;
  }
  // obtain centers, closest pixels to the mean location
  centers = iftCreateIntArray(num_initial_regions);
  featp = iftCreateFloatArray(ndim);
  featq = iftCreateFloatArray(ndim);
  for (int p = 0; p < label->n; ++p) {
    index_label = label->val[p] - 1;
    u = iftGetVoxelCoord(label, p);
    featp->val[0] = u.x;
    featp->val[1] = u.y;
    featp->val[2] = u.z;

    featq->val[0] = iftMatrixElem(feats, 0, index_label);
    featq->val[1] = iftMatrixElem(feats, 1, index_label);
    featq->val[2] = iftMatrixElem(feats, 2, index_label);
    dist = iftFeatDistance(featp->val, featq->val, ndim);
    //printf("dist %f cmp %f\n", dist, best_dist->val[index_label]);
    if (dist < best_dist->val[index_label]) {
      best_dist->val[index_label] = dist;
      centers->val[index_label] = p;
    }
  }
  // set centers in new_label image
  new_label = iftCreateImage(label->xsize, label->ysize, label->zsize);
  for (int i = 0; i < num_initial_regions; ++i) {
    //printf("centers %d\n", centers->val[i]);
    new_label->val[centers->val[i]] = 1;
  }

  iftDestroyFloatArray(&best_dist);
  iftDestroyFloatArray(&featp);
  iftDestroyFloatArray(&featq);
  iftDestroyIntArray(&centers);
  iftDestroyIntArray(&num_pixels);
  iftDestroyMatrix(&feats);
  return new_label;
}

iftImage *iftExtract_SLIC_ISF_MEAN_Superpixels(iftImage *img, iftImage *initial_label, int nsuperpixels, float alpha, float beta, int niters, int smooth_niters, int *nseeds, int *finalniters) {
  iftImage  *mask1, *seeds, *label;
  iftMImage *mimg;
  iftAdjRel *A;
  iftIGraph *igraph;
  timer     *t1=NULL,*t2=NULL;

  /* Compute ISF superpixels */
  if (iftIs3DImage(img)){
    A      = iftSpheric(1.0);
  } else {
    A      = iftCircular(1.0);
  }

  if (iftIsColorImage(img)){
    mimg   = iftImageToMImage(img,LABNorm_CSPACE);
  } else {
    mimg   = iftImageToMImage(img,GRAY_CSPACE);
  }

  mask1  = iftSelectImageDomain(mimg->xsize,mimg->ysize,mimg->zsize);

  /* minima of a basins manifold in that domain */
  igraph = iftImplicitIGraph(mimg,mask1,A);
  
  t1 = iftTic();
  /* seed sampling for ISF */
  //seeds   = iftGridSampling(mimg,mask1,nsuperpixels);
  //seeds   = iftAltMixedSampling(mimg,mask1,nsuperpixels);
  seeds   = iftGetMeanPositionCenters(initial_label);

  *nseeds = iftNumberOfElements(seeds);

  iftDestroyImage(&mask1);
  iftDestroyMImage(&mimg);

  *finalniters = iftIGraphISF_Mean(igraph,seeds,alpha,beta,niters);
  //*finalniters = iftIGraphISF_Root(igraph,seeds,alpha,beta,niters);

  /* Smooth regions in the label map of igraph */  
  if (smooth_niters > 0){
    iftIGraphSetWeightForRegionSmoothing(igraph, img);
    iftIGraphSmoothRegions(igraph, smooth_niters);
  }
  label   = iftIGraphLabel(igraph);
  t2 = iftToc();
  printf("ISF proc time im ms: %f\n", iftCompTime(t1,t2));

  iftDestroyImage(&seeds);
  iftDestroyIGraph(&igraph);
  iftDestroyAdjRel(&A);
  

  return label;
}

int main(int argc, char *argv[]) 
{
  iftImage  *img, *label, *initial_label;
  iftImage  *gt_borders=NULL, *gt_regions=NULL, *border=NULL;
  iftAdjRel *A;
  int        nautoiters, nseeds;
  char       labelfilename[256];
  char       borderfilename[256];
  iftColor   RGB, YCbCr;
  int        normvalue;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc<9 || argc>11 )
    iftError("Usage: iftISF <image.[pgm,ppm,scn,png]> <slic_label.pgm> <nsamples> <alpha (e.g., [0.005-0.2])> <beta (e.g., 12)> <niters (e.g., 10)> <smooth niters (e.g., 2)> <output_label> <ground-truth border image (OPTIONAL)> <ground-truth region image (OPTIONAL)>","main");

  img  = iftReadImageByExt(argv[1]);
  initial_label  = iftReadImageByExt(argv[2]);

  if (argc >= 10){
    gt_borders = iftReadImageByExt(argv[9]);
    gt_regions = iftReadImageByExt(argv[10]);
  }
  
  nautoiters = 1;
  normvalue =  iftNormalizationValue(iftMaximumValue(img)); 

  label     = iftExtract_SLIC_ISF_MEAN_Superpixels(img, initial_label, atoi(argv[3]), atof(argv[4]), atof(argv[5]), atoi(argv[6]), atoi(argv[7]), &(nseeds), &(nautoiters));
  //label   = iftCopyImage(initial_label);
  
  border  = iftBorderImage(label,0);
  
  A = iftCircular(0.0);
  
  nseeds = iftMaximumValue(label);
  
  // Compute metrics
  if (argc>=10){
    float br, ue, comp, topology;
    br        = iftBoundaryRecall(gt_borders, border, 2.0);
    printf("BR: %f \n", br);
    
    ue       = iftUnderSegmentation(gt_regions, label);
    printf("UE: %f \n", ue);

    comp     = iftCompactness2D(label);
    printf("Comp: %f \n", comp);

    topology = iftTopologyMeasure(label);
    printf("Top: %f \n", topology);
  }
  
  // Write output image
  if (!iftIs3DImage(img)){
    if (argc >= 10){
      RGB.val[0] = normvalue;
      RGB.val[1] = 0;
      RGB.val[2] = normvalue;
      YCbCr      = iftRGBtoYCbCr(RGB, normvalue);
      iftDrawBorders(img,gt_borders,A,YCbCr,A);
    }

    RGB.val[0] = normvalue;
    RGB.val[1] = normvalue/3.0;
    RGB.val[2] = 0;
    
    YCbCr      = iftRGBtoYCbCr(RGB, normvalue);
    sprintf(labelfilename, "%s.pgm", argv[8]);
    iftWriteImageP2(label,labelfilename);
    iftDrawBorders(img,border,A,YCbCr,A);
    sprintf(borderfilename, "%s.png", argv[8]);        
    iftWriteImageByExt(img, borderfilename);
  } else {
    printf("%d\n",iftMaximumValue(label));
    sprintf(labelfilename, "%s.scn", argv[8]);
    iftWriteImage(label,labelfilename);
    RGB.val[2] = RGB.val[1] = RGB.val[0] = iftMaximumValue(img);
    YCbCr      = iftRGBtoYCbCr(RGB, RGB.val[0]);
    iftLabeledSet* Centers = iftGeodesicCenters(label);
    iftDestroyAdjRel(&A);
    A = iftSpheric(1.0);
    while (Centers != NULL) {
      int trash;
      int p = iftRemoveLabeledSet(&Centers,&trash);
      iftVoxel u = iftGetVoxelCoord(img,p);
      iftDrawPoint(img, u, YCbCr, A);
    }
    sprintf(borderfilename, "%s-centers.scn", argv[8]);
    iftWriteImage(img,borderfilename);
  }

  // Print number of iterations and superpixels
  printf("Number of iterations = %d \n", nautoiters);
  printf("Number of superpixels = %d \n", nseeds);
  
  
  // Free
  iftDestroyImage(&img);
  if (argc>=10) {
    iftDestroyImage(&gt_borders);
    iftDestroyImage(&gt_regions);
  }

  iftDestroyImage(&initial_label);
  iftDestroyImage(&label);
  iftDestroyImage(&border);
  iftDestroyAdjRel(&A);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
     MemDinInicial,MemDinFinal);   

  return(0);
}
