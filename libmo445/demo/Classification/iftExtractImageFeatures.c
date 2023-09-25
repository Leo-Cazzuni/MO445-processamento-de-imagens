#include "ift.h"

/************************** HEADERS **************************/


int main(int argc, const char *argv[]) {
  iftMImage   *feat;
  iftImage    *img, *label;
  iftAdjRel   *A;
  iftDataSet  *Z;
  size_t       mem_start, mem_end;
  timer       *t1, *t2;

  mem_start = iftMemoryUsed();

  if ((argc != 5)&&(argc !=6)){
    printf("iftExtractImageFeatures <image.*> <label.* (optional)> <adj. radius> <voxel coord. (0/1)> <dataset.zip>\n");
    exit(1);
  }

  t1 = iftTic();
  
  img   = iftReadImageByExt(argv[1]);
  if (argc == 6){
    label = iftReadImageByExt(argv[2]);
    if (iftIs3DImage(img))
      A     = iftSpheric(atof(argv[3]));  
    else
      A     = iftCircular(atof(argv[3]));  

    feat  = iftExtractImageFeatures(img, label, A, atoi(argv[4]));    
    
    Z     = iftMImageToDataSet(feat, label);
    iftSetStatus(Z,IFT_TRAIN);
    
    iftWriteDataSet(Z,argv[5]);
    iftDestroyImage(&label);
  } else {
    if (iftIs3DImage(img))
      A     = iftSpheric(atof(argv[2]));  
    else
      A     = iftCircular(atof(argv[2]));  
    feat  = iftExtractImageFeatures(img, NULL, A, atoi(argv[3]));    
    Z     = iftMImageToDataSet(feat, NULL);
    iftWriteDataSet(Z,argv[4]);
    iftSetStatus(Z,IFT_TRAIN);
  }

  iftDestroyMImage(&feat);
  iftDestroyAdjRel(&A);
  iftDestroyImage(&img);
  iftDestroyDataSet(&Z);
  
  mem_end = iftMemoryUsed();
  iftVerifyMemory(mem_start,mem_end);
    
  t2 = iftToc();
  puts(iftFormattedTime(iftCompTime(t1,t2)));

  return(0);
}
