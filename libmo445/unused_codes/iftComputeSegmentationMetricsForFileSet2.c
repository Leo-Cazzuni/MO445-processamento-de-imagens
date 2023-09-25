#include "ift.h"

float iftAchievableSegmentationAccuracy(iftImage *orig_gt, iftImage *orig_label)
{
  iftImage *label, *gt;
  int nobjs, nsuperpixels, ncorrect_pixels=0, npixels_best_obj, index_best_obj;
  int min_val_superpixels, min_val_gt, is_min_gt_0 = 0, is_min_label_0 = 0;
  float acc;
  // Number of pixel in the intersection superpixel gt_object (rows: superpixels, columns: objects)
  iftMatrix *mat_npixels;
  int *npixels_object_gt, *npixels_object_intersect;

  assert(orig_label->n == orig_label->n);

  // Get min values
  min_val_superpixels = iftMinimumValue(orig_label);
  min_val_gt = iftMinimumValue(orig_gt);
  // Set minimum value of gt and label to 1
  if (min_val_gt == 0) {
    gt = iftAddValue(orig_gt, 1);
    is_min_gt_0 = 1;
  } else {
    gt = orig_gt;
  }
  if (min_val_superpixels == 0) {
    label = iftAddValue(orig_label, 1);
    is_min_label_0 = 1;
  } else {
    label = orig_label;
  }

  // Alloc matrix to count number of pixels in the intersection (superpixels, gt_object)
  nobjs = iftMaximumValue(gt);
  nsuperpixels = iftMaximumValue(label);
  mat_npixels = iftCreateMatrix(nobjs, nsuperpixels);
  npixels_object_gt = iftAllocIntArray(nobjs);
  npixels_object_intersect = iftAllocIntArray(nobjs);

  // Fill intersection matrix
  for (int p = 0; p < label->n; ++p) {
    int index_sup = label->val[p]-1;
    int index_obj = gt->val[p]-1;
    iftMatrixElem(mat_npixels, index_obj, index_sup) += 1;
    npixels_object_gt[gt->val[p]-1]++;
  }
  // Compute accuracy
  for (int i = 0; i < nsuperpixels; ++i)
  {
    npixels_best_obj = -1;
    index_best_obj = 0;
    for (int j = 0; j < nobjs; ++j)
    {
      if (npixels_best_obj < iftMatrixElem(mat_npixels, j, i)) {
        npixels_best_obj = iftMatrixElem(mat_npixels, j, i);
        index_best_obj = j;
      }
    }
    npixels_object_intersect[index_best_obj] += npixels_best_obj;
    ncorrect_pixels += npixels_best_obj;
  }
  acc = (float)ncorrect_pixels / (float)label->n;

  // Free
  if (is_min_gt_0) iftDestroyImage(&gt);
  if (is_min_label_0) iftDestroyImage(&label);
  iftDestroyMatrix(&mat_npixels);
  iftFree(npixels_object_intersect);
  iftFree(npixels_object_gt);
  return acc;
}

int main(int argc, char *argv[])
{
  if (argc != 7 )
    iftExit("Usage: iftComputeSegmentationMetricsForFileSet2 <image_dir> <input_label_dir> <gt_region_image_dir> "
                    "<ext_img_dir> <nb_train_images> <area>",
            "main");

  char *orig_img_ext=argv[4];
  iftDir* image_files = iftLoadFilesFromDirBySuffix(argv[1], orig_img_ext);

  char *gt_img_ext="";
  if (!strcmp(orig_img_ext,"png") || !strcmp(orig_img_ext,"PNG") || !strcmp(orig_img_ext,"ppm") || !strcmp(orig_img_ext,"PPM") || !strcmp(orig_img_ext,"pgm") || !strcmp(orig_img_ext,"PGM"))
    gt_img_ext="pgm";
  else
  if (!strcmp(orig_img_ext,"scn"))
    gt_img_ext="scn";
  else
    iftExit("Invalid extension for orig image directory", "find_best_args");

  iftDir* label_files = iftLoadFilesFromDirBySuffix(argv[2], gt_img_ext);
  int file_count=label_files->nfiles;

  iftDir* gt_region_files;

  float* br_array;
  float* ue_array;
  float* comp_array;
  float* asa_array;
  float* nseeds=iftAllocFloatArray(file_count);

  gt_region_files = iftLoadFilesFromDirBySuffix(argv[3],gt_img_ext);

  br_array=iftAllocFloatArray(file_count);
  ue_array=iftAllocFloatArray(file_count);
  comp_array=iftAllocFloatArray(file_count);
  asa_array=iftAllocFloatArray(file_count);

  int area=atof(argv[6]);

  struct stat st = {0};
  char new_dir[50];
  sprintf(new_dir,"%sregions/",argv[2]);

  if (stat(new_dir, &st) == -1) {
    mkdir(new_dir, 0777);
  }

#pragma omp parallel for
  for (int i=0;i<file_count;i++){

    iftImage  *img, *label, *aux;
    iftImage  *gt_borders=NULL, *gt_regions=NULL, *border=NULL;

    img  = iftReadImageByExt(image_files->files[i]->path);
    label  = iftReadImageByExt(label_files->files[i]->path);
    aux=iftRelabelGrayScaleImage(label,1);
    iftDestroyImage(&label);
    label=aux;

    iftAdjRel *A;
    if (!iftIs3DImage(img))
      A=iftCircular(1.5);
    else
      A=iftSpheric(1.0);

    /* put as background all pixels with value less than 20, this is specifically for the liver dataset*/
    if (!iftIsColorImage(img)){
      for (int p=0;p<img->n;p++){
        label->val[p]=(img->val[p] <= 20)?1:label->val[p]+1;
      }
    }

    aux=iftRelabelRegions(label,A);
    iftDestroyImage(&label);
    label=aux;

    aux=iftSelectAndPropagateRegionsAboveArea(label,area);
    iftDestroyImage(&label);
    label=aux;

    int norm_value = iftNormalizationValue(iftMaximumValue(img));
    iftAdjRel *B  = iftCircular(0.0);
    iftColor RGB,YCbCr;
    RGB.val[0] = 0;
    RGB.val[1] = norm_value;
    RGB.val[2] = norm_value;
    YCbCr      = iftRGBtoYCbCr(RGB,norm_value);
    aux        = iftCopyImage(img);
    iftDrawBorders(aux,label,A,YCbCr,B);

    char regions_filename[100];
    sprintf(regions_filename,"%s%d.ppm",new_dir,i+1);

    iftWriteImageP6(aux,regions_filename);

    iftDestroyImage(&aux);
    iftDestroyAdjRel(&A);
    iftDestroyAdjRel(&B);


    gt_regions = iftReadImageByExt(gt_region_files->files[i]->path);
    aux=iftRelabelGrayScaleImage(gt_regions,0);
    iftDestroyImage(&gt_regions);
    gt_regions=aux;

    gt_borders = iftBorderImage(gt_regions,0);

    border  = iftBorderImage(label,0);
    nseeds[i]  = (float)iftMaximumValue(label);

    // Compute metrics
    br_array[i]= iftBoundaryRecall(gt_borders, border, 2.0);
    ue_array[i] = iftUnderSegmentation(gt_regions, label);
    if (img->zsize == 1)
      comp_array[i] = iftCompactness2D(label);
    asa_array[i] = iftAchievableSegmentationAccuracy(gt_regions, label);

    // Free
    iftDestroyImage(&img);
    iftDestroyImage(&gt_borders);
    iftDestroyImage(&gt_regions);
    iftDestroyImage(&label);
    iftDestroyImage(&border);
  }

  printf("Mean_ASA\tVar_ASA\tMean_BR\tVar_BR\tMean_UE\tVar_UE\tMean_COMP\tVar_COMP\tMean_SuperNb\n");

  printf("---------------Metrics in all images-------------\n");
  printf("%.4f\t%.4f\t%.4f\t%.4f\t%.4f\t%.4f\t%.4f\t%.4f\t%d\n",iftMean(asa_array,file_count),iftVar(asa_array,
                                                                                                     file_count),iftMean(br_array,file_count),iftVar(br_array,file_count),iftMean(ue_array,file_count),iftVar(ue_array,file_count),iftMean(comp_array,
                                                                                                                                                                                                                                           file_count),iftVar (comp_array,file_count),(int)iftMean(nseeds,file_count));

  int train_images=atoi(argv[5]);

  printf("---------------Metrics in train images-------------\n");
  printf("%.4f\t%.4f\t%.4f\t%.4f\t%.4f\t%.4f\t%.4f\t%.4f\t%d\n",iftMean(asa_array,train_images),iftVar(asa_array,
                                                                                                       train_images),iftMean(br_array,train_images),iftVar(br_array,train_images),iftMean(ue_array,train_images),iftVar(ue_array,train_images),iftMean(comp_array,
                                                                                                                                                                                                                                                       train_images),iftVar (comp_array,train_images),(int)iftMean(nseeds,train_images));

  printf("---------------Metrics in test images-------------\n");
  printf("%.4f\t%.4f\t%.4f\t%.4f\t%.4f\t%.4f\t%.4f\t%.4f\t%d\n",iftMean(asa_array+train_images,file_count-train_images),iftVar
                 (asa_array+train_images,
                  file_count-train_images),iftMean(br_array+train_images,file_count-train_images),iftVar(br_array+train_images,file_count-train_images),iftMean
                 (ue_array+train_images,
                  file_count-train_images),iftVar
                 (ue_array+train_images,
                  file_count-train_images),
         iftMean(comp_array+train_images,
                 file_count-train_images),iftVar (comp_array+train_images,file_count-train_images),(int)iftMean(nseeds+train_images,file_count-train_images));


  for (int i=0;i<file_count;i++){
    printf("%s %.3f %.3f %.3f %.3f %d\n",image_files->files[i]->path,asa_array[i],br_array[i],ue_array[i],
           comp_array[i], (int)nseeds[i]);
  }

  iftFree(asa_array);
  iftFree(br_array);
  iftFree(ue_array);
  iftFree(comp_array);
  iftFree(nseeds);
  iftDestroyDir(&image_files);
  iftDestroyDir(&label_files);
  iftDestroyDir(&gt_region_files);

  return(0);
}

