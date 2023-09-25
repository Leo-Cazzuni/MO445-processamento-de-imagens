#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftDataSet      *Ztrain=NULL;
  iftKnnGraph     *graph=NULL;
  int              kmax;
  size_t           mem_start, mem_end;
  timer           *t1, *t2;

  mem_start = iftMemoryUsed();

  if (argc != 5){
    printf("Usage: iftSupOPFKnnTrain <trainSet.zip> < kmax (number/percentage of samples)> <knngraph.zip> <labels.zip>\n");
    exit(1);
  }
  
  t1 = iftTic();

  Ztrain = iftReadDataSet(argv[1]);

  if (Ztrain->nclasses == 0)
    iftExit("There are no classes","iftSupOPFKnnTrain");
  
  if (atof(argv[2]) > 1.0)
    kmax=iftMin(atoi(argv[2]),Ztrain->nsamples/2);
  else
    kmax=iftMax(atof(argv[2])*Ztrain->nsamples,1);   

  iftSetStatus(Ztrain,IFT_TRAIN); // it must be the only status
  graph = iftCreateKnnGraph(Ztrain,kmax);



  iftFastUnsupTrain(graph,iftNormalizedCut);

  iftPropagateClusterTrueLabels(graph);
  
  iftWriteKnnGraph(graph,argv[3]);
  iftWriteDataSet(Ztrain,argv[4]);
  
  printf("Number of groups: %d\n",Ztrain->ngroups);
  
  iftDestroyKnnGraph(&graph);
  iftDestroyDataSet(&Ztrain);
  
  mem_end = iftMemoryUsed();
  iftVerifyMemory(mem_start,mem_end);
    
  t2 = iftToc();
  puts(iftFormattedTime(iftCompTime(t1,t2)));

  return(0);
}
