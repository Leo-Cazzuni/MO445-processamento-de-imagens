#include "ift.h"

/* Train a supervised OPF classifier from a given dataset and save it
   for future use (by A.X. Falcao, March 18th 2018) */

int main(int argc, char *argv[])
{
  iftDataSet      *Ztrain=NULL;
  iftCplGraph     *graph=NULL;
  timer           *tstart=NULL;
	
  if (argc != 3){
    iftError("Usage: iftSupTrainByOPF <...>\n"
	       "[1] input_dataset.zip\n"
	       "[2] output_classifier.zip\n", "main");
  }

  iftRandomSeed(time(NULL));

  Ztrain = iftReadDataSet(argv[1]); 
  iftAddStatus(Ztrain,IFT_TRAIN);
	   
  tstart = iftTic();

  printf("- nsamples: %d, nfeats: %d, nclasses: %d\n", Ztrain->nsamples, Ztrain->nfeats, Ztrain->nclasses);

  printf("training ... "); fflush(stdout);
  graph  = iftCreateCplGraph(Ztrain); // Create complete graph 
  iftSupTrain(graph); // Train a supervised OPF classifier
  //iftSupTrain2(graph); // Train a supervised OPF classifier
  printf("OK\n");

  iftWriteCplGraph(graph,argv[2]);
  iftDestroyCplGraph(&graph);
  iftDestroyDataSet(&Ztrain);

  printf("Done ... %s\n", iftFormattedTime(iftCompTime(tstart, iftToc())));

  return(0);
}
