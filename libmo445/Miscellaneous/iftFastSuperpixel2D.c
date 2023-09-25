#include "ift.h"

void iftForceMinima(iftLabeledSet **S, iftImage *basins, iftImage *regmin, iftAdjRel *A, iftImage *root, iftImage *label)
{
  iftLabeledSet *aux = *S, *newS=NULL;

  if (root == NULL) {
    while (aux != NULL){
      int p = aux->elem;
      iftVoxel u = iftGetVoxelCoord(regmin,p);
      int Imin = basins->val[p], qmin = p;
      for (int i=1; i < A->n; i++) {
	iftVoxel v = iftGetAdjacentVoxel(A,u,i);
	if (iftValidVoxel(regmin,v)){
	  int q = iftGetVoxelIndex(regmin,v);
	  if ((regmin->val[q]!=0)&&
	      (basins->val[q] < Imin)){
	    Imin = basins->val[q];
	    qmin = q;
	  }
	}
      }
      
      if (qmin != p){
	if (!iftLabeledSetHasElement(newS,qmin))
	  iftInsertLabeledSet(&newS,qmin,aux->label);
	else
	  iftInsertLabeledSet(&newS,p,aux->label);
      } else 
	iftInsertLabeledSet(&newS,p,aux->label);
      
      aux = aux->next;
    }
  } else {
    while (aux != NULL){
      int p = aux->elem;
      iftVoxel u = iftGetVoxelCoord(regmin,p);
      int Imin = basins->val[p], qmin = p;
      for (int i=1; i < A->n; i++) {
	iftVoxel v = iftGetAdjacentVoxel(A,u,i);
	if (iftValidVoxel(regmin,v)){
	  int q = iftGetVoxelIndex(regmin,v);
	  if ((root->val[q]!=q)&&
	      (regmin->val[q]!=0)&&
	      (basins->val[q] < Imin)){
	    Imin = basins->val[q];
	    qmin = q;
	  }
	}
      }
      
      if (qmin != p){
	if (!iftLabeledSetHasElement(newS,qmin))
	  iftInsertLabeledSet(&newS,qmin,aux->label);
	else
	  iftInsertLabeledSet(&newS,p,aux->label);
      } else 
	iftInsertLabeledSet(&newS,p,aux->label);
      
      aux = aux->next;
    }
  }

  iftDestroyLabeledSet(S);
  (*S)=newS;
}

/* removal_seeds returns with the seeds for removal and seeds returns
   with the new seeds */

void iftRecomputeSeeds(iftImageForest *fst, iftAdjRel *A, iftLabeledSet **seeds, iftSet **removal_seeds, iftImage *regmin)
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
	  if ((regmin->val[q]!=0)&&
	      (basins->val[q] < Imin)){
	    Imin = basins->val[q];
	    qmin = q;
	  }
	}
      }
    }
  

    if (qmin != p){
      if (!iftLabeledSetHasElement(new_seeds,qmin)){
	iftInsertSet(removal_seeds,p);
	iftInsertLabeledSet(&new_seeds,qmin,aux->label);
      } 
    }

    aux = aux->next;
  }




  iftDestroyLabeledSet(seeds);
  (*seeds) = new_seeds;
}

void iftRecomputeSeedsNew(iftImageForest *fst, iftAdjRel *A, iftLabeledSet **seeds, iftSet **removal_seeds, iftImage *regmin)
{
  iftLabeledSet *aux = *seeds;
  iftLabeledSet *new_seeds = NULL;
  iftImage      *basins = fst->img, *root=fst->root;
  int            nregions;
  iftVoxel      *center = (iftVoxel *)calloc((nregions=iftMaximumValue(fst->label))+1,sizeof(iftVoxel));
  int           *size   = iftAllocIntArray(nregions+1);


  for (int p = 0; p < basins->n; p++) { 
    iftVoxel u    = iftGetVoxelCoord(basins,p);
    
    center[fst->label->val[p]].x += u.x;
    center[fst->label->val[p]].y += u.y;
    center[fst->label->val[p]].z += u.z;
    size[fst->label->val[p]]++;

  }

  for (int i=1; i <= nregions; i++) {
    center[i].x /= size[i];
    center[i].y /= size[i];
    center[i].z /= size[i];
    int p = iftGetVoxelIndex(basins,center[i]);

    if (fst->label->val[p]!=i){ 
      aux=*seeds; 
      while (aux != NULL) {
	if (aux->label == i) {
	  p = aux->elem;
	  break;
	}
	aux = aux->next;
      }
      iftInsertLabeledSet(&new_seeds,p,i);
    } else {
      iftInsertLabeledSet(&new_seeds,p,i);
    }
  }
  free(center); free(size);

  iftForceMinima(&new_seeds, basins, regmin, A, root, fst->label);

  aux = *seeds;

  while (aux != NULL) {
    int p;

    p = aux->elem;
    
    if (!iftLabeledSetHasElement(new_seeds,p)){
      iftInsertSet(removal_seeds,p);
    }
    
    aux = aux->next;
  }

  iftDestroyLabeledSet(seeds);
  (*seeds) = new_seeds;
}

int main(int argc, char *argv[]) 
{
  iftImage       *img[3];
  iftAdjRel      *A, *B, *C;
  iftMImage      *mimg;
  iftImage       *mask, *seed; 
  iftLabeledSet  *S=NULL;
  iftImageForest *fst;
  iftSet         *R=NULL;
  iftColor        RGB, YCbCr;
  int             normvalue;
  timer          *t1=NULL,*t2=NULL;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/


  if (argc!=5)
    iftError("Usage: iftFastSuperpixel2D <image.[ppm,png,pgm]> <adjacency radius> <number of superpixels> <result>","main");

  img[0] = iftReadImageByExt(argv[1]);    
  normvalue =  iftNormalizationValue(iftMaximumValue(img[0])); 

  if (iftIs3DImage(img[0])){
    iftError("It is not extended to 3D yet","main");
  }
  A      = iftCircular(atof(argv[2]));
  B      = iftCircular(3.0);
  C      = iftCircular(sqrtf(img[0]->n/atof(argv[3])));

  img[1]   = iftMedianFilter(img[0],B);

  if (iftIsColorImage(img[0])){
    mimg   = iftImageToMImage(img[1],LABNorm_CSPACE);
  } else {
    mimg   = iftImageToMImage(img[1],GRAY_CSPACE);
  }

  t1     = iftTic();
  
  //img[2]   = iftMImageBasins(mimg,A); 
  img[2]   = iftImageBasins(img[1],A); 



  iftWriteImageByExt(img[2],"basins.pgm");
  iftImage *regmin = iftRegionalMinima(img[2]);
  
  mask  = iftSelectImageDomain(mimg->xsize,mimg->ysize,mimg->zsize);
  seed = iftGridSampling(mimg,mask,atoi(argv[3]));

  //seed = iftSelectNonBorderVoxels(mimg, mask, atoi(argv[3]));
  //seed = iftAltMixedSampling(mimg,mask,atoi(argv[3]));
  S=NULL;
  for (int p=0, l=1; p < seed->n; p++) 
    if (seed->val[p]!=0){
	iftInsertLabeledSet(&S,p,l);
	l++;
    }

  iftForceMinima(&S,img[2],regmin,C,NULL,NULL);
  
  iftDestroyAdjRel(&B);
  B    = iftCircular(1.0);
  fst  = iftCreateImageForest(img[2], B);
  
  printf("iteration 1\n");
  iftDiffWatershed(fst, S, NULL);

  R = NULL;
  iftRecomputeSeeds(fst, C, &S, &R, regmin);
  
  int i = 2;
  //  for (int i=2; i < 10; i++)  {
  while (R != NULL) {
    printf("iteration %d\n",i); i++; 
    iftDiffWatershed(fst, S, R);
    iftDestroySet(&R);
    iftRecomputeSeeds(fst, C, &S, &R, regmin);
  }
  
  t2     = iftToc();

  fprintf(stdout,"Superpixels computed in %f ms\n",iftCompTime(t1,t2));
  printf("Number of superpixels %d\n",iftMaximumValue(fst->label));

  iftDestroyImage(&regmin);
  iftDestroyAdjRel(&A);
  iftDestroyAdjRel(&B);
  iftDestroyAdjRel(&C);
  iftDestroyLabeledSet(&S);
  iftDestroyImage(&mask);
  iftDestroyImage(&seed);
  iftDestroyMImage(&mimg);
  

  RGB.val[0] = normvalue/3.0;
  RGB.val[1] = normvalue;
  RGB.val[2] = normvalue/3.0;
  YCbCr      = iftRGBtoYCbCr(RGB,normvalue);
  A          = iftCircular(1.0);
  B          = iftCircular(0.0);
  iftDrawBorders(img[0],fst->label,A,YCbCr,B);
  char filename[200];
  sprintf(filename,"%s.pgm",argv[4]);
  iftWriteImageByExt(fst->label,filename);
  sprintf(filename,"%s.ppm",argv[4]);
  iftWriteImageByExt(img[0],filename);
  
  iftDestroyImage(&img[0]);
  iftDestroyImage(&img[1]);
  iftDestroyImage(&img[2]);
  iftDestroyImageForest(&fst);
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

