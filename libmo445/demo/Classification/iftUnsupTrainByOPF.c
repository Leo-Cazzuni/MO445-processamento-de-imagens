#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftDataSet      *Ztrain=NULL;
  iftKnnGraph     *graph=NULL;
  int              kmax;
  size_t           mem_start, mem_end;
  timer           *t1, *t2;

  mem_start = iftMemoryUsed();

  if (argc != 4){
    printf("Usage: iftUnsupTrainByOPF <trainSet.zip> < kmax (number/percentage of samples)> <knngraph.zip>\n");
    exit(1);
  }
  
  t1 = iftTic();

  Ztrain = iftReadDataSet(argv[1]);
  iftAddStatus(Ztrain,IFT_TRAIN);
  
  if (atof(argv[2]) > 1.0)
    kmax=iftMin(atoi(argv[2]),Ztrain->nsamples/2);
  else
    kmax=iftMax(atof(argv[2])*Ztrain->nsamples,1);   

  /* iftMST *mst = iftCreateMST(Ztrain); */
  /* graph = iftMSTtoKnnGraph(mst,kmax); */
  /* iftDestroyMST(&mst); */
  graph = iftCreateKnnGraph(Ztrain,kmax);

  iftUnsupTrain(graph,iftNormalizedCut);
  
  iftWriteKnnGraph(graph,argv[3]);
  iftWriteDataSet(Ztrain,argv[1]);
  
  printf("Number of groups: %d\n",Ztrain->ngroups);
  
  iftDestroyKnnGraph(&graph);
  iftDestroyDataSet(&Ztrain);
  
  mem_end = iftMemoryUsed();
  iftVerifyMemory(mem_start,mem_end);
    
  t2 = iftToc();
  puts(iftFormattedTime(iftCompTime(t1,t2)));

  return(0);
}
