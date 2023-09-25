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
  iftImage  *img, *label;
  iftImage  *gt_borders=NULL, *gt_regions=NULL, *border=NULL;
  iftAdjRel *A;
  int        nautoiters, nseeds;
  char       labelfilename[256];
  char       borderfilename[256];
  iftColor   RGB, YCbCr;
  int        normvalue, queried_nseeds;
  float proc_time;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc<6 || argc>8 )
    iftError("Usage: iftISF <image.[pgm,ppm,scn,png]> <nsamples> <radius (e.g., 3.0)> <area_close (e.g 50)> <output_label> <ground-truth border image (OPTIONAL)> <ground-truth region image (OPTIONAL)>","main");

  img  = iftReadImageByExt(argv[1]);

  if (argc >= 7){
    gt_borders = iftReadImageByExt(argv[6]);
    gt_regions = iftReadImageByExt(argv[7]);
  }
  
  queried_nseeds = atoi(argv[2]);

  normvalue =  iftNormalizationValue(iftMaximumValue(img)); 
  
  label = iftExtract_ISF_REGMIN_Superpixels(img, queried_nseeds, atof(argv[3]), atoi(argv[4]), &(nseeds), &nautoiters, &proc_time);

  printf("ISF REGMIN processing time: %f\n", proc_time);
  
  border  = iftBorderImage(label,0);
  
  A = iftCircular(0.0);
  
  nseeds = iftMaximumValue(label);
  
  // Compute metrics
  if (argc>=7){
    float br, ue, comp, topology;
    br        = iftBoundaryRecall(gt_borders, border, 2.0);
    printf("BR: %f \n", br);
    
    ue       = iftUnderSegmentation(gt_regions, label);
    printf("UE: %f \n", ue);

    comp     = iftCompactness2D(label);
    printf("Comp: %f \n", comp);

    topology = iftTopologyMeasure(label);
    printf("Top: %f \n", topology);
  }
  
  // Write output image
  if (!iftIs3DImage(img)){
    if (argc >= 7){
      RGB.val[0] = normvalue;
      RGB.val[1] = 0;
      RGB.val[2] = normvalue;
      YCbCr      = iftRGBtoYCbCr(RGB, normvalue);
      iftDrawBorders(img,gt_borders,A,YCbCr,A);
    }

    RGB.val[0] = normvalue;
    RGB.val[1] = normvalue/3.0;
    RGB.val[2] = 0;
    
    YCbCr      = iftRGBtoYCbCr(RGB, normvalue);
    sprintf(labelfilename, "%s.pgm", argv[5]);
    iftWriteImageP2(label,labelfilename);
    iftDrawBorders(img,border,A,YCbCr,A);
    sprintf(borderfilename, "%s.png", argv[5]);
    iftWriteImageByExt(img, borderfilename);
  } else {
    printf("%d\n",iftMaximumValue(label));
    sprintf(labelfilename, "%s.scn", argv[5]);
    iftWriteImage(label,labelfilename);
    RGB.val[2] = RGB.val[1] = RGB.val[0] = iftMaximumValue(img);
    YCbCr      = iftRGBtoYCbCr(RGB, RGB.val[0]);
    iftLabeledSet* Centers = iftGeodesicCenters(label);
    iftDestroyAdjRel(&A);
    A = iftSpheric(1.0);
    while (Centers != NULL) {
      int trash;
      int p = iftRemoveLabeledSet(&Centers,&trash);
      iftVoxel u = iftGetVoxelCoord(img,p);
      iftDrawPoint(img, u, YCbCr, A,255);
    }
    sprintf(borderfilename, "%s-centers.scn", argv[5]);
    iftWriteImage(img,borderfilename);
  }

  // Print number of iterations and superpixels
  printf("Number of iterations = %d \n", nautoiters);
  printf("Number of superpixels = %d \n", nseeds);
  
  
  // Free
  iftDestroyImage(&img);
  if (argc>=7) {
    iftDestroyImage(&gt_borders);
    iftDestroyImage(&gt_regions);
  }
  iftDestroyImage(&label);
  iftDestroyImage(&border);
  iftDestroyAdjRel(&A);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
     MemDinInicial,MemDinFinal);   

  return(0);
}
