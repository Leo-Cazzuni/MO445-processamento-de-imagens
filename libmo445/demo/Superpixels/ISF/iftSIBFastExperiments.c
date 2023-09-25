#include "ift.h"
#include "suputils.h"

iftImage *iftExtractSIBSuperpixels(iftImage *img, int nsuperpixels, float alpha, float beta, int niters, int *nseeds, int *finalniters) {
  iftImage  *mask1, *seeds, *label;
  iftMImage *mimg;
  iftAdjRel *A;
  iftIGraph *igraph;

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

  //iftDestroyImage(&aux);
  iftDestroyImage(&seeds);
  //iftDestroyImage(&basins);
  iftDestroyIGraph(&igraph);
  //iftDestroyImage(&img);
  iftDestroyAdjRel(&A);

  return label;
}

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

int main(int argc, char *argv[]) 
{
  if(argc!=8)
    {
        iftError("Usage: <dataset path> <nsamples> <alpha (e.g., [0.005-0.2])> <beta (e.g., 12)> <niters (e.g., 10)> <output path> <output csv nseeds file>", "SIBExperiments");
        return 1;
    }

  FILE *f1;
  iftImage *superpixels, *img;
  iftDir* datasetfiles = iftLoadFilesFromDirBySuffix(argv[1], "ppm");
  int i, niters, mean_niters, nseeds;
  float mean_nseeds;
  int nimages = datasetfiles->nfiles;
  char filename[256];
  char filename_ext[256];
  f1 = fopen(argv[7], "w");
  
  mean_nseeds = 0.0;
  mean_niters = 0;
  
  for (i = 0; i < nimages; i++) {
    
    // Extract superpixels
    printf("Processing img sib-fast: %s\n", datasetfiles->files[i]->path);
    img = iftReadImageByExt(datasetfiles->files[i]->path);

    //superpixels = iftExtractSIBSuperpixels(datasetfiles->files[i]->path, atoi(argv[2]), atof(argv[3]), atof(argv[4]), atoi(argv[5]), &(nseeds), &niters);

    iftImage *img1 = iftInterp2D(img, 0.5, 0.5);
    iftImage *label1 = iftExtractSIBSuperpixels(img1, atoi(argv[2]), atof(argv[3]), atof(argv[4]), atoi(argv[5]), &(nseeds), &niters);
    superpixels = iftInterp2DNN(label1, (img->xsize/(float)label1->xsize), (img->ysize/(float)label1->ysize));
    iftDestroyImage(&label1);
    iftDestroyImage(&img1);


    // Get filename without extension
    strcpy(filename_ext, iftFilename(datasetfiles->files[i]->path, NULL));
    sscanf(filename_ext,"%[^.]",filename);
    // Write superpixels image
    iftWriteImageP2(superpixels, "%s/%s.pgm", argv[6], filename);

    mean_niters += niters;
    mean_nseeds += nseeds;
    fprintf(f1,"%f, %f\n", (float)niters, (float)nseeds);

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