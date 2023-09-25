#include "ift.h"

iftImage *iftExtract_ISF_MIX_ROOT_Superpixels(iftImage *img, int nsuperpixels, float alpha, float beta, int niters, int smooth_niters, int *nseeds, int *finalniters, float *proc_time) {
  timer     *t1=NULL,*t2=NULL;
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
  
  t1 = iftTic();
  /* seeds for ift slic */
  seeds  = iftAltMixedSampling(mimg,mask1,nsuperpixels);
  *nseeds = iftNumberOfElements(seeds);

  iftDestroyImage(&mask1);
  iftDestroyMImage(&mimg);
  
  *finalniters = iftIGraphISF_Root(igraph,seeds,alpha,beta,niters);

  /* Smooth regions in the label map of igraph */
  if (smooth_niters > 0) {
    iftIGraphSetWeightForRegionSmoothing(igraph, img);
    iftIGraphSmoothRegions(igraph, smooth_niters);
  }

  label   = iftIGraphLabel(igraph);

  t2 = iftToc();
  *proc_time = iftCompTime(t1, t2);

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

  //timer     *t1=NULL,*t2=NULL;
  FILE *f1;
  iftImage *superpixels, *img;
  iftDir* datasetfiles = iftLoadFilesFromDirBySuffix(argv[1], "ppm");
  int i, niters, mean_niters, nseeds;
  float mean_nseeds, mean_time, proc_time;
  int nimages = datasetfiles->nfiles;
  char filename[256];
  f1 = fopen(argv[8], "w");

  mean_nseeds = 0.0;
  mean_niters = 0.0;
  mean_time = 0.0;

  for (i = 0; i < nimages; i++) {

    // Extract superpixels
    printf("Processing isf %s\n", datasetfiles->files[i]->path);

    //t1 = iftTic();
    img = iftReadImageByExt(datasetfiles->files[i]->path);
    superpixels = iftExtract_ISF_MIX_ROOT_Superpixels(img, atoi(argv[2]), atof(argv[3]), atof(argv[4]), atoi(argv[5]), atoi(argv[6]), &(nseeds), &niters, &proc_time);
    //t2 = iftToc();
    //mean_time += iftCompTime(t1,t2);
    mean_time += proc_time;

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

  mean_time /= nimages;
  mean_niters /= nimages;
  mean_nseeds /= nimages;

  printf("Avg iters: %d \n", mean_niters);
  printf("MeanSeeds: %f \n", mean_nseeds);
  printf("Mean processing time in ms: %f\n", mean_time);

  fprintf(f1,"%f, %f\n", (float)mean_niters, (float)mean_nseeds);

  fclose(f1);
  iftDestroyDir(&datasetfiles);
  
  return(0);
}
