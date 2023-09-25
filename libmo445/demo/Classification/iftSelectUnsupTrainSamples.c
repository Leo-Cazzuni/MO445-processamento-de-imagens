#include "ift.h"

/************************** HEADERS **************************/

int main(int argc, const char *argv[]) {
  iftDataSet *Z = NULL, *Ztrain = NULL;
  size_t mem_start, mem_end;
  timer *t1, *t2;

  mem_start = iftMemoryUsed();

  if (argc != 4){
    printf("iftSelectUnsupTrainSamples <inputDataset.zip> <num. of train. samples> <trainSet.zip>\n");
    exit(1);
  }
  
  t1 = iftTic();
  
  Z     = iftReadDataSet(argv[1]);

  if (atoi(argv[2]) > Z->nsamples)
    iftError("Number of training samples cannot exceed the number %d of samples","iftSelectUnsupTrainSamples",Z->nsamples);
  
  iftRandomSeed(time(NULL));
  iftSelectUnsupTrainSamples(Z, atof(argv[2])/Z->nsamples, 10);

  Ztrain  = iftExtractSamples(Z,IFT_TRAIN);
  
  iftWriteDataSet(Ztrain,argv[3]);

  iftDestroyDataSet(&Z);
  iftDestroyDataSet(&Ztrain);
  
  mem_end = iftMemoryUsed();
  iftVerifyMemory(mem_start,mem_end);
    
  t2 = iftToc();
  puts(iftFormattedTime(iftCompTime(t1,t2)));

  return 0;
}
