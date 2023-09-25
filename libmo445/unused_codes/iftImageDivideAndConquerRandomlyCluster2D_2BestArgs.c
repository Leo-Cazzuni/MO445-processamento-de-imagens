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

  float br_acc = 0.0;
  int superpixels_acc = 0;

  iftImage        *img=NULL,*label=NULL,*aux=NULL,*border=NULL,*gt_label=NULL,*gt_border=NULL;
  iftDataSet      *Z=NULL;
  timer           *t1=NULL,*t2=NULL;
  iftAdjRel *A;
  iftMImage *mimg,*eimg;

  int part_size=(int)iftGetDblValFromDict("part_size_arg",params);
  float train_perc_arg=(float)iftGetDblValFromDict("train_perc_arg",params);
  float kmax1_arg=(float)iftGetDblValFromDict("kmax1_arg",params);
  float kmax2_arg=(float)iftGetDblValFromDict("kmax2_arg",params);
  int area_arg=(int)iftGetDblValFromDict("area_arg",params);

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
    int ngroups= iftDivideAndConquerRandomlyUnsupOPF2(Z, part_size, train_perc_arg, iftNormalizedCut, kmax1_arg, kmax2_arg);
    t2 = iftToc();

    float time_array=iftCompTime(t1,t2)/1000;
    /* if the method execution is more than the time specified discard this configuration of parameters*/
    if (time_array > (float)limit_secs){
      br_acc=-1;
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

    /*compute boundary recall*/
    border  = iftBorderImageWithoutMargins(label);
    gt_label=iftReadImageByExt(gt_label_files->files[i]->path);
    aux=iftRelabelGrayScaleImage(gt_label);
    iftDestroyImage(&gt_label);
    gt_label=aux;
    gt_border=iftBorderImageWithoutMargins(gt_label);
    br_acc+= iftBoundaryRecall(gt_border, border, 2.0);

    iftDestroyImage(&img);
    iftDestroyMImage(&mimg);
    iftDestroyDataSet(&Z);
    iftDestroyImage(&label);
    iftDestroyImage(&border);
    iftDestroyImage(&gt_border);
    iftDestroyImage(&gt_label);

  }

  if (br_acc > 0){
    float mean_sup=superpixels_acc/file_count;
    if (mean_sup > (float)limit_nb_superp)
      br_acc=-1;
    else
      br_acc/=file_count;
  }

  iftDestroyDir(&image_files);
  iftDestroyDir(&gt_label_files);

//  printf("br: %.3f\n",br_acc);

  return br_acc;


}

int main(int argc, char *argv[])
{
  iftRandomSeed(IFT_RANDOM_SEED);

  if (argc != 6)
      iftExit("Usage: iftImageDivideAndConquerByBlocksCluster2D_2BestArgs <dir_orig_img> <dir_label_img> <count_train_img> <limit_nb_superp> <limit_secs>",
              "main");

  iftDict* params = iftCreateDict();
  iftDict* problem = iftCreateDict();

  iftInsertIntoDict("part_size_arg", iftRange(10000, 50000, 20000),params);
  iftInsertIntoDict("train_perc_arg", iftRange(0.005, 0.05, 0.02),params);
  iftInsertIntoDict("kmax1_arg", iftRange(0.005, 0.05, 0.015),params);
  iftInsertIntoDict("kmax2_arg", iftRange(0.005, 0.05, 0.015),params);
  iftInsertIntoDict("area_arg", iftRange(0, 200, 50),params);

  iftInsertIntoDict("orig_img_path", argv[1],problem);
  iftInsertIntoDict("gt_labels_path", argv[2],problem);
  iftInsertIntoDict("count_train_img", atoi(argv[3]),problem);
  iftInsertIntoDict("limit_nb_superp", atoi(argv[4]),problem);
  iftInsertIntoDict("limit_secs", atoi(argv[5]),problem);

  printf("Best parameters for %s with %d images an limit number superpixels %d and %d seconds\n",argv[1],atoi(argv[3]),atoi(argv[4]),atoi(argv[5]));

  iftDict *result =iftGridSearch(params, find_best_args, problem);

  printf("best_part_size_arg -> %d\n",(int)iftGetDblValFromDict("part_size_arg",result));
  printf("best_train_perc_arg -> %.4f\n",(float)iftGetDblValFromDict("train_perc_arg",result));
  printf("best_kmax1_arg -> %.4f\n",(float)iftGetDblValFromDict("kmax1_arg",result));
  printf("best_kmax2_arg -> %.4f\n",(float)iftGetDblValFromDict("kmax2_arg",result));
  printf("best_area_arg -> %d\n",(int)iftGetDblValFromDict("area_arg",result));
  printf("best_objetive_func -> %.4f\n",(float)iftGetDblValFromDict("best_func_val",result));

  iftDestroyDict(&params);
  iftDestroyDict(&problem);
  iftDestroyDict(&result);

  return(0);
}




