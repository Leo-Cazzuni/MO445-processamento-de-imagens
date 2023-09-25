//
// Created by adan on 17/11/17.
//

#include "iftSegmentation.h"

iftImage *iftSmoothRegionsByDiffusionInsideBB(iftImage *label, iftImage *orig, iftImage *block_img,float smooth_factor, int niters)
{
  iftAdjRel *A;

  if (iftIs3DImage(label))
    A = iftSpheric(sqrtf(3.0));
  else
    A = iftCircular(sqrtf(2.0));

  iftImage  *grad      = iftImageGradientMagnitude(orig,A);
  iftFImage *weight    = iftSmoothWeightImage(grad,smooth_factor);
  iftImage  *nlabel    = iftFastSmoothObjectsInsideBB(label,weight,block_img,niters);

  iftDestroyImage(&grad);
  iftDestroyFImage(&weight);
  iftDestroyAdjRel(&A);

  return(nlabel);
}

iftImage *iftFastSmoothObjectsInsideBB(iftImage *label1, iftFImage *weight, iftImage *block_img,int niters) {

  iftAdjRel *A;
  iftImage  *label2;
  iftFImage *flabel1, *flabel2;
  float     *sum, max_membership;
  int       l, i, p, q, max_label, iter;
  iftVoxel  u, v;
  iftSet    *Fprev = NULL, *Fnext = NULL;
  iftBMap   *inFrontier;
  iftFImage *norm_factor;

  if (niters < 1)
    iftExit("Invalid number of iterations", "iftFastSmoothObjects");

  if (label1->zsize == 1) // 2D
    A = iftCircular(sqrtf(2.0));
  else // 3D
    A = iftSpheric(sqrtf(3.0));

  norm_factor = iftWeightNormFactor(weight, A);
  int label1_max_val = iftMaximumValue(label1);
  sum        = iftAllocFloatArray(label1_max_val + 1);
  label2     = iftCopyImage(label1);
  flabel1    = iftCreateFImage(label1->xsize, label1->ysize, label1->zsize);
  flabel2    = iftCreateFImage(label2->xsize, label2->ysize, label2->zsize);
  inFrontier = iftCreateBMap(label1->n);

  for (p = 0; p < label1->n; p++)
    flabel1->val[p] = flabel2->val[p] = 1.0;

  /* Find initial frontier set: this implementation is more
     efficient when the background is large. */

  for (int z = 0; z < label1->zsize; z++)
    for (int y = 0; y < label1->ysize; y++)
      for (int x = 0; x < label1->xsize; x++) {
        u.x=x; u.y=y; u.z=z;
        p = iftGetVoxelIndex(label1, u);
        if (label1->val[p] > 0) {
          for (i = 1; i < A->n; i++) {
            v.x = u.x + A->dx[i];
            v.y = u.y + A->dy[i];
            v.z = u.z + A->dz[i];
            if (iftValidVoxel(label1, v)) {
              q = iftGetVoxelIndex(label1, v);
              if (label1->val[q] != label1->val[p] && block_img->val[p] == block_img->val[q]) {

                if (iftBMapValue(inFrontier, p) == 0) {
                  iftInsertSet(&Fprev, p);
                  iftBMapSet1(inFrontier, p);
                }
                if (iftBMapValue(inFrontier, q) == 0) {
                  iftInsertSet(&Fprev, q);
                  iftBMapSet1(inFrontier, q);
                }
              }
            }
          }
        }
      }

  /* Smooth objects */

  for (iter = 0; iter < niters; iter++) {
    //      printf("Processing iteration %d\n",iter+1);

    while (Fprev != NULL) {

      p = iftRemoveSet(&Fprev);

      iftInsertSet(&Fnext, p);

      u.x = iftGetXCoord(label1, p);
      u.y = iftGetYCoord(label1, p);
      u.z = iftGetZCoord(label1, p);

      for (l = 0; l <= label1_max_val; l++)
        sum[l] = 0.0;

      for (i = 1; i < A->n; i++) {
        v.x = u.x + A->dx[i];
        v.y = u.y + A->dy[i];
        v.z = u.z + A->dz[i];
        if (iftValidVoxel(label1, v) && block_img->val[p] == block_img->val[iftGetVoxelIndex(label1, v)]) {
          q = iftGetVoxelIndex(label1, v);
          sum[label1->val[q]] += flabel1->val[q] * weight->val[q];
          if (iftBMapValue(inFrontier, q) == 0) { // expand frontier set
            iftInsertSet(&Fnext, q);
            iftBMapSet1(inFrontier, q);
          }
        }
      }

      for (l = 0; l <= label1_max_val; l++) {
        sum[l] = sum[l] / norm_factor->val[p];
      }

      max_membership = IFT_INFINITY_FLT_NEG;
      max_label = IFT_NIL;
      for (l = 0; l <= label1_max_val; l++)
        if (sum[l] > max_membership) {
          max_membership = sum[l];
          max_label      = l;
        }

      label2->val[p]  = max_label;
      flabel2->val[p] = sum[max_label];

    }

    Fprev = Fnext;
    Fnext = NULL;

    for (p = 0; p < flabel1->n; p++) {
      flabel1->val[p] = flabel2->val[p];
      label1->val[p]  = label2->val[p];
    }
  }

  iftFree(sum);
  iftDestroyFImage(&flabel1);
  iftDestroyFImage(&flabel2);
  iftDestroyFImage(&norm_factor);
  iftDestroyAdjRel(&A);
  iftDestroySet(&Fprev);
  iftDestroyBMap(&inFrontier);

  iftCopyVoxelSize(label1, label2);

  return (label2);
}

