#include <ift.h>

struct problem{
    char *orig_img_path;
    char *gt_labels_path;
    int count_train_img;
    int limit_nb_superp;
    int limit_secs;
    int type_measure; /* 0 for br; 1 for ue*/
    int train_samples;
    char *orig_img_ext;
    int nb_blocks;
    int area;
};


float find_best_args(void* my_problem, float* theta) {

  /*check the theta params*/
  if (theta[0] <0.001 || theta[0] > 0.5 || theta[1] <0.001 || theta[1] > 0.5 )
    return IFT_INFINITY_FLT_NEG;

  char *orig_img_path = ((struct problem *)my_problem)->orig_img_path;
  char *gt_labels_path = ((struct problem *)my_problem)->gt_labels_path;
  char *orig_img_ext= ((struct problem *)my_problem)->orig_img_ext;

  iftDir *image_files = iftLoadFilesFromDirBySuffix(orig_img_path, orig_img_ext);
  char *gt_img_ext="";
  if (!strcmp(orig_img_ext,"png") || !strcmp(orig_img_ext,"PNG") || !strcmp(orig_img_ext,"ppm") || !strcmp(orig_img_ext,"PPM") || !strcmp(orig_img_ext,"pgm") || !strcmp(orig_img_ext,"PGM"))
    gt_img_ext="pgm";
  else
  if (!strcmp(orig_img_ext,"scn"))
    gt_img_ext="scn";
  else
    iftError("Invalid extension for orig image directory","find_best_args");
  iftDir *gt_label_files = iftLoadFilesFromDirBySuffix(gt_labels_path, gt_img_ext);

  int file_count = ((struct problem *)my_problem)->count_train_img;
  int limit_nb_superp = ((struct problem *)my_problem)->limit_nb_superp;
  int limit_secs = ((struct problem *)my_problem)->limit_secs;
  int type_measure=((struct problem *)my_problem)->type_measure;

  float measure_acc = 0.0;
  int superpixels_acc = 0;

  iftImage        *img=NULL,*label=NULL,*aux=NULL,*gt_label=NULL,*border=NULL,*gt_border=NULL;
  iftDataSet      *Z=NULL;
  timer           *t1=NULL,*t2=NULL;
  iftAdjRel *A;
  iftMImage *mimg,*eimg;

//  int nb_blocks=(int)theta[0];
//  float train_perc_arg=theta[1];
  int nb_blocks=((struct problem *)my_problem)->nb_blocks;
  float train_perc_arg=((struct problem *)my_problem)->train_samples;
  float kmax1_arg=theta[0];
  float kmax2_arg=theta[1];
  int area_arg=((struct problem *)my_problem)->area;
  bool exited=false;

  for (int i=0;i<file_count;i++) {

    img = iftReadImageByExt(image_files->files[i]->path);

    /* convert the image to multi-image*/
    if (!iftIsColorImage(img)) {
      mimg = iftImageToMImage(img, GRAYNorm_CSPACE);
      if (img->zsize > 1)
        A = iftSpheric(1.0);
      else
        A = iftCircular(sqrtf(2.0));
      eimg=iftExtendMImageByAdjacencyAndVoxelCoord(mimg,A,1);
      iftDestroyMImage(&mimg);
      iftDestroyAdjRel(&A);
      mimg = eimg;
    }
    else {
      mimg = iftImageToMImage(img, YCbCrNorm_CSPACE);
      eimg = iftExtendMImageByVoxelCoord(mimg, 1);
      iftDestroyMImage(&mimg);
      mimg = eimg;
    }

    Z= iftMImageToDataSet(mimg);

    t1 = iftTic();
    iftKnnGraph* graph=iftClusterImageByDivideAndConquerBlocksUsingOPF(Z, nb_blocks, train_perc_arg, iftNormalizedCut,
                                                    kmax1_arg, kmax2_arg);
    t2 = iftToc();
    iftDestroyKnnGraph(&graph);

    float time = iftCompTime(t1, t2) / 1000;
    /* if the method execution is more than the time specified discard this configuration of parameters*/
    if (time > (float)limit_secs){
      measure_acc =IFT_INFINITY_FLT_NEG;
      exited=true;
      break;
    }

    label = iftDataSetToLabelImage(Z,false);

    /* do smoothing in the label image*/
    aux = iftSmoothRegionsByDiffusion(label,img,0.5,2);
    iftDestroyImage(&label);
    label=aux;

    /* eliminate short superpixels*/
    aux = iftSelectAndPropagateRegionsAboveArea(label,area_arg);
    iftDestroyImage(&label);
    label=aux;

    int nb_sup=iftMaximumValue(label);
    if (nb_sup > 2*limit_nb_superp){
      measure_acc =IFT_INFINITY_FLT_NEG;
      exited=true;
      break;
    }
    superpixels_acc+=nb_sup;

    gt_label=iftReadImageByExt(gt_label_files->files[i]->path);
    aux=iftRelabelGrayScaleImage(gt_label);
    iftDestroyImage(&gt_label);
    gt_label=aux;

    if (type_measure){
      /*computing under segmentation error*/
      /*here we are minimizing not maximizing*/
      measure_acc -= iftUnderSegmentation(gt_label,label);
    }
    else{
      /*compute boundary recall*/
      border  = iftBorderImage(label,0);
      gt_border=iftBorderImage(gt_label,0);
      measure_acc += iftBoundaryRecall(gt_border, border, 2.0);

      iftDestroyImage(&border);
      iftDestroyImage(&gt_border);
    }

    iftDestroyImage(&img);
    iftDestroyMImage(&mimg);
    iftDestroyDataSet(&Z);
    iftDestroyImage(&label);

    iftDestroyImage(&gt_label);

  }

  if (!exited){
    float mean_sup=superpixels_acc/file_count;
    if (mean_sup > (float)limit_nb_superp)
      measure_acc =IFT_INFINITY_FLT_NEG;
    else
      measure_acc /=file_count;
  }

  iftDestroyDir(&image_files);
  iftDestroyDir(&gt_label_files);

  return measure_acc;

}

int main(int argc, char *argv[])
{
  iftRandomSeed(IFT_RANDOM_SEED);

  if (argc != 13)
    iftError("Usage: iftClusterImageByDivideAndConquerBlocksUsingOPFBestArgsMSPS <dir_orig_img> <dir_label_img> <count_train_img> <limit_nb_superp> <limit_secs> <optimizing measure 0(BR)/1(UE)> <count_train_samples> <orig_img_ext (ppm:png,...)> <init_nb_block> <init_kmax1> <init_kmax2> <area>","main");

  /*doing MSPS*/
  struct problem *my_problem;
  my_problem=(struct problem *)iftAlloc(1,sizeof(struct problem));

  my_problem->orig_img_path= argv[1];
  my_problem->gt_labels_path=argv[2];
  my_problem->count_train_img=atoi(argv[3]);
  my_problem->limit_nb_superp=atoi(argv[4]);
  my_problem->limit_secs=atoi(argv[5]);
  my_problem->type_measure=atoi(argv[6]);
  my_problem->train_samples=atoi(argv[7]);
  my_problem->orig_img_ext=argv[8];
  my_problem->nb_blocks=atoi(argv[9]);

  iftMSPS* msps = iftCreateMSPS(2, 3, find_best_args,(void *)my_problem);

//  msps->theta[0]=atoi(argv[9]);
  msps->theta[0]=atof(argv[10]);
  msps->theta[1]=atof(argv[11]);
  my_problem->area=atoi(argv[12]);

//  iftMatrixElem(msps->delta, 0, 0) = 6;
//  iftMatrixElem(msps->delta, 0, 1) = 3;
//  iftMatrixElem(msps->delta, 0, 2) = 1;

  iftMatrixElem(msps->delta, 0, 0) = 0.02;
  iftMatrixElem(msps->delta, 0, 1) = 0.005;
  iftMatrixElem(msps->delta, 0, 2) = 0.001;

  iftMatrixElem(msps->delta, 1, 0) = 0.02;
  iftMatrixElem(msps->delta, 1, 1) = 0.005;
  iftMatrixElem(msps->delta, 1, 2) = 0.001;

  msps->verbose=true;
  msps->niters=200;
//  msps->stopcriteria=0.002;
  float best_fun_val=iftMSPSMax(msps);

  printf("Best parameters for %s with %d images an limit number superpixels %d and %d seconds with measure-> %s and train samples %d \n",argv[1],atoi(argv[3]),atoi(argv[4]),atoi(argv[5]),atoi(argv[6])?"ue":"br",atoi(argv[7]));

  printf("best_num_blocks_arg -> %d\n",my_problem->nb_blocks);
  printf("best_train_perc_arg -> %d\n",atoi(argv[7]));
  printf("best_kmax1_arg -> %.4f\n",msps->theta[0]);
  printf("best_kmax2_arg -> %.4f\n",msps->theta[1]);
  printf("best_area_arg -> %d\n",my_problem->area);
  printf("best_objetive_func -> %.4f\n",atoi(argv[6])?-best_fun_val:best_fun_val);

  iftFree(my_problem);
  iftDestroyMSPS(&msps);

  return(0);
}




