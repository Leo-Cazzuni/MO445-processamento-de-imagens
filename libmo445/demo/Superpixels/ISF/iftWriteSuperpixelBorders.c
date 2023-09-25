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

  // Print accuracies by acc_by_class
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

int main(int argc, char *argv[]) 
{
  iftImage  *img, *label;
  iftImage  *gt_borders=NULL, *gt_regions=NULL, *border=NULL, *border_without_margins=NULL;
  iftAdjRel *A,*B;
  int        nseeds;
  char       labelfilename[256];
  char       borderfilename[256];
  iftColor     RGB, YCbCr;

  /*--------------------------------------------------------*/

  int MemDinInicial, MemDinFinal;
  MemDinInicial = iftMemoryUsed();

  /*--------------------------------------------------------*/

  if (argc<4 || argc>6 )
      iftError(
              "Usage: iftWriteSuperpixelBorders <image.[pgm,ppm,scn]> <input_label> <output_border> <ground-truth border image (OPTIONAL)> <ground-truth region image (OPTIONAL)>",
              "main");
  
  img  = iftReadImageByExt(argv[1]);
  label  = iftReadImageByExt(argv[2]);

  if (iftIs3DImage(img)){
    B = iftSpheric(1.0);
    A = iftSpheric(sqrtf(3.0));
  } else {
    B = iftCircular(1.0);
    A = iftCircular(sqrtf(2.0));  
  }

  if (argc > 4){
    gt_borders = iftReadImageByExt(argv[4]);
    gt_regions = iftReadImageByExt(argv[5]);
  }

  printf("Read images OK\n");
  border = iftBorderImage(label,1);
  nseeds  = iftMaximumValue(label);
  printf("Border OK\n");
  iftDestroyAdjRel(&A);
  A = iftCircular(0);

  // Compute metrics
  if (argc>4){
    float br, ue, comp;
    br        = iftBoundaryRecall(gt_borders, border, 2.0);
    //printf("BR: %.2f \n", br);
    printf("BR: %f \n", br);

    ue       = iftUnderSegmentation(gt_regions, label);
    printf("UE: %f \n", ue);

    comp     = iftCompactness2D(label);
    printf("Comp: %f \n", comp);

    float topology = iftTopologyMeasure(label);
    printf("Top: %f \n", topology);

    //float asa = iftAchievableSegmentationAccuracy(gt_regions, label);
    //printf("ASA: %f \n", asa);
    
  }
  printf("Writing\n");

  border_without_margins  = iftBorderImageWithoutMargins(label);

  // Write output image
  if (!iftIs3DImage(img)){
    if (argc > 4){
      RGB.val[0] = 255; // 255
      RGB.val[1] = 0; // 0
      RGB.val[2] = 255; // 255
      YCbCr      = iftRGBtoYCbCr(RGB, 255);
      iftDrawBorders(img,gt_borders,A,YCbCr,A);
      printf("write gt borders\n");
    }
    RGB.val[0] = 0; // 255
    RGB.val[1] = 255; // 0
    RGB.val[2] = 255; // 0
    YCbCr      = iftRGBtoYCbCr(RGB, 255);
    //sprintf(labelfilename, "%s.pgm", argv[3]);
    //iftWriteImageP2(label,labelfilename);
    iftDrawBorders(img,border_without_margins,A,YCbCr,A);
    sprintf(borderfilename, "%s.ppm", argv[3]);        
    iftWriteImageP6(img, borderfilename);
  } else {
    sprintf(labelfilename, "%s.scn", argv[3]);
    iftWriteImage(label,labelfilename);
    int maxval = iftMaximumValue(img);
    RGB.val[0] = maxval;
    RGB.val[1] = maxval;
    RGB.val[2] = maxval;
    YCbCr      = iftRGBtoYCbCr(RGB, maxval);
    iftDrawBorders(img,border_without_margins,A,YCbCr,A);
    sprintf(borderfilename, "%s-border.scn", argv[3]);
    iftWriteImage(img,borderfilename);
  }

  // Print number of iterations and superpixels
  printf("Number of superpixels: %d \n", nseeds);


  // Free
  iftDestroyImage(&img);
  if (argc>=4) {
    iftDestroyImage(&gt_borders);
    iftDestroyImage(&gt_regions);
  }
  iftDestroyImage(&label);
  iftDestroyImage(&border);
  iftDestroyImage(&border_without_margins);
  iftDestroyAdjRel(&A);
  iftDestroyAdjRel(&B);

  /* ---------------------------------------------------------- */

  MemDinFinal = iftMemoryUsed();
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}

