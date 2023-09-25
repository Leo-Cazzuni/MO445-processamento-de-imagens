#include <ift.h>

double find_best_args(iftDict* problem, iftDict* params) {

  char *orig_img_path = iftGetStrValFromDict("orig_img_path", problem);
  char *gt_labels_path = iftGetStrValFromDict("gt_labels_path", problem);
  char *orig_img_ext= iftGetStrValFromDict("orig_img_ext", problem);

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

  int file_count = iftGetLongValFromDict("count_train_img", problem);
  int limit_nb_sup = iftGetLongValFromDict("limit_nb_sup", problem);
  int limit_secs = iftGetLongValFromDict("limit_secs", problem);
  int type_measure = iftGetLongValFromDict("type_measure", problem);

  float measure_acc = 0.0;
  int sup_acc = 0;

  iftImage *img=NULL,*label=NULL,*aux=NULL,*border=NULL,*gt_label=NULL,*gt_border=NULL;

  float kernel_size=iftGetDblValFromDict("kernel_size",params);
  float max_dist=iftGetDblValFromDict("max_dist",params);
  int area_arg=iftGetLongValFromDict("area_arg",problem);
  bool exited=false;
  FILE *fpipe;
  char command[200];
  int nb_superpixels=0;
  float time=0;

  for (int i=0;i<file_count && !exited;i++) {

    sprintf(command,"/usr/bin/python quickshift.py %s %.2f %.2f",image_files->files[i]->path,kernel_size,max_dist);

    if ( !(fpipe = popen(command, "r")) )
    {   // If fpipe is NULL
      perror("Problems with pipe");
      return 1;
    }

    fscanf(fpipe,"%f %d",&time,&nb_superpixels);
    pclose(fpipe);

    img=iftReadImageByExt(image_files->files[i]->path);
    gt_label=iftReadImageByExt(gt_label_files->files[i]->path);

//    printf("Nb superpixels -> %d, time %.2f\n",nb_superpixels,time);

    label = iftReadImageByExt("labels.pgm");

    aux=label;
    label=iftRelabelGrayScaleImage(aux,1);
    iftDestroyImage(&aux);

    /* do smoothing*/
//    aux=label;
//    label = iftSmoothRegionsByDiffusion(aux,img,0.5,2);
//    iftDestroyImage(&aux);

    aux = iftSelectAndPropagateRegionsAboveAreaByColor(img,label,area_arg);
    iftDestroyImage(&label);
    label=aux;

    nb_superpixels=iftMaximumValue(label);

    if (nb_superpixels > 2*limit_nb_sup || time > (float)limit_secs){
      measure_acc =IFT_INFINITY_FLT_NEG;
      exited=true;
    }
    else{
      sup_acc +=nb_superpixels;

      aux=iftRelabelGrayScaleImage(gt_label,0);
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
    }

    iftDestroyImage(&label);
    iftDestroyImage(&gt_label);
    iftDestroyImage(&img);
  }

  if (!exited){
    int mean_sup = sup_acc / file_count;
    if (mean_sup > limit_nb_sup)
      measure_acc =IFT_INFINITY_FLT_NEG;
    else
      measure_acc /=file_count;
  }

  iftDestroyDir(&image_files);
  iftDestroyDir(&gt_label_files);

  printf("\nWith kernel_size = %.5f and max_dist = %.5f ->fun_obj = %.5f\n",kernel_size,max_dist,measure_acc);
  fflush(stdout);

  return measure_acc;

}

int main(int argc, char *argv[])
{
  iftRandomSeed(IFT_RANDOM_SEED);

  if (argc != 15)
    iftError("Usage: iftClusterImageByQuickShiftBestArgsGS <dir_orig_img> <dir_label_img> <count_train_img> <limit_nb_superp> <limit_secs> <optimizing_measure(0:BR/1:UE)> <orig_img_ext(ppm:png,...)> <init_kernel_size> <final_kernel_size> <step_kernel_size> <init_max_dist> <final_max_dist> <step_max_dist> <area>","main");

  iftDict* params = iftCreateDict();
  iftDict* problem = iftCreateDict();

  iftInsertIntoDict("kernel_size",iftRange(atof(argv[8]), atof(argv[9]), atof(argv[10])),params);
  iftInsertIntoDict("max_dist", iftRange(atof(argv[11]),atof(argv[12]), atof(argv[13])),params);

  iftInsertIntoDict("orig_img_ext",argv[7],problem);
  iftInsertIntoDict("orig_img_path", argv[1],problem);
  iftInsertIntoDict("gt_labels_path", argv[2],problem);
  iftInsertIntoDict("count_train_img", atoi(argv[3]),problem);
  iftInsertIntoDict("area_arg", atoi(argv[14]),problem);
  iftInsertIntoDict("limit_nb_sup", atoi(argv[4]),problem);
  iftInsertIntoDict("limit_secs", atoi(argv[5]),problem);
  iftInsertIntoDict("type_measure", atoi(argv[6]),problem);

  iftDict *result =iftGridSearch(params, find_best_args, problem);

  printf("Best parameters for %s with %d images an limit number superpixels %d and %d seconds with measure-> %s and train samples by block %d\n",argv[1],atoi(argv[3]),atoi(argv[4]),atoi(argv[5]),atoi(argv[6])?"ue":"br",atoi(argv[7]));

  printf("best_kernel_size -> %.4f\n",(float)iftGetDblValFromDict("kernel_size",result));
  printf("best_max_dist -> %.4f\n",(float)iftGetDblValFromDict("max_dist",result));
  float res=(float)iftGetDblValFromDict("best_func_val",result);
  printf("best_objetive_func -> %.4f\n",atoi(argv[6])?-res:res);

  iftDestroyDict(&params);
  iftDestroyDict(&problem);
  iftDestroyDict(&result);

  return(0);
}
