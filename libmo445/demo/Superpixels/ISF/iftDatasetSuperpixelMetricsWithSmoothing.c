#include "ift.h"

iftImage* iftSmoothSuperpixels(iftImage *img, iftImage *label, int num_smooth_iterations)
{
  iftImage  *prev_label, *next_label, *grad;
  iftFImage *prev_weight, *next_weight, *norm_factor, *weight;
  float     *sum, max_membership;
  int        l, i, p, q, r, max_label, iter, max_prev_label;
  iftVoxel   u, v;  
  iftAdjRel *A, *B;
  iftSet    *prev_frontier = NULL, *next_frontier = NULL, *S = NULL;
  iftBMap   *inFrontier;

  if (iftIs3DImage(img)) {
    A = iftSpheric(1.0);
    B = iftSpheric(sqrtf(3.0));
  } else {
    A = iftCircular(1.0);
    B = iftCircular(sqrtf(2.0));
  }

  /* Initialization */
  
  prev_label = iftCopyImage(label);
  grad      = iftImageGradientMagnitude(img,B);  
  weight    = iftSmoothWeightImage(grad,0.5);

  next_label  = iftCopyImage(prev_label);
  norm_factor = iftWeightNormFactor(weight,A);
  inFrontier  = iftCreateBMap(prev_label->n);

  max_prev_label = iftMaximumValue(prev_label);
  sum         = iftAllocFloatArray(max_prev_label + 1);
  prev_weight = iftCreateFImage(prev_label->xsize, prev_label->ysize, prev_label->zsize);
  next_weight = iftCreateFImage(next_label->xsize, next_label->ysize, next_label->zsize);
  prev_frontier = iftObjectBorderSet(prev_label, A);

  for (p = 0; p < prev_label->n; p++){
    prev_weight->val[p] = next_weight->val[p] = 1.0;
  }

  S = prev_frontier;
  while (S != NULL) {
    p = S->elem;
    iftBMapSet1(inFrontier,p);
    S = S->next;
  }

  /* Smooth frontier and reset its path values */

  for (iter = 0; iter < num_smooth_iterations; iter++)
    {      
      while (prev_frontier != NULL)
        {
    p = iftRemoveSet(&prev_frontier);
    iftInsertSet(&next_frontier, p);
    u   = iftGetVoxelCoord(prev_label, p);

    for (l = 0; l <= max_prev_label; l++)
            {
        sum[l] = 0.0;
            }
    
    for (i = 1; i < A->n; i++)
            {
        v = iftGetAdjacentVoxel(A, u, i);
        if (iftValidVoxel(prev_label, v))
                {
      q = iftGetVoxelIndex(prev_label, v);
      sum[prev_label->val[q]] += prev_weight->val[q] * weight->val[q];
      if (iftBMapValue(inFrontier, q) == 0) /* expand frontier */
                    {
          //if (igraph->pred[q] != NIL)
          //              {
        iftInsertSet(&next_frontier, q);
        iftBMapSet1(inFrontier, q);
          //              }
                    }
                }
            }

    for (l = 0; l <= max_prev_label; l++)
      sum[l]  = sum[l] / norm_factor->val[p];

    max_membership = IFT_INFINITY_FLT_NEG; max_label = IFT_NIL;
    for (l = 0; l <= max_prev_label; l++)
            {
        if (sum[l] > max_membership)
                {
      max_membership = sum[l];
      max_label      = l;
                }
            }
    next_label->val[p]  = max_label;
    next_weight->val[p] = sum[max_label];
        }
      
      prev_frontier = next_frontier;
      next_frontier = NULL;
      
      for (r = 0; r < prev_label->n; r++)
        {
    prev_weight->val[r] = next_weight->val[r];
    prev_label->val[r]  = next_label->val[r];
        }
    }

    free(sum);
    iftDestroyFImage(&prev_weight);
    iftDestroyFImage(&next_weight);
    iftDestroyImage(&next_label);
    iftDestroyFImage(&weight);
    iftDestroyFImage(&norm_factor);
    iftDestroyBMap(&inFrontier);
    iftDestroySet(&prev_frontier);
    iftDestroyImage(&grad);

    /* It fixes the label map, by eliminating the smallest regions and
       relabel them with the adjacent labels */

    iftMaximumValue(prev_label);
    next_label = iftSelectKLargestRegionsAndPropagateTheirLabels(prev_label, A, max_prev_label);
    
    iftDestroyImage(&prev_label);
    iftDestroyAdjRel(&A);
    iftDestroyAdjRel(&B);
    return next_label;
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

void iftSuperpixelMetricsWithSmoothing(char *img_path, char *superpixel_path, char *dir_border, char *dir_label, float *br, float *ue, float *comp, float *topology, int niters_smooth) {
  iftImage  *orig_label=NULL, *label, *gt_regions, *gt_borders, *gt_borders_or, *border, *img;
  char fullpath_label[256];
  char fullpath_border[256];
  char imagename[256];
  //char imagename_ext[256];

  int ngts, p;
  float uegt, tolerance_br;

  img = iftReadImageByExt(img_path);
  orig_label = iftReadImageByExt(superpixel_path);
  if (niters_smooth > 0) {
    label = iftSmoothSuperpixels(img, orig_label, niters_smooth);
    iftDestroyImage(&orig_label);
  } else {
    label = orig_label;
  }

  //strcpy(imagename_ext, iftBasename(superpixel_path, NULL));
  //sscanf(imagename_ext,"%[^.]",imagename);
  strcpy(imagename, iftFilename(img_path, ".ppm"));

  tolerance_br = 2.0;
  sprintf(fullpath_label, "%s%s.pgm", dir_label, imagename);
  sprintf(fullpath_border, "%s%s.pgm", dir_border, imagename);

  printf("imagename %s\n", imagename);

  border     = iftBorderImage(label);

  // Compute metrics for the created superpixels
  if (iftFileExists(fullpath_label)) {
    gt_regions = iftReadImageByExt(fullpath_label);
    gt_borders = iftReadImageByExt(fullpath_border);

    // Compute boundary recall and Undersegmentation error
    *br = iftBoundaryRecall(gt_borders, border, tolerance_br);
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
  iftDestroyImage(&img);
}


int main(int argc, char *argv[]) 
{
  if(argc!=7)
  {
        iftError("Usage: <images path> <superpixel images path> <gt borders path> <gt regions path> <niters_smooth> <output_csv>", "DatasetSuperpixelMetris");
        return 1;
  }
  
  FILE *f1;

  iftDir* dir_images = iftLoadFilesFromDirBySuffix(argv[1], "ppm");
  iftDir* dir_superpixels = iftLoadFilesFromDirBySuffix(argv[2], "pgm");
  int i, nimages;
  float meanBR, meanUE, meanComp, meanTop, meanTotal, sdBR, sdUE, sdComp, sdTop, sdTotal, br, ue, comp, top;
  nimages = dir_superpixels->nfiles;
  f1 = fopen(argv[6], "w");

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
    printf("Processing image: %s\n", dir_images->files[i]->path);
    printf("Processing superpixel: %s\n", dir_superpixels->files[i]->path);
    iftSuperpixelMetricsWithSmoothing(dir_images->files[i]->path, dir_superpixels->files[i]->path, argv[3], argv[4], &(br), &(ue), &(comp), &(top), atoi(argv[5]));

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
  
  iftDestroyDir(&dir_superpixels);
  iftDestroyDir(&dir_images);

  
  return(0);
}

