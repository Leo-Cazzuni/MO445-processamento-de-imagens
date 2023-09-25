#include "ift.h"


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

iftImage *iftExtractISFSuperpixels(iftImage *img, int nsuperpixels, float alpha, float beta, int niters, int smooth_niters, int *nseeds, int *finalniters) {
  iftImage  *mask1, *seeds, *label;
  iftMImage *mimg;
  iftAdjRel *A;
  iftIGraph *igraph;

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

  /* entire image domain */

  mask1  = iftSelectImageDomain(mimg->xsize,mimg->ysize,mimg->zsize);

  /* minima of a basins manifold in that domain */
  igraph = iftImplicitIGraph(mimg,mask1,A);

  /* set weight for region smoothing */
  
  iftIGraphSetWeightForRegionSmoothing(igraph, img);
  
  /* seeds for ift slic */
  seeds  = iftGridSampling(mimg,mask1,nsuperpixels);
  printf("grid1\n");
  //seeds  = iftAltMixedSampling(mimg,mask1,nsuperpixels);
  *nseeds = iftNumberOfElements(seeds);

  iftDestroyImage(&mask1);
  iftDestroyMImage(&mimg);
  
  *finalniters = iftIGraphISF(igraph,seeds,alpha,beta,niters);

  /* Smooth regions in the label map of igraph */
  if (smooth_niters > 0)
    iftIGraphSmoothRegions(igraph, smooth_niters);

  label   = iftIGraphLabel(igraph);

  iftDestroyImage(&seeds);
  iftDestroyIGraph(&igraph);
  iftDestroyAdjRel(&A);
  return label;
}


int main(int argc, char *argv[]) 
{
  if(argc!=9)
    {
        iftError("Usage: <dataset path> <nsamples> <alpha (e.g., [0.005-0.2])> <beta (e.g., 12)> <niters (e.g., 10)> <smooth niters (e.g., 2)> <output path> <output csv nseeds file>", "ISFExperiments");
        return 1;
    }

  FILE *f1;
  iftImage *superpixels, *img;
  iftDir* datasetfiles = iftLoadFilesFromDirBySuffix(argv[1], "ppm");
  int i, niters, mean_niters, nseeds;
  float mean_nseeds;
  int nimages = datasetfiles->nfiles;
  char filename[256];
  f1 = fopen(argv[8], "w");

  mean_nseeds = 0.0;
  mean_niters = 0.0;

  for (i = 0; i < nimages; i++) {

    // Extract superpixels
    printf("Processing: %s\n", datasetfiles->files[i]->path);
    img = iftReadImageByExt(datasetfiles->files[i]->path);

    //superpixels = iftExtractISFSuperpixels(img, atoi(argv[2]), atof(argv[3]), atof(argv[4]), atoi(argv[5]), atoi(argv[6]), &(nseeds), &niters);
    iftImage *img1 = iftInterp2D(img, 0.5, 0.5);
    printf("interp2D\n");
    iftImage *label1 = iftExtractISFSuperpixels(img1, atoi(argv[2]), atof(argv[3]), atof(argv[4]), atoi(argv[5]), atoi(argv[6]), &(nseeds), &niters);
    superpixels = iftInterp2DNN(label1, (img->xsize/(float)label1->xsize), (img->ysize/(float)label1->ysize));
    iftDestroyImage(&label1);
    iftDestroyImage(&img1);

    // Get filename without extension
    strcpy(filename, iftFilename(datasetfiles->files[i]->path, ".ppm"));
    // Write superpixels image
    iftWriteImageP2(superpixels, "%s/%s.pgm", argv[7], filename);

    mean_niters += niters;
    mean_nseeds += nseeds;
    fprintf(f1,"%f, %f\n", (float)niters, (float)nseeds);

    iftDestroyImage(&img);
    iftDestroyImage(&superpixels);
  }
  mean_niters /= nimages;
  mean_nseeds /= nimages;

  printf("Avg iters: %d \n", mean_niters);
  printf("MeanSeeds: %f \n", mean_nseeds);

  fprintf(f1,"%f, %f\n", (float)mean_niters, (float)mean_nseeds);

  fclose(f1);
  iftDestroyDir(&datasetfiles);
  
  return(0);
}
