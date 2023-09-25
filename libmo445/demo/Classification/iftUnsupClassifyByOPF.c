#include "ift.h"

/* Propagate groups by using a pre-trained unsupervised classifier and
   then rewrite the dataset with the propagated groups (by
   A.X. Falcao, July 2018) */

int main(int argc, char *argv[])
{
  iftDataSet      *Ztest=NULL;
  iftKnnGraph     *graph=NULL;
  timer           *tstart=NULL;
  int              MemDinInicial, MemDinFinal;
	
  if (argc != 3){
    iftError("Usage: iftUnsupClassifyByOPF <input-testset.zip> <input-classifier.zip>", "main");
  }

  MemDinInicial = iftMemoryUsed(1);
  
  iftRandomSeed(time(NULL));

  Ztest = iftReadDataSet(argv[1]);
  iftSetStatus(Ztest,IFT_TEST);
  
  graph = iftReadKnnGraph(argv[2]);
  
  tstart = iftTic();

  iftUnsupClassify(graph,Ztest); 

  iftWriteDataSet(Ztest,argv[1]);
  
  puts("\nDone...");
  puts(iftFormattedTime(iftCompTime(tstart, iftToc())));
  
  iftDestroyKnnGraph(&graph);
  iftDestroyDataSet(&Ztest);

  MemDinFinal = iftMemoryUsed();
  iftVerifyMemory(MemDinInicial, MemDinFinal);

  return(0);
}
