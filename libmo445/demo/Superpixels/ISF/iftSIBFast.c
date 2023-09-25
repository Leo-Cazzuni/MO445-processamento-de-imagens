#include "ift.h"
#include "suputils.h"

iftImage *iftInterp2DNN(const iftImage *img, float sx, float sy)
{
  iftImage  *ximg, *yimg;  
  int        xsize,ysize;
  
  if (img->zsize != 1)
    iftError("Image must be 2D","iftInterp2D");
  if ((sx <= 0.0)||(sy <= 0.0))
    iftError("Invalid scale factors","iftInterp2D");

  xsize= iftRound(fabs(sx * img->xsize));
  ysize= iftRound(fabs(sy * img->ysize));

  if (img->Cb != NULL) {
      

    if (sx != 1.0) {

      /* Interpolate along x */
      
      ximg = iftCreateImage(xsize,img->ysize,1);
      ximg->dx = img->dx/sx;
      ximg->dy = img->dy;
      ximg->dz = img->dz;      
      iftSetCbCr(ximg,128);
      
#pragma omp parallel for shared(ximg, img, sx)
      for (int y = 0;  y < ximg->ysize; y++){
  iftVoxel u,v,w;
  u.z=v.z=w.z=0; 
  u.y = w.y = v.y = y;
  for (v.x = 0; v.x < ximg->xsize; v.x++){  
    int q    = iftGetVoxelIndex(ximg,v); 
    u.x  = (int)(v.x/sx);
    float dx   = (v.x/sx) - u.x; 
    w.x  = ((u.x+1)==img->xsize)?u.x:u.x+1;
    int p    = iftGetVoxelIndex(img,u); 
    int r    = iftGetVoxelIndex(img,w); 
    /*
    ximg->val[q] = (int)(img->val[p]*(1.0-dx)+img->val[r]*dx);
    ximg->Cb[q]  = (int)(img->Cb[p]*(1.0-dx)+img->Cb[r]*dx);
    ximg->Cr[q]  = (int)(img->Cr[p]*(1.0-dx)+img->Cr[r]*dx);
    */
    if (dx > 0.5) {
      ximg->val[q] = img->val[r];
      ximg->Cb[q]  = img->Cb[r];
      ximg->Cr[q]  = img->Cr[r];
    } else {
      ximg->val[q] = img->val[p];
      ximg->Cb[q]  = img->val[p];
      ximg->Cr[q]  = img->val[p];
    }

  }
      }
    } else {
      ximg = iftCopyImage(img);
    }

    if ( sy != 1.0) {
 
      /* Interpolate along y */
      
      yimg = iftCreateImage(xsize,ysize,1);
      iftSetCbCr(yimg,128);
      yimg->dx = ximg->dx;
      yimg->dy = ximg->dy/sy;
      yimg->dz = ximg->dz;
      
#pragma omp parallel for shared(yimg, ximg, sy)
      for (int x = 0; x < yimg->xsize; x++){
  iftVoxel u, v, w;
  u.z=v.z=w.z=0; 
  u.x = w.x = v.x = x;
  for (v.y = 0; v.y < yimg->ysize; v.y++){  
    int q    = iftGetVoxelIndex(yimg,v); 
    u.y  = (int)(v.y/sy);
    float dy   = (v.y/sy) - u.y; 
    w.y  = ((u.y+1)==ximg->ysize)?u.y:u.y+1;
    int p    = iftGetVoxelIndex(ximg,u); 
    int r    = iftGetVoxelIndex(ximg,w); 
    /*
    yimg->val[q] = (int)(ximg->val[p]*(1.0-dy)+ximg->val[r]*dy);
    yimg->Cb[q]  = (int)(ximg->Cb[p]*(1.0-dy)+ximg->Cb[r]*dy);
    yimg->Cr[q]  = (int)(ximg->Cr[p]*(1.0-dy)+ximg->Cr[r]*dy);
    */
    if (dy > 0.5) {
      yimg->val[q] = ximg->val[r];
      yimg->Cb[q]  = ximg->Cb[r];
      yimg->Cr[q]  = ximg->Cr[r];
    } else {
      yimg->val[q] = ximg->val[p];
      yimg->Cb[q]  = ximg->Cb[p];
      yimg->Cr[q]  = ximg->Cr[p];
    }
  }
      }
    } else {
      yimg = iftCopyImage(ximg);
    }
    
  }else{ /* Gray-Scale Image */


    if (sx != 1.0) {

      /* Interpolate along x */
      
      ximg = iftCreateImage(xsize,img->ysize,1);
      ximg->dx = img->dx/sx;
      ximg->dy = img->dy;
      ximg->dz = img->dz;      
      
#pragma omp parallel for shared(ximg, img, sx)
      for (int y = 0; y < ximg->ysize; y++){
  iftVoxel u, v, w;
  u.z=v.z=w.z=0; 
  u.y = w.y = v.y = y;
  for (v.x = 0; v.x < ximg->xsize; v.x++){  
    int q    = iftGetVoxelIndex(ximg,v); 
    u.x  = (int)(v.x/sx);
    float dx   = (v.x/sx) - u.x; 
    w.x  = ((u.x+1)==img->xsize)?u.x:u.x+1;
    int p    = iftGetVoxelIndex(img,u); 
    int r    = iftGetVoxelIndex(img,w); 
    //ximg->val[q] = (int)(img->val[p]*(1.0-dx)+img->val[r]*dx);
    
    if (dx > 0.5)
      ximg->val[q] = img->val[r];
    else
      ximg->val[q] = img->val[p];
    
  }
      }
    } else {
      ximg = iftCopyImage(img);
    }
    
    if (sy != 1.0) {
      
      /* Interpolate along y */
      
      yimg = iftCreateImage(xsize,ysize,1);
      yimg->dx = ximg->dx;
      yimg->dy = ximg->dy/sy;
      yimg->dz = ximg->dz;

#pragma omp parallel for shared(yimg, ximg, sy)
      for (int x = 0; x < yimg->xsize; x++){
  iftVoxel u,v,w;
  u.z=v.z=w.z=0; 
  u.x = w.x = v.x = x;
  for (v.y = 0; v.y < yimg->ysize; v.y++){  
    int q    = iftGetVoxelIndex(yimg,v); 
    u.y  = (int)(v.y/sy);
    float dy   = (v.y/sy) - u.y; 
    w.y  = ((u.y+1)==ximg->ysize)?u.y:u.y+1;
    int p    = iftGetVoxelIndex(ximg,u); 
    int r    = iftGetVoxelIndex(ximg,w); 
    //yimg->val[q] = (int)(ximg->val[p]*(1.0-dy)+ximg->val[r]*dy);
    
    if (dy > 0.5)
      yimg->val[q] = ximg->val[r];
    else
      yimg->val[q] = ximg->val[p];
    
  }
      }
    } else { 
      yimg = iftCopyImage(ximg);
    }  
  }

  iftDestroyImage(&ximg);

  return(yimg);
}

iftImage* iftSmoothSuperpixels(iftImage *img, iftImage *label, int num_smooth_iterations)
{
  iftImage  *prev_label, *next_label, *grad;
  iftFImage *prev_weight, *next_weight, *norm_factor, *weight;
  float     *sum, max_membership;
  int        l, i, p, q, r, max_label, iter, max_prev_label;
  iftVoxel   u, v;  
  iftAdjRel *A, *B;
  iftSet    *prev_frontier = NULL, *next_frontier = NULL, *S = NULL;
  iftBMap   *inFrontier;

  if (iftIs3DImage(img)) {
    A = iftSpheric(1.0);
    B = iftSpheric(sqrtf(3.0));
  } else {
    A = iftCircular(1.0);
    B = iftCircular(sqrtf(2.0));
  }

  /* Initialization */
  
  prev_label = iftCopyImage(label);
  grad      = iftImageGradientMagnitude(img,B);  
  weight    = iftSmoothWeightImage(grad,0.5);

  next_label  = iftCopyImage(prev_label);
  norm_factor = iftWeightNormFactor(weight,A);
  inFrontier  = iftCreateBMap(prev_label->n);

  max_prev_label = iftMaximumValue(prev_label);
  sum         = iftAllocFloatArray(max_prev_label + 1);
  prev_weight = iftCreateFImage(prev_label->xsize, prev_label->ysize, prev_label->zsize);
  next_weight = iftCreateFImage(next_label->xsize, next_label->ysize, next_label->zsize);
  prev_frontier = iftObjectBorderSet(prev_label, A);

  for (p = 0; p < prev_label->n; p++){
    prev_weight->val[p] = next_weight->val[p] = 1.0;
  }

  S = prev_frontier;
  while (S != NULL) {
    p = S->elem;
    iftBMapSet1(inFrontier,p);
    S = S->next;
  }

  /* Smooth frontier and reset its path values */

  for (iter = 0; iter < num_smooth_iterations; iter++)
    {      
      while (prev_frontier != NULL)
        {
    p = iftRemoveSet(&prev_frontier);
    iftInsertSet(&next_frontier, p);
    u   = iftGetVoxelCoord(prev_label, p);

    for (l = 0; l <= max_prev_label; l++)
            {
        sum[l] = 0.0;
            }
    
    for (i = 1; i < A->n; i++)
            {
        v = iftGetAdjacentVoxel(A, u, i);
        if (iftValidVoxel(prev_label, v))
                {
      q = iftGetVoxelIndex(prev_label, v);
      sum[prev_label->val[q]] += prev_weight->val[q] * weight->val[q];
      if (iftBMapValue(inFrontier, q) == 0) /* expand frontier */
                    {
          //if (igraph->pred[q] != NIL)
          //              {
        iftInsertSet(&next_frontier, q);
        iftBMapSet1(inFrontier, q);
          //              }
                    }
                }
            }

    for (l = 0; l <= max_prev_label; l++)
      sum[l]  = sum[l] / norm_factor->val[p];

    max_membership = IFT_INFINITY_FLT_NEG; max_label = IFT_NIL;
    for (l = 0; l <= max_prev_label; l++)
            {
        if (sum[l] > max_membership)
                {
      max_membership = sum[l];
      max_label      = l;
                }
            }
    next_label->val[p]  = max_label;
    next_weight->val[p] = sum[max_label];
        }
      
      prev_frontier = next_frontier;
      next_frontier = NULL;
      
      for (r = 0; r < prev_label->n; r++)
        {
    prev_weight->val[r] = next_weight->val[r];
    prev_label->val[r]  = next_label->val[r];
        }
    }

    free(sum);
    iftDestroyFImage(&prev_weight);
    iftDestroyFImage(&next_weight);
    iftDestroyImage(&next_label);
    iftDestroyFImage(&weight);
    iftDestroyFImage(&norm_factor);
    iftDestroyBMap(&inFrontier);
    iftDestroySet(&prev_frontier);
    iftDestroyImage(&grad);

    /* It fixes the label map, by eliminating the smallest regions and
       relabel them with the adjacent labels */

    iftMaximumValue(prev_label);
    next_label = iftSelectKLargestRegionsAndPropagateTheirLabels(prev_label, A, max_prev_label);
    
    iftDestroyImage(&prev_label);
    iftDestroyAdjRel(&A);
    iftDestroyAdjRel(&B);
    return next_label;
}


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
  iftIGraphBasins(igraph);

  /* seeds for ift slic */
  seeds  = iftGridSampling(mimg,mask1,nsuperpixels);
  //seeds  = iftAltMixedSampling(mimg,mask1,nsuperpixels);
  *nseeds = iftNumberOfElements(seeds);

  //seeds  = iftSelectNonBorderVoxels(mimg, mask1, atoi(argv[2]));
  //seeds  = iftSelectNSamplesFromMask(mimg,mask2,atoi(argv[2]));
  
  iftDestroyImage(&mask1);
  iftDestroyMImage(&mimg);
  
  *finalniters = iftIGraphISF1(igraph,seeds,alpha,beta,niters);

  //iftImage *aux    = iftIGraphLabel(igraph);
  label   = iftIGraphLabel(igraph);
  //label    = iftSmoothRegionsByDiffusion(aux,img,0.5,5);

  t2 = iftToc();

  printf("SIB in %f ms\n",iftCompTime(t1,t2));

  //iftDestroyImage(&aux);
  iftDestroyImage(&seeds);
  //iftDestroyImage(&basins);
  iftDestroyIGraph(&igraph);
  //iftDestroyImage(&img);
  iftDestroyAdjRel(&A);

  return label;
}

int main(int argc, char *argv[])
{
  iftImage  *img, *label; //*mask2
  iftImage  *gt_borders = NULL, *gt_regions = NULL, *border = NULL;
  iftAdjRel *A,*B;  
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

  img  = iftReadImageByExt(argv[1]);

  if (argc == 9) {
    gt_borders = iftReadImageByExt(argv[7]);
    gt_regions = iftReadImageByExt(argv[8]);
  }

  if (iftIs3DImage(img)){
    B = iftSpheric(1.0);
  } else {
    B = iftCircular(1.0);
  }

  //label = iftExtractSIBSuperpixels(img, atoi(argv[2]), atof(argv[3]), atof(argv[4]), atoi(argv[5]), &(nseeds), &nautoiters);

  iftImage *img1 = iftInterp2D(img, 0.5, 0.5);
  iftImage *label1 = iftExtractSIBSuperpixels(img1, atoi(argv[2]), atof(argv[3]), atof(argv[4]), atoi(argv[5]), &(nseeds), &nautoiters);
  label = iftInterp2DNN(label1, (img->xsize/(float)label1->xsize), (img->ysize/(float)label1->ysize));
  iftDestroyImage(&label1);
  iftDestroyImage(&img1);

  //label = iftSmoothSuperpixels(img, label2, atoi(argv[6]));
  //iftDestroyImage(&label2);

  // Show seeds
  /*
  iftImage *showSeeds = iftCreateImage(seeds->xsize, seeds->ysize, seeds->zsize);
  for (p=0; p< showSeeds->n; p++) {
    if(seeds->val[p] >0)
      showSeeds->val[p] = 255;
  }
  iftWriteImageP2(showSeeds,"grid_sib.pgm");
  iftDestroyImage(&showSeeds);
  */
  //printf("label ok\n");
  A = iftCircular(0.0);
  border   = iftBorderImage(label);
  //printf("after border\n");
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
    printf("3D\n");
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
  iftDestroyAdjRel(&B);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}
