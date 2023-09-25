#include <ift.h>
#define IMAGE_EXT "ppm"

iftImage *iftBorderImageWithoutMargins(iftImage *label)
{
  iftAdjRel *A;
  iftImage  *border = iftCreateImage(label->xsize,label->ysize,label->zsize);
  int        p,q,i;
  iftVoxel   u, v;

  if (iftIs3DImage(label))
    A = iftSpheric(1.0);
  else
    A = iftCircular(1.0);

  for(p=0; p < label->n; p++){
    u = iftGetVoxelCoord(label, p);
    for(i=1; i < A->n; i++){
      v = iftGetAdjacentVoxel(A,u,i);
      if (iftValidVoxel(label, v)){
        q = iftGetVoxelIndex(label, v);
        if (label->val[p] != label->val[q]){
          border->val[p] = label->val[p];
          break;
        }
      }
    }
  }

  iftDestroyAdjRel(&A);
  return(border);
}

double find_best_args(iftDict* problem, iftDict* params) {

  char *orig_img_path = iftGetStrValFromDict("orig_img_path", problem);
  char *gt_labels_path = iftGetStrValFromDict("gt_labels_path", problem);

  iftDir *image_files = iftLoadFilesFromDirBySuffix(orig_img_path, IMAGE_EXT);
  iftDir *gt_label_files = iftLoadFilesFromDirBySuffix(gt_labels_path, "pgm");

  int file_count = iftGetLongValFromDict("count_train_img", problem);
  int limit_nb_superp = iftGetLongValFromDict("limit_nb_superp", problem);
  int limit_secs = iftGetLongValFromDict("limit_secs", problem);
  int type_measure = iftGetLongValFromDict("type_measure", problem);

  float measure_acc = 0.0;
  int superpixels_acc = 0;

  iftImage        *img=NULL,*label=NULL,*aux=NULL,*border=NULL,*gt_label=NULL,*gt_border=NULL;
  iftDataSet      *Z=NULL;
  timer           *t1=NULL,*t2=NULL;
  iftAdjRel *A;
  iftMImage *mimg,*eimg;

  int nb_blocks=(int)iftGetDblValFromDict("num_blocks_arg",params);
  float kmax1_arg=iftGetDblValFromDict("kmax1_arg",params);
  float kmax2_arg=iftGetDblValFromDict("kmax2_arg",params);
  int area_arg=(int)iftGetDblValFromDict("area_arg",params);
  bool exited=false;

  for (int i=0;i<file_count;i++) {

    img = iftReadImageByExt(image_files->files[i]->path);

    /* convert the image to multi-image*/
    if (!iftIsColorImage(img)) {
      A = iftCircular(1.5);
      mimg = iftImageToMImage(img, GRAYNorm_CSPACE);
      eimg = iftExtendMImageByAdjacencyAndVoxelCoord(mimg, A, 1);
      iftDestroyAdjRel(&A);
      iftDestroyMImage(&mimg);
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
    int ngroups= iftImageDivideAndConquerByBlocksUnsupOPF(Z, nb_blocks, iftNormalizedCut, kmax1_arg,kmax2_arg);
    t2 = iftToc();

    float time_array=iftCompTime(t1,t2)/1000;
    /* if the method execution is more than the time specified discard this configuration of parameters*/
    if (time_array > (float)limit_secs){
      measure_acc =IFT_INFINITY_FLT_NEG;
      exited=true;
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
      border  = iftBorderImageWithoutMargins(label);
      gt_border=iftBorderImageWithoutMargins(gt_label);
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

  if (!exited) {
    float mean_sup = superpixels_acc / file_count;
    if (mean_sup > (float) limit_nb_superp)
      measure_acc =IFT_INFINITY_FLT_NEG;
    else
      measure_acc /= file_count;
  }

  iftDestroyDir(&image_files);
  iftDestroyDir(&gt_label_files);

  return measure_acc;

}

int main(int argc, char *argv[])
{
  iftRandomSeed(IFT_RANDOM_SEED);

  if (argc != 7)
      iftExit("Usage: iftImageDivideAndConquerByBlocksCluster2D_2BestArgs <dir_orig_img> <dir_label_img> <count_train_img> <limit_nb_superp> <limit_secs> <optimizing measure 0(BR)/1(UE)>",
              "main");

  iftDict* params = iftCreateDict();
  iftDict* problem = iftCreateDict();

  iftInsertIntoDict("num_blocks_arg", iftRange(80, 100, 10),params);
  iftInsertIntoDict("kmax1_arg", iftRange(0.01, 0.03, 0.01),params);
  iftInsertIntoDict("kmax2_arg", iftRange(0.005, 0.015, 0.005),params);
  iftInsertIntoDict("area_arg", iftRange(0, 100, 50),params);

  iftInsertIntoDict("orig_img_path", argv[1],problem);
  iftInsertIntoDict("gt_labels_path", argv[2],problem);
  iftInsertIntoDict("count_train_img", atoi(argv[3]),problem);
  iftInsertIntoDict("limit_nb_superp", atoi(argv[4]),problem);
  iftInsertIntoDict("limit_secs", atoi(argv[5]),problem);
  iftInsertIntoDict("type_measure", atoi(argv[6]),problem);

  iftDict *result =iftGridSearch(params, find_best_args, problem);

  printf("Best parameters for %s with %d images an limit number superpixels %d and %d seconds with measure-> %s\n",argv[1],atoi(argv[3]),atoi(argv[4]),atoi(argv[5]),atoi(argv[6])?"ue":"br");

  printf("best_num_blocks_arg -> %d\n",(int)iftGetDblValFromDict("num_blocks_arg",result));
  printf("best_kmax1_arg -> %.4f\n",(float)iftGetDblValFromDict("kmax1_arg",result));
  printf("best_kmax2_arg -> %.4f\n",(float)iftGetDblValFromDict("kmax2_arg",result));
  printf("best_area_arg -> %d\n",(int)iftGetDblValFromDict("area_arg",result));
  float res=(float)iftGetDblValFromDict("best_func_val",result);
  printf("best_objetive_func -> %.4f\n",atoi(argv[6])?-res:res);

  iftDestroyDict(&params);
  iftDestroyDict(&problem);
  iftDestroyDict(&result);

  return(0);
}




