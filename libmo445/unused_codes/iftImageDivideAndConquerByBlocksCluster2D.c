#include <ift.h>

int main(int argc, char *argv[])
{
  iftImage        *img=NULL,*label=NULL,*aux=NULL;
  iftDataSet      *Z=NULL;
  iftColor         RGB,YCbCr;
  iftAdjRel       *A=NULL,*B=NULL;
  char             ext[10],*pos;
  timer           *t1=NULL,*t2=NULL;


  if (argc<7 || argc >8)
      iftExit("Usage: iftImageDivideAndConquerByBlocksCluster2D <image.ppm[pgm,png]> <num_blocks> <kmax_perc1 [0,1]> <kmax_perc2 [0,1]> <area> <do_smoothing> <gt_image(OPTIONAL)>",
              "main");

  iftRandomSeed(IFT_RANDOM_SEED);
//  iftRandomSeed((unsigned int)time(NULL));

  img=iftReadImageByExt(argv[1]);

  /* convert the image to multi-image*/
  iftMImage *mimg;
  iftMImage *eimg;

  if (!iftIsColorImage(img)) {
    mimg = iftImageToMImage(img, GRAYNorm_CSPACE);
    A  = iftCircular(sqrtf(2.0));
    eimg=iftExtendMImageByAdjacencyAndVoxelCoord(mimg,A,1);
    iftDestroyMImage(&mimg);
    iftDestroyAdjRel(&A);
    mimg = eimg;
  }
  else{
    mimg = iftImageToMImage(img, YCbCrNorm_CSPACE);
    eimg = iftExtendMImageByVoxelCoord(mimg,1);
    iftDestroyMImage(&mimg);
    mimg = eimg;
  }

  Z= iftMImageToDataSet(mimg);
  fprintf(stdout,"%s\t%.3f\t%.3f\t",argv[1],atof(argv[3]),atof(argv[4]));

  /* read the gt image if it was given as parameter*/
  iftImage *gt=NULL;
  if (argc==8){
    gt=iftReadImageByExt(argv[7]);
    iftImageGTToDataSet(gt,Z);
  }

  t1 = iftTic();
  int ngroups= iftImageDivideAndConquerByBlocksUnsupOPF(Z, atoi(argv[2]), iftNormalizedCut, atof(argv[3]),atof(argv[4]));
  t2 = iftToc();
  fprintf(stdout,"%d\t%.2f\t",ngroups,iftCompTime(t1,t2));

  if (Z->nclasses > 0){
    iftSetStatus(Z,TEST);
    printf("%.2f\t",iftTruePositives(Z));
  }

  label = iftDataSetToLabelImage(Z, false);

  iftImage *labels_orig=iftColorizeComp(label);
  iftWriteImageP6(labels_orig,"labels_orig.ppm");
  iftDestroyImage(&labels_orig);

  int do_smoothing=atoi(argv[6]);

  if (do_smoothing){
    aux=label;
    label = iftSmoothRegionsByDiffusion(aux,img,0.5,5);
    iftDestroyImage(&aux);
  }

  aux   = iftSelectAndPropagateRegionsAboveArea(label,atoi(argv[5]));
  iftDestroyImage(&label);
  label=aux;
  printf("%d",iftMaximumValue(label));
  printf("\n");

  iftWriteImageP2(label,"labels.pgm");
  aux   = iftColorizeComp(label);
  iftWriteImageP6(aux,"clusters.ppm");
  iftDestroyImage(&aux);

  iftImage *border = iftBorderImage(label,0);
  iftWriteImageP2(border, "border.pgm");
  iftDestroyImage(&border);

  iftDestroyAdjRel(&A);
  A  = iftCircular(sqrtf(2.0));
  B          = iftCircular(0.0);
  RGB.val[0] = 0;
  RGB.val[1] = 255;
  RGB.val[2] = 255;
  YCbCr      = iftRGBtoYCbCr(RGB,255);
  aux        = iftCopyImage(img);
  iftDrawBorders(aux,label,A,YCbCr,B);

  pos = strrchr(argv[1],'.') + 1;
  sscanf(pos,"%s",ext);
  if (strcmp(ext,"png")==0)
    iftWriteImagePNG(aux,"regions.png");
  else
    iftWriteImageP6(aux,"regions.ppm");

  iftDestroyImage(&aux);
  iftDestroyAdjRel(&B);
  iftDestroyImage(&gt);
  iftDestroyAdjRel(&A);
  iftDestroyImage(&img);
  iftDestroyMImage(&mimg);
  iftDestroyImage(&label);
  iftDestroyDataSet(&Z);


  return(0);
}




