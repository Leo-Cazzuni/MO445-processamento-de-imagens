#include "ift.h"

/* Classify a dataset by using a pre-trained OPF classifier and add
   the kappa result to a given file (by A.X. Falcao, March 18th
   2018) */

int main(int argc, char *argv[])
{
  iftDataSet      *Ztest=NULL;
  iftCplGraph     *graph=NULL;
  FILE            *fp=NULL;
  timer           *tstart=NULL;
	
  if (argc != 4){
    iftError("Usage: iftClassifyByOPF <...>\n"
	       "[1] input_dataset.zip\n"
	       "[2] input_classifier.zip\n"
         "[3] results.csv\n", "main");
  }

  iftRandomSeed(time(NULL));

  Ztest = iftReadDataSet(argv[1]);
  iftSetStatus(Ztest,IFT_TEST);
  fp    = fopen(argv[3],"a");
  
  graph = iftReadCplGraph(argv[2]);

  printf("- nsamples: %d, nfeats: %d, nclasses: %d\n", Ztest->nsamples, Ztest->nfeats, Ztest->nclasses);
  
  tstart = iftTic();

  printf("classifying ... "); fflush(stdout);
  iftClassify(graph,Ztest);
  printf("OK\n");

  Ztest->nclasses = graph->Z->nclasses;
  float kappa = iftCohenKappaScore(Ztest);
  float truePos = iftTruePositives(Ztest);
  printf("kappa: %f, accuracy: %f\n",kappa,truePos);

  iftFloatArray *TP = iftTruePositivesByClass(Ztest);
  int *nSampPerClass = iftCountSamplesPerClassDataSet(Ztest);

  for (int i=1; i < TP->n; i++) 
    printf("- class %d: %f (%d samples)\n",i,TP->val[i],nSampPerClass[i]);
  iftFree(TP);
  fprintf(fp,"%f;%f\n",kappa,truePos);

  iftDestroyCplGraph(&graph);
  iftDestroyDataSet(&Ztest);

  printf("Done ... %s\n", iftFormattedTime(iftCompTime(tstart, iftToc())));

  return(0);
}




