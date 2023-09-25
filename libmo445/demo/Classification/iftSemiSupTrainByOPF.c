#include "ift.h"

/* Train a semi-supervised OPF classifier from a given dataset and save it
   for future use (by A.X. Falcao, July 18th 2018) */

int main(int argc, char *argv[])
{
  iftDataSet      *Ztrain=NULL;
  iftCplGraph     *graph=NULL;
  timer           *tstart=NULL;
  int              MemDinInicial, MemDinFinal;
	
  if (argc != 3){
    iftExit("Usage: iftSemiSupTrainByOPF <input-trainingset.zip> <output-classifier.zip>", "main");
  }

  MemDinInicial = iftMemoryUsed(1);
 
  iftRandomSeed(time(NULL));

  Ztrain = iftReadDataSet(argv[1]);   
  
  /* ---------------- */
  
  printf("Total number of samples  %d\n",Ztrain->nsamples);
  printf("Total number of features %d\n",Ztrain->nfeats);
  printf("Total number of classes  %d\n",Ztrain->nclasses);
  
  tstart = iftTic();

  graph  = iftSemiSupTrain(Ztrain);
  
  puts("\nDone...");
  puts(iftFormattedTime(iftCompTime(tstart, iftToc())));

  iftWriteDataSet(Ztrain,argv[1]);
  iftWriteCplGraph(graph,argv[2]);
  iftDestroyCplGraph(&graph);
  iftDestroyDataSet(&Ztrain);

  MemDinFinal = iftMemoryUsed();
  iftVerifyMemory(MemDinInicial, MemDinFinal);

  return(0);
}
