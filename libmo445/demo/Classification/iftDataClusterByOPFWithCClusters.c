#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftDataSet      *Z=NULL;
  iftKnnGraph     *graph=NULL;
  iftImage        *img=NULL;
  timer           *t1=NULL,*t2=NULL;

  /*--------------------------------------------------------*/

  int MemDinInicial, MemDinFinal;
  MemDinInicial = iftMemoryUsed();
  int c; // number of clusters	

  /*--------------------------------------------------------*/

  if (argc != 4)
      iftError("Usage: iftDataClusterByOPF <dataset.dat> <kmax_perc [0,1)> <number of clusters>","main");

  iftRandomSeed(IFT_RANDOM_SEED);
 
  Z = iftReadOPFDataSet(argv[1]); 
  iftSetStatus(Z,IFT_TRAIN);
  printf("Z: (nsamples, nfeats): %d, %d\n", Z->nsamples, Z->nfeats);
 
  t1      = iftTic();
  graph = iftCreateKnnGraph(Z,(atof(argv[2])*Z->nsamples)); 
  c = atoi(argv[3]);	
  iftUnsupTrainWithCClusters(graph,c);
  t2     = iftToc();

  fprintf(stdout,"clustering in %f ms with %d groups\n",iftCompTime(t1,t2),Z->nlabels);
  iftPrintNumberOfSamplesPerCluster(Z);

  if (Z->nfeats == 2) {

    img = iftDraw2DFeatureSpace(Z,LABEL,0);
    iftWriteImageP6(img,"labels.ppm");
    iftDestroyImage(&img);
    
    img = iftDraw2DFeatureSpace(Z,STATUS,IFT_TRAIN);
    iftWriteImageP6(img,"train.ppm");
    iftDestroyImage(&img);

    
    img = iftDraw2DFeatureSpace(Z,CLASS,0);
    iftWriteImageP6(img,"classes.ppm");
    iftDestroyImage(&img);
    

    img = iftDraw2DFeatureSpace(Z,WEIGHT,0);
    iftWriteImageP6(img,"weight.ppm");
    iftDestroyImage(&img);
  }

  iftDestroyDataSet(&Z);
  iftDestroyKnnGraph(&graph);

  /* ---------------------------------------------------------- */

  MemDinFinal = iftMemoryUsed();
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}
