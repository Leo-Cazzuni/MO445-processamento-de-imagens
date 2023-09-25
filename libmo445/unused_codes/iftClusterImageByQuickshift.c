#include "ift.h"

int main(int argc, char *argv[])
{
  iftImage        *img=NULL,*label=NULL,*aux=NULL;
  iftColor         RGB,YCbCr;
  iftAdjRel       *A=NULL,*B=NULL;
  char             ext[10],*pos;
  int              norm_value;
  char command[200];
  FILE *fpipe;
  float time;
  int nb_superpixels;

  if (argc< 6 || argc>7)
    iftError("Usage: iftClusterImageByQuickshift <image.ppm> <kernel_size> <max_dist> <area(0..300)> <do_smoothing(0:NO/1:YES)> [<gt_image]>","main");

  iftRandomSeed(IFT_RANDOM_SEED);

  sprintf(command,"/usr/bin/python quickshift.py %s %.2f %.2f",argv[1],atof(argv[2]),atof(argv[3]));

  if ( !(fpipe = popen(command, "r")) )
  {   // If fpipe is NULL
    perror("Problems with pipe");
    return 1;
  }

  fscanf(fpipe,"%f %d",&time,&nb_superpixels);
  pclose(fpipe);

  img=iftReadImageByExt(argv[1]);
  label = iftReadImageByExt("labels.pgm");

  aux=iftRelabelGrayScaleImage(label,1);
  iftDestroyImage(&label);
  label=aux;

  fprintf(stdout,"%s %.3f ",argv[1],time);

  if (!iftIs3DImage(img)){
    iftImage *labels_orig=iftColorizeComp(label);
    iftWriteImageP6(labels_orig,"labels_orig.ppm");
    iftDestroyImage(&labels_orig);
  }

  /* check if we do smoothing */
  if (atoi(argv[5])){
    aux=label;
    label = iftSmoothRegionsByDiffusion(aux,img,0.5,2);
    iftDestroyImage(&aux);
  }

  aux = iftSelectAndPropagateRegionsAboveArea(label,atoi(argv[4]));
  iftDestroyImage(&label);
  label=aux;

  printf("%d\n",iftMaximumValue(label));

  if (remove("labels.pgm"))
    iftError("Unable to delete the label file created in python script","main");

  if (iftIs3DImage(img)){
    iftWriteImage(label,"labels.scn");
  }
  else{
    iftWriteImageP2(label,"labels.pgm");
    aux   = iftColorizeComp(label);
    iftWriteImageP6(aux,"clusters.ppm");
    iftDestroyImage(&aux);
  }

  /*compute br and ue*/
  iftImage *border = iftBorderImage(label,0);
  if (argc == 7){
    iftImage *gt_img=iftReadImageByExt(argv[6]);
    aux=iftRelabelGrayScaleImage(gt_img,0);
    iftDestroyImage(&gt_img);
    gt_img=aux;
    iftImage *gt_borders=iftBorderImage(gt_img,0);
    printf("br -> %.4f\n",iftBoundaryRecall(gt_borders, border, 2.0));
    printf("ue -> %.4f\n",iftUnderSegmentation(gt_img, label));

    iftDestroyImage(&gt_img);
    iftDestroyImage(&gt_borders);
  }

  iftWriteImageP2(border, "border.pgm");

  iftDestroyImage(&border);

  if (!iftIs3DImage(img)){
    norm_value = iftNormalizationValue(iftMaximumValue(img));

    iftDestroyAdjRel(&A);
    A  = iftCircular(sqrtf(2.0));
    B          = iftCircular(0.0);
    RGB.val[0] = 0;
    RGB.val[1] = norm_value;
    RGB.val[2] = norm_value;
    YCbCr      = iftRGBtoYCbCr(RGB,norm_value);
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

  }

  iftDestroyAdjRel(&A);
  iftDestroyImage(&img);
  iftDestroyImage(&label);

  return(0);
}
