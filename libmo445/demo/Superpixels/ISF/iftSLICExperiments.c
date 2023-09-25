#include <stdio.h>
#include <stdlib.h>
#include "ift.h"


float iftSLICBoundaryRecall(int i, char *path, char *pathLabel, char *pathOut, int nsuperpixels, float alpha, float beta, int niters) {
    iftAdjRel *Abr;
    iftImage  *mask1, *mask2, *seeds, *label;
    iftMImage *mimg;
    iftAdjRel *A;
    iftIGraph *igraph;
    char fullPath[256];
    char fullPathLabel[256];
    char fullPathOut[256];
    float br = 0.0;
    
    Abr      = iftCircular(2.0);
    
    sprintf(fullPath, "%s000001_0000%02d.ppm", path, (i+1));
    sprintf(fullPathLabel, "%s000001_0000%02d.pgm", pathLabel, (i+1));
    sprintf(fullPathOut, "%s000001_0000%02d.ppm", pathOut, (i+1));
    printf("%s\n", fullPath);
    
    iftImage* img = iftReadImageP6(fullPath);
    iftImage* gt = iftReadImageP5(fullPathLabel);


    /* Compute IFTSLIC superpixels */

    if (iftIs3DImage(img)){
      A      = iftSpheric(1.0);
    } else {
      A      = iftCircular(1.0);
    }
   
    if (iftIsColorImage(img)){
      mimg   = iftImageToMImage(img,LABNorm_CSPACE);
    } else {
      mimg   = iftImageToMImage(img,GRAYNorm_CSPACE);
    }
 
    /* entire image domain */

    mask1  = iftSelectImageDomain(mimg->xsize,mimg->ysize,mimg->zsize);  

    /* minima of a basins manifold in that domain */
    igraph = iftImplicitIGraph(mimg,mask1,A);
    iftIGraphBasins(igraph);
    mask2  = iftIGraphWeightMinima(igraph);

    /* seeds for ift slic */

    seeds  = iftGridSampling(mimg,mask1,nsuperpixels);
    //seeds  = iftSelectNonBorderVoxels(mimg, mask1, atoi(argv[2]));

    //seeds  = iftSelectNSamplesFromMask(mimg,mask2,atoi(argv[2]));

    iftDestroyImage(&mask1);
    iftDestroyImage(&mask2);
    iftDestroyMImage(&mimg);

    iftIGraphSLICRecompSeeds(igraph,seeds,alpha,beta,niters);
  
    iftImage *aux    = iftIGraphLabel(igraph);
    label    = iftSmoothRegionsByDiffusion(aux,img,0.5,5);

    /* Compute boundary recall */
    //printf("img: %d, %d \n", gt->ysize, gt->xsize);
    //printf("label: %d, %d \n", label->ysize, label->xsize);
    br = iftBoundaryRecall(gt, label, Abr);
    
    iftDestroyImage(&aux);
    iftDestroyImage(&seeds);
    iftDestroyIGraph(&igraph);

    if (!iftIs3DImage(img)){
      iftColor  RGB, YCbCr;
      iftAdjRel *B = iftCircular(1.0);    
      RGB.val[0] = 0;
      RGB.val[1] = 255;
      RGB.val[2] = 255;
      YCbCr      = iftRGBtoYCbCr(RGB,255);
      iftWriteImageP2(label,"label.pgm");
      iftDrawBorders(img,label,A,YCbCr,B);
      iftWriteImageP6(img, fullPathOut);
      iftDestroyAdjRel(&B);
      iftDestroyAdjRel(&A);
    } else {
      iftWriteImage(label,"label.scn");
    }

    iftDestroyImage(&img);
    iftDestroyImage(&label);
    iftDestroyAdjRel(&A);
    
    iftDestroyImage(&gt);

    iftDestroyAdjRel(&Abr);

    return br;
}


int main(int argc, char *argv[]) 
{
  if(argc!=8)
    {
        iftError("Usage: <dataset path> <label path> <nsamples> <alpha (e.g., [0.005-0.2])> <beta (e.g., 12)> <niters (e.g., 10)> <output path>", "SLICExperiments");
        return 1;
    }

  iftDir* datasetfiles = iftLoadFilesFromDirBySuffix(argv[1], "ppm");
  
  int i;
  int nImages = datasetfiles->nfiles;
  
  float *boundaryRecall = iftAllocFloatArray(nImages);
  float meanBR = 0.0;
  
  #pragma omp parallel for
  for (i = 0; i < nImages; i++) {

    boundaryRecall[i] = iftSLICBoundaryRecall(i, argv[1], argv[2], argv[7], atoi(argv[3]), atof(argv[4]), atof(argv[5]), atoi(argv[6]));
    printf("BR[%d]: %f \n", i, boundaryRecall[i]);
    meanBR += boundaryRecall[i];
  }
  meanBR /= (float)nImages;

  printf("Mean BR: %f \n", meanBR);
  
  
  free(boundaryRecall);
  
  
  
  return(0);
}

