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


int main(int argc, char *argv[]) 
{
  iftImage       *img[3];
  iftAdjRel      *A, *B, *C;
  iftMImage      *mimg;
  iftImage       *mask, *seed; 
  iftLabeledSet  *S=NULL;
  iftImageForest *fst;
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


  if (argc<6)
    iftError("Usage: iftFastSuperpixel2D <image.[ppm,png,pgm]> <adjacency radius> <number of superpixels> <area> <result> [basins]","main");

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

  if(argc > 6) {
    img[2] = iftReadImageByExt(argv[6]);
  } else {
    iftImage *aux = iftImageBasins(img[1], A);
    if (atoi(argv[4]) > 0) {
      img[2] = iftFastAreaClose(aux, atoi(argv[4]));
      iftDestroyImage(&aux);
    } else {
      img[2] = aux;
    }
  }

  //iftWriteImageByExt(img[2],"basins.pgm");
  iftImage *regmin = iftRegionalMinima(img[2]);
  
  mask  = iftSelectImageDomain(mimg->xsize,mimg->ysize,mimg->zsize);
  seed = iftGridSampling(mimg,mask,atoi(argv[3]));

  S=NULL;
  for (int p=0, l=1; p < seed->n; p++) 
    if (seed->val[p]!=0){
	iftInsertLabeledSet(&S,p,l);
	l++;
    }

  iftForceMinima(&S,regmin,C);

  iftDestroyAdjRel(&B);
  B    = iftCircular(1.5);
  fst  = iftCreateImageForest(img[2], B);

  iftDiffWatershed(fst, S, NULL);

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
  sprintf(filename,"%s.pgm",argv[5]);
  iftWriteImageByExt(fst->label,filename);
  sprintf(filename,"%s.ppm",argv[5]);
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

