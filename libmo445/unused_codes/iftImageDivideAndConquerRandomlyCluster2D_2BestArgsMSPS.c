#include <ift.h>
#define IMAGE_EXT "ppm"

struct problem{
    char *orig_img_path;
    char *gt_labels_path;
    int count_train_img;
    int limit_nb_superp;
    int limit_secs;
};


float find_best_args(void* my_problem, float* theta) {

  /*check the theta params*/
  if (theta[0]<10000 || theta[0] > 150000 || theta[1] <0.001 || theta[1] > 0.1 || theta[2] <0.001 || theta[2] > 0.07 || theta[3] <0.001 || theta[3] > 0.07 || theta[4] < 0 || theta[4]>400)
    return IFT_INFINITY_FLT;

  char *orig_img_path = ((struct problem *)my_problem)->orig_img_path;
  char *gt_labels_path = ((struct problem *)my_problem)->gt_labels_path;

  iftDir *image_files = iftLoadFilesFromDirBySuffix(orig_img_path, IMAGE_EXT);
  iftDir *gt_label_files = iftLoadFilesFromDirBySuffix(gt_labels_path, "pgm");

  int file_count = ((struct problem *)my_problem)->count_train_img;
  int limit_nb_superp = ((struct problem *)my_problem)->limit_nb_superp;
  int limit_secs = ((struct problem *)my_problem)->limit_secs;

  float ue_acc = 0.0;
  int superpixels_acc = 0;

  iftImage        *img=NULL,*label=NULL,*aux=NULL,*gt_label=NULL;
  iftDataSet      *Z=NULL;
  timer           *t1=NULL,*t2=NULL;
  iftAdjRel *A;
  iftMImage *mimg,*eimg;

  int part_size=(int)theta[0];
  float train_perc_arg=theta[1];
  float kmax1_arg=theta[2];
  float kmax2_arg=theta[3];
  int area_arg=(int)theta[4];

  for (int i=0;i<file_count;i++) {

    img = iftReadImageByExt(image_files->files[i]->path);

    /* convert the image to multi-image*/
    if (!iftIsColorImage(img)) {
      mimg = iftImageToMImage(img, GRAYNorm_CSPACE);
      A  = iftCircular(sqrtf(2.0));
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
    iftDivideAndConquerRandomlyUnsupOPF2(Z, part_size, train_perc_arg, iftNormalizedCut, kmax1_arg, kmax2_arg);
    t2 = iftToc();

    float time = iftCompTime(t1, t2) / 1000;
    /* if the method execution is more than the time specified discard this configuration of parameters*/
    if (time > (float)limit_secs){
      ue_acc =-1;
      break;
    }

    label = iftDataSetToLabelImage(Z,false);

    /* do smoothing in the label image*/
    aux = iftSmoothRegionsByDiffusion(label,img,0.5,5);
    iftDestroyImage(&label);
    label=aux;

    /* eliminate short superpixels*/
    aux = iftSelectAndPropagateRegionsAboveArea(label,area_arg);
    iftDestroyImage(&label);
    label=aux;

    superpixels_acc+=iftMaximumValue(label);

    /*compute ue*/
    gt_label=iftReadImageByExt(gt_label_files->files[i]->path);
    aux=iftRelabelGrayScaleImage(gt_label);
    iftDestroyImage(&gt_label);
    gt_label=aux;
    ue_acc += iftUnderSegmentation(gt_label,label);

    iftDestroyImage(&img);
    iftDestroyMImage(&mimg);
    iftDestroyDataSet(&Z);
    iftDestroyImage(&label);
    iftDestroyImage(&gt_label);

  }

  if (ue_acc >= 0){
    float mean_sup=superpixels_acc/file_count;
    if (mean_sup > (float)limit_nb_superp)
      ue_acc =-1;
    else
      ue_acc /=file_count;
  }

  iftDestroyDir(&image_files);
  iftDestroyDir(&gt_label_files);

  if (ue_acc < 0)
    ue_acc=IFT_INFINITY_FLT;

  return ue_acc;

}

int main(int argc, char *argv[])
{
  iftRandomSeed(IFT_RANDOM_SEED);

  if (argc != 6)
      iftExit("Usage: iftImageDivideAndConquerByBlocksCluster2D_2BestArgs <dir_orig_img> <dir_label_img> <count_train_img> <limit_nb_superp> <limit_secs>",
              "main");

  /*doing MSPS*/
  struct problem *my_problem;
  my_problem=(struct problem *)iftAlloc(1,sizeof(struct problem));

  my_problem->orig_img_path= argv[1];
  my_problem->gt_labels_path=argv[2];
  my_problem->count_train_img=atoi(argv[3]);
  my_problem->limit_nb_superp=atoi(argv[4]);
  my_problem->limit_secs=atoi(argv[5]);

  iftMSPS* msps = iftCreateMSPS(5, 2, find_best_args,(void *)my_problem);

  msps->theta[0] =50000;
  msps->theta[1]=0.04;
  msps->theta[2]=0.005;
  msps->theta[3]=0.005;
  msps->theta[4]=50;

  iftMatrixElem(msps->delta, 0, 0) = 20000;
  iftMatrixElem(msps->delta, 0, 1) = 5000;

  iftMatrixElem(msps->delta, 1, 0) = 0.01;
  iftMatrixElem(msps->delta, 1, 1) = 0.005;

  iftMatrixElem(msps->delta, 2, 0) = 0.01;
  iftMatrixElem(msps->delta, 2, 1) = 0.005;

  iftMatrixElem(msps->delta, 3, 0) = 0.01;
  iftMatrixElem(msps->delta, 3, 1) = 0.005;

  iftMatrixElem(msps->delta, 4, 0) = 50;
  iftMatrixElem(msps->delta, 4, 1) = 15;

  msps->verbose=true;
  msps->iterstar=200;
//  msps->stopcriteria=0.002;
  float best_fun_val=iftMSPSMin(msps);

  printf("Best parameters for %s with %d images an limit number superpixels %d and %d seconds\n",argv[1],atoi(argv[3]),atoi(argv[4]),atoi(argv[5]));

  printf("best_part_size_arg -> %d\n",(int)msps->theta[0]);
  printf("best_train_perc_arg -> %.4f\n",msps->theta[1]);
  printf("best_kmax1_arg -> %.4f\n",msps->theta[2]);
  printf("best_kmax2_arg -> %.4f\n",msps->theta[3]);
  printf("best_area_arg -> %d\n",(int)msps->theta[4]);
  printf("best_objetive_func -> %.4f\n",best_fun_val);

  iftFree(my_problem);
  iftDestroyMSPS(&msps);

  return(0);
}




