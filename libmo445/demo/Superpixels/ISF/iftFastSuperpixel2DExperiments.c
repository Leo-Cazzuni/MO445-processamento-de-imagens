#include "ift.h"

/* removal_seeds returns with the seeds for removal and seeds returns
   with the new seeds */

void iftRecomputeSeeds(iftImageForest *fst, iftAdjRel *A, iftLabeledSet **seeds, iftSet **removal_seeds)
{
  iftLabeledSet *aux = *seeds;
  iftLabeledSet *new_seeds = NULL;
  iftImage      *basins = fst->img, *root = fst->root;

  /* Find the minimum within the given adjacency of each seed. If this
     minimum is better than the current one, it becomes a new seed and
     the previous one is marked for removal. Depending on the
     overlapping of the search region, the number of seeds may
     decrease. */
  
  while (aux != NULL) {
    int p, q;

    p = aux->elem;

    iftVoxel u    = iftGetVoxelCoord(basins,p);
    int      Imin = basins->val[p], qmin = p;
    for (int i=1; i < A->n; i++) {
      iftVoxel v = iftGetAdjacentVoxel(A,u,i);
      if (iftValidVoxel(basins,v)){
	q = iftGetVoxelIndex(basins,v);
	if (root->val[q]!=q){ /* avoids seeds of other superpixels */
	  if (basins->val[q] < Imin){
	    Imin = basins->val[q];
	    qmin = q;
	  }
	}
      }
    }
  

    if (qmin != p){
      iftInsertSet(removal_seeds,p);
      if (!iftIsElemInLabeledSet(new_seeds,qmin))
	iftInsertLabeledSet(&new_seeds,qmin,aux->label);
    }

    aux = aux->next;
  }

  iftDestroyLabeledSet(seeds);
  (*seeds) = new_seeds;
}

iftImage *iftExtractFastSuperpixels(const char *fullpath, int nsuperpixels, float radius, int *nseeds, int *finalniters) {
  iftImage       *img[3], *label;
  iftAdjRel      *A, *B, *C;
  iftMImage      *mimg;
  iftImage       *mask, *seed; 
  iftLabeledSet  *S=NULL;
  iftImageForest *fst;
  iftSet         *R=NULL;

  /* Compute Fast superpixels */
  img[0] = iftReadImageByExt(fullpath);    

  if (iftIs3DImage(img[0])){
    iftError("It is not extended to 3D yet","main");
  }
  A      = iftCircular(radius);
  B      = iftCircular(3.0);
  C      = iftCircular(sqrtf(img[0]->n/(float)nsuperpixels));

  img[1]   = iftMedianFilter(img[0],B);

  if (iftIsColorImage(img[0])){
    mimg   = iftImageToMImage(img[1],LABNorm_CSPACE);
  } else {
    mimg   = iftImageToMImage(img[1],GRAY_CSPACE);
  }

  
  //img[2]   = iftMImageBasins(mimg,A); 
  img[2]   = iftImageBasins(img[1],A); 
  iftWriteImageByExt(img[2],"basins.pgm");
    
  mask  = iftSelectImageDomain(mimg->xsize,mimg->ysize,mimg->zsize);
  seed = iftGridSampling(mimg,mask,nsuperpixels);
  //seed = iftSelectNonBorderVoxels(mimg, mask, atoi(argv[3]));
  //seed = iftAltMixedSampling(mimg,mask,atoi(argv[3]));
  S=NULL;
  for (int p=0, l=1; p < seed->n; p++) 
    if (seed->val[p]!=0){
  iftInsertLabeledSet(&S,p,l);
  l++;
    }
  
  iftDestroyAdjRel(&B);
  B    = iftCircular(1.0);
  fst  = iftCreateImageForest(img[2], B);
  
  printf("iteration 1\n");
  iftDiffWatershed(fst, S, NULL);


  R = NULL;
  iftRecomputeSeeds(fst, C, &S, &R);
  
  int i=2;
  while (R != NULL) {
    printf("iteration %d\n",i); i++; 
    iftDiffWatershed(fst, S, R);
    iftDestroySet(&R);
    iftRecomputeSeeds(fst, C, &S, &R);
  }
  *finalniters = i;
  *nseeds = iftMaximumValue(fst->label);
  
  printf("Number of superpixels %d\n", (*nseeds));
  
  label = iftCopyImage(fst->label);

  iftDestroyImageForest(&fst);

  iftDestroyAdjRel(&A);
  iftDestroyAdjRel(&B);
  iftDestroyAdjRel(&C);
  iftDestroyLabeledSet(&S);
  iftDestroyImage(&mask);
  iftDestroyImage(&seed);
  iftDestroyMImage(&mimg);

  iftDestroyImage(&img[0]);
  iftDestroyImage(&img[1]);
  iftDestroyImage(&img[2]);

  return label;
}


int main(int argc, char *argv[]) 
{
  if(argc!=6)
    {
        iftError("Usage: iftFastSuperpixel2DExperiments <dataset path> <nsamples> <radius (e.g., 3.0)> <output path> <output csv nseeds file>", "SIBExperiments");
        return 1;
    }
  timer     *t1=NULL,*t2=NULL;
  FILE *f1;
  iftImage *superpixels;
  iftDir* datasetfiles = iftLoadFilesFromDirBySuffix(argv[1], "ppm");
  int i, niters, mean_niters, nseeds;
  float mean_nseeds, mean_time;
  int nimages = datasetfiles->nfiles;
  char filename[256];
  char filename_ext[256];
  f1 = fopen(argv[5], "w");
  
  mean_nseeds = 0.0;
  mean_niters = 0;
  mean_time = 0.0;
  
  for (i = 0; i < nimages; i++) {
    
    // Extract superpixels
    printf("Processing img or only mix: %s\n", datasetfiles->files[i]->path);

    t1 = iftTic();
    superpixels = iftExtractFastSuperpixels(datasetfiles->files[i]->path, atoi(argv[2]), atof(argv[3]), &(nseeds), &niters);
    t2 = iftToc();

    mean_time += iftCompTime(t1,t2);

    // Get filename without extension
    strcpy(filename_ext, iftFilename(datasetfiles->files[i]->path, NULL));
    sscanf(filename_ext,"%[^.]",filename);
    // Write superpixels image
    iftWriteImageP2(superpixels, "%s/%s.pgm", argv[4], filename);

    mean_niters += niters;
    mean_nseeds += nseeds;
    fprintf(f1,"%f, %f\n", (float)niters, (float)nseeds);

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

