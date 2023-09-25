#include "ift.h"

void iftForceMinima(iftLabeledSet **S, iftImage *regmin, iftAdjRel *A)
{
  iftLabeledSet *aux = *S, *newS=NULL;
  int l = 1;

  while (aux != NULL){
    int      p   = aux->elem;
    iftVoxel u   = iftGetVoxelCoord(regmin,p);
    int min_dist = IFT_INFINITY_INT, qmin = IFT_NIL;
    for (int i=0; i < A->n; i++) {
      iftVoxel v = iftGetAdjacentVoxel(A,u,i);
      if (iftValidVoxel(regmin,v)){
  int q = iftGetVoxelIndex(regmin,v);
  if ((regmin->val[q]!=0)&& 
      (!iftLabeledSetHasElement(newS,q))){
    int dist = iftSquaredVoxelDistance(u,v);
    if (dist < min_dist){
      min_dist = dist;
      qmin     = q;
    }
  }
      }
    }
    if (qmin != IFT_NIL){
      iftInsertLabeledSet(&newS,qmin,l);
      l++;
    }
    aux = aux->next;
  }

  iftDestroyLabeledSet(S);
  (*S)=newS;

}

iftImage *iftExtract_ISF_REGMIN_Superpixels(iftImage *orig_img, int nsuperpixels, float radius, int area, int *nseeds, int *finalniters, float *proc_time) {
  iftImage       *img[2], *label;
  iftAdjRel      *A, *B, *C;
  iftMImage      *mimg;
  iftImage       *mask, *seed; 
  iftLabeledSet  *S=NULL;
  iftImageForest *fst;
  timer     *t1=NULL,*t2=NULL;

  /* Compute ISF_REGMIN superpixels */

  if (iftIs3DImage(orig_img)){
    iftError("It is not extended to 3D yet","main");
  }
  A      = iftCircular(radius);
  B      = iftCircular(3.0);
  C      = iftCircular(sqrtf(orig_img->n/(float)nsuperpixels));

  img[0]   = iftMedianFilter(orig_img,B);

  if (iftIsColorImage(orig_img)){
    mimg   = iftImageToMImage(img[0],LABNorm_CSPACE);
  } else {
    mimg   = iftImageToMImage(img[0],GRAY_CSPACE);
  } 

  iftImage *aux   = iftImageBasins(img[0],A);
  if (area > 0){
    img[1]          = iftFastAreaClose(aux, area);
    iftDestroyImage(&aux);
  } else {
    img[1] = aux;
  }
  
  iftImage *regmin = iftRegionalMinima(img[1]);
  
  t1 = iftTic();

  mask  = iftSelectImageDomain(mimg->xsize,mimg->ysize,mimg->zsize);
  seed = iftGridSampling(mimg,mask,nsuperpixels);

  S=NULL;
  for (int p=0, l=1; p < seed->n; p++) 
    if (seed->val[p]!=0){
  iftInsertLabeledSet(&S,p,l);
  l++;
    }

  iftForceMinima(&S,regmin,C);

  iftDestroyAdjRel(&B);
  B    = iftCircular(1.5);
  fst  = iftCreateImageForest(img[1], B);

  iftDiffWatershed(fst, S, NULL);

  t2 = iftToc();

  *proc_time = iftCompTime(t1,t2);

  *finalniters = 1;
  *nseeds = iftMaximumValue(fst->label);

  label = iftCopyImage(fst->label);

  iftDestroyImage(&regmin);
  iftDestroyAdjRel(&A);
  iftDestroyAdjRel(&B);
  iftDestroyAdjRel(&C);
  iftDestroyLabeledSet(&S);
  iftDestroyImage(&mask);
  iftDestroyImage(&seed);
  iftDestroyMImage(&mimg);

  iftDestroyImageForest(&fst);
  iftDestroyImage(&img[0]);
  iftDestroyImage(&img[1]);

  return label;
}


int main(int argc, char *argv[]) 
{
  if(argc!=7)
  {
    iftError("Usage: iftISF_REGMIN_Experiments <dataset path> <nsamples> <radius (e.g., 3.0)> <area_close (e.g 50)> <output path> <output csv nseeds file>", "iftISF_REGMIN_Experiments");
    return 1;
  }
  //timer     *t1=NULL,*t2=NULL;
  FILE *f1;
  iftImage *superpixels, *img;
  iftDir* datasetfiles = iftLoadFilesFromDirBySuffix(argv[1], "ppm");
  int i, niters, mean_niters, nseeds, queried_nseeds;
  float mean_nseeds, mean_time, proc_time;
  int nimages = datasetfiles->nfiles;
  char filename[256];
  char filename_ext[256];
  f1 = fopen(argv[6], "w");

  queried_nseeds = atoi(argv[2]);
  
  mean_nseeds = 0.0;
  mean_niters = 0;
  mean_time = 0.0;

  for (i = 0; i < nimages; i++) {
    
    // Extract superpixels
    printf("Processing : %s\n", datasetfiles->files[i]->path);
    img = iftReadImageByExt(datasetfiles->files[i]->path);        
    //t1 = iftTic();
    superpixels = iftExtract_ISF_REGMIN_Superpixels(img, queried_nseeds, atof(argv[3]), atoi(argv[4]), &(nseeds), &niters, &proc_time);
    //t2 = iftToc();
    //mean_time += iftCompTime(t1,t2);

    mean_time += proc_time;

    // Get filename without extension
    strcpy(filename_ext, iftFilename(datasetfiles->files[i]->path, NULL));
    sscanf(filename_ext,"%[^.]",filename);
    // Write superpixels image
    iftWriteImageP2(superpixels, "%s/%s.pgm", argv[5], filename);

    mean_niters += niters;
    mean_nseeds += nseeds;

    fprintf(f1,"%f, %f\n", (float)niters, (float)nseeds);

    iftDestroyImage(&superpixels);
    iftDestroyImage(&img);
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