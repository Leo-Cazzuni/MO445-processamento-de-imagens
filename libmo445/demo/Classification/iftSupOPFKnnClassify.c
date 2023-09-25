#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftDataSet      *Ztest=NULL;
  iftKnnGraph     *graph=NULL;
  size_t           mem_start, mem_end;
  timer           *t1, *t2;

  mem_start = iftMemoryUsed();

  if (argc != 3){
    printf("Usage: iftSupKnnClassify <knngraph.zip> <testSet.zip>\n");
    exit(1);
  }
  
  t1 = iftTic();

  graph = iftReadKnnGraph(argv[1]);
  Ztest = iftReadDataSet(argv[2]);

  iftSupOPFKnnClassify(graph,Ztest);

  iftWriteDataSet(Ztest,argv[2]);
  
  iftDestroyKnnGraph(&graph);
  iftDestroyDataSet(&Ztest);
  
  mem_end = iftMemoryUsed();
  iftVerifyMemory(mem_start,mem_end);
    
  t2 = iftToc();
  puts(iftFormattedTime(iftCompTime(t1,t2)));

  return(0);
}
