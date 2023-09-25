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

  // Print accuracies by class
  /*
  for (int i = 0; i < nobjs; ++i)
  {
    float acc_by_class = (float)npixels_object_intersect[i] / (float)npixels_object_gt[i];
    printf("ASA class %d : %f \n", i+1, acc_by_class);
  }
  */
  // Free
  if (is_min_gt_0) iftDestroyImage(&gt);
  if (is_min_label_0) iftDestroyImage(&label);
  iftDestroyMatrix(&mat_npixels);
  return acc;
}

void iftSingleUserSuperpixelMetrics(char *path_superpixel, char *border_path, char *label_path, float *br, float *ue, float *comp, float *topology) {
  iftImage  *label, *gt_regions, *gt_borders, *border;
  float tolerance_br;
  label = iftReadImageByExt(path_superpixel);

  tolerance_br = 2.0;

  border     = iftBorderImage(label);
  // Compute metrics for the created superpixels
  gt_regions = iftReadImageByExt(label_path);
  gt_borders = iftReadImageByExt(border_path);

  // Compute boundary recall and Undersegmentation error
  *br = iftBoundaryRecall(gt_borders, border, tolerance_br);
  *ue = iftUnderSegmentation(gt_regions, label);
  
  // Compute compactness and topology
  *comp = iftCompactness2D(label);
  *topology = iftTopologyMeasure(label);

  iftDestroyImage(&gt_regions);
  iftDestroyImage(&gt_borders);
  iftDestroyImage(&border);
}

void iftSuperpixelMetrics(char *superpixel_path, char *dir_border, char *dir_label, float *br, float *ue, float *comp, float *topology) {
  iftImage  *label, *gt_regions, *gt_borders, *gt_borders_or, *border;
  char fullpath_label[256];
  char fullpath_border[256];
  char imagename[256];
  //char imagename_ext[256];

  int ngts, p;
  float uegt, tolerance_br;

  label = iftReadImageByExt(superpixel_path);

  //strcpy(imagename_ext, iftBasename(superpixel_path, NULL));
  //sscanf(imagename_ext,"%[^.]",imagename);
  strcpy(imagename, iftFilename(superpixel_path, ".pgm"));

  tolerance_br = 2.0;
  sprintf(fullpath_label, "%s%s.pgm", dir_label, imagename);
  sprintf(fullpath_border, "%s%s.pgm", dir_border, imagename);

  border     = iftBorderImage(label);

  // Compute metrics for the created superpixels
  if (iftFileExists(fullpath_label)) {
    gt_regions = iftReadImageByExt(fullpath_label);
    gt_borders = iftReadImageByExt(fullpath_border);
    

    // Compute boundary recall and Undersegmentation error
    *br = iftBoundaryRecall(gt_borders, border, tolerance_br);
    //*br = iftAchievableSegmentationAccuracy(gt_regions, label);
    *ue = iftUnderSegmentation(gt_regions, label);
    iftDestroyImage(&gt_regions);
    iftDestroyImage(&gt_borders);
  } else {
    gt_borders_or = iftCreateImage(label->xsize, label->ysize, label->zsize);
    uegt = 0.0;
    // Compute OR image for GT and
    ngts = 1;
    sprintf(fullpath_label, "%s%s_%d.pgm", dir_label, imagename, ngts);
    sprintf(fullpath_border, "%s%s_%d.pgm", dir_border, imagename, ngts);
    while( iftFileExists(fullpath_label) ){
      gt_regions = iftReadImageP5(fullpath_label);
      gt_borders = iftReadImageP5(fullpath_border);
      for(p=0; p < gt_borders_or->n; p++) {
        if (gt_borders->val[p] > 0) {
          gt_borders_or->val[p] = gt_borders->val[p];
        }
      }
      ngts++;
      // Compute undersegmentation error
      uegt += iftUnderSegmentation(gt_regions, label);
      // Set new GT filename
      sprintf(fullpath_label, "%s%s_%d.pgm", dir_label, imagename, ngts);
      sprintf(fullpath_border, "%s%s_%d.pgm", dir_border, imagename, ngts);
      // Destroy GT image
      iftDestroyImage(&gt_regions);
      iftDestroyImage(&gt_borders);

    }
    ngts = ngts - 1;
    // Compute mean of undersegmentation errors
    *ue = uegt / (float)ngts;
    // Compute boundary recall
    *br = iftBoundaryRecall(gt_borders_or, border, tolerance_br);

    iftDestroyImage(&gt_borders_or);
  }  
  // Compute compactness and topology
  *comp = iftCompactness2D(label);
  *topology = iftTopologyMeasure(label);

  iftDestroyImage(&border);
}


int main(int argc, char *argv[]) 
{
  if(argc!=5)
  {
        iftError("Usage: <superpixel images path> <gt borders path> <gt regions path> <output_csv>", "DatasetSuperpixelMetris");
        return 1;
  }
  
  FILE *f1;

  iftDir* datasetfiles = iftLoadFilesFromDirBySuffix(argv[1], "pgm");
  int i, nimages;
  float meanBR, meanUE, meanComp, meanTop, meanTotal, sdBR, sdUE, sdComp, sdTop, sdTotal, br, ue, comp, top;
  nimages = datasetfiles->nfiles;
  f1 = fopen(argv[4], "w");

  float *boundaryRecall = iftAllocFloatArray(nimages);
  float *undersegError = iftAllocFloatArray(nimages);
  float *compactness = iftAllocFloatArray(nimages);
  float *topology = iftAllocFloatArray(nimages);
  float *totalMeasure = iftAllocFloatArray(nimages);
  meanBR = 0.0;
  meanUE = 0.0;
  meanComp = 0.0;
  meanTop = 0.0;
  meanTotal = 0.0;
  
  sdBR = 0.0;
  sdUE = 0.0;
  sdComp = 0.0;
  sdTop = 0.0;
  sdTotal = 0.0;


  for (i = 0; i < nimages; i++) {
    iftSuperpixelMetrics(datasetfiles->files[i]->path, argv[2], argv[3], &(br), &(ue), &(comp), &(top));

    boundaryRecall[i] = br;
    undersegError[i] = ue;
    compactness[i] = comp;
    topology[i] = top;
    totalMeasure[i] = (2*boundaryRecall[i]*compactness[i])/(boundaryRecall[i]+compactness[i]);//*topology[i];
    
    printf("BR[%d]: %f \n", i, boundaryRecall[i]);
    printf("Comp[%d]: %f \n", i, compactness[i]);
    printf("Top[%d]: %f \n", i, topology[i]);
    printf("UE[%d]: %f \n", i, undersegError[i]);
    printf("Total[%d]: %f \n", i, totalMeasure[i]);
    meanBR += boundaryRecall[i];
    meanUE += undersegError[i];
    meanComp += compactness[i]; 
    meanTop += topology[i]; 
    meanTotal += totalMeasure[i];

    // Write results
    fprintf(f1,"%f, %f, %f, %f\n", boundaryRecall[i], compactness[i], topology[i], undersegError[i]);
    
  }

  meanBR /= (float) nimages;
  meanUE /= (float) nimages;
  meanComp /= (float) nimages;
  meanTop /= (float) nimages;
  meanTotal /= (float) nimages;

  for (i = 0; i < nimages; i++) {
    sdBR += (meanBR - boundaryRecall[i]) * (meanBR - boundaryRecall[i]);
    sdUE += (meanUE - undersegError[i]) * (meanUE - undersegError[i]);
    sdComp += (meanComp - compactness[i]) * (meanComp - compactness[i]);
    sdTop += (meanTop - topology[i]) * (meanTop - topology[i]);
    sdTotal += (meanTotal - totalMeasure[i]) * (meanTotal - totalMeasure[i]);
  }
  sdBR /= (float)(nimages -1);
  sdBR = sqrtf(sdBR);
  sdUE /= (float)(nimages -1);
  sdUE = sqrtf(sdUE);
  sdComp /= (float)(nimages -1);
  sdComp = sqrtf(sdComp);
  sdTop /= (float)(nimages -1);
  sdTop = sqrtf(sdTop);
  sdTotal /= (float)(nimages -1);
  sdTotal = sqrtf(sdTotal);

  // Print results
  printf("Mean BR: %f    , SD BR: %f \n", meanBR, sdBR);
  printf("Mean Comp: %f    , SD Comp: %f \n", meanComp, sdComp);
  printf("Mean Top: %f    , SD Top: %f \n", meanTop, sdTop);
  printf("Mean UE: %f    , SD UE: %f \n", meanUE, sdUE);
  printf("Mean Total: %f    , SD Total: %f \n", meanTotal, sdTotal);
  printf("Results: %f, %f, %f, %f, %f, %f, %f, %f, %f, %f  \n", meanBR, sdBR, meanComp, sdComp, meanTop, sdTop, meanUE, sdUE, meanTotal, sdTotal);

  // Write results in csv
  fprintf(f1,"%f, %f, %f, %f\n", meanBR, meanComp, meanTop, meanUE);
  fprintf(f1,"%f, %f, %f, %f", sdBR, sdComp, sdTop, sdUE);
  fclose(f1);

  // Free
  free(boundaryRecall);
  free(undersegError);
  free(compactness);
  free(topology);
  free(totalMeasure);
  
  iftDestroyDir(&datasetfiles);
  
  return(0);
}

