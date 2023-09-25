#include "ift.h"
#include "suputils.h"

iftImage *iftExtractSIBSuperpixels(const char *fullpath, int nsuperpixels, float alpha, float beta, int niters, int *nseeds, int *finalniters) {
  iftImage  *mask1, *seeds, *label, *img;
  iftMImage *mimg;
  iftAdjRel *A;
  iftIGraph *igraph;
  img = iftReadImageP6(fullpath);

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
  iftDestroyImage(&img);
  iftDestroyAdjRel(&A);

  return label;
}

int main(int argc, char *argv[]) 
{
  if(argc!=8)
    {
        iftError("Usage: <dataset path> <nsamples> <alpha (e.g., [0.005-0.2])> <beta (e.g., 12)> <niters (e.g., 10)> <output path> <output csv nseeds file>", "SIBExperiments");
        return 1;
    }

  FILE *f1;
  iftImage *superpixels;
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
    printf("Processing img: %s\n", datasetfiles->files[i]->path);
    superpixels = iftExtractSIBSuperpixels(datasetfiles->files[i]->path, atoi(argv[2]), atof(argv[3]), atof(argv[4]), atoi(argv[5]), &(nseeds), &niters);
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