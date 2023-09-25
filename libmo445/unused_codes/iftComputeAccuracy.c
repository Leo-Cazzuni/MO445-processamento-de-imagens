#include "ift.h"

int main(int argc, char *argv[])
{

  if (argc !=3 )
    iftError("Usage: iftComputeAccuracy <label_image_dir> <gt_image_dir>","main");

  iftDir *label_files = iftLoadFilesFromDirBySuffix(argv[1], "pgm");
  iftDir *gt_region_files = iftLoadFilesFromDirBySuffix(argv[2],"pgm");

  int file_count=label_files->nfiles;

  float *acc_vector=iftAllocFloatArray(file_count);

#pragma omp parallel for
  for (int i=0;i<file_count;i++){
    iftImage *label=iftReadImageByExt(label_files->files[i]->path);
    iftImage *gt=iftReadImageByExt(gt_region_files->files[i]->path);

    int ncorrectsamples=0;
    for (int p=0;p<label->n;p++)
      if (label->val[p] == gt->val[p])
        ncorrectsamples++;

    acc_vector[i]=(float)ncorrectsamples/label->n;

    iftDestroyImage(&label);
    iftDestroyImage(&gt);
  }

  printf("%.4f %.4f\n",iftMean(acc_vector,file_count),iftStddevFloatArray(acc_vector,file_count));

  iftFree(acc_vector);
  iftDestroyDir(&label_files);
  iftDestroyDir(&gt_region_files);

  return(0);
}
