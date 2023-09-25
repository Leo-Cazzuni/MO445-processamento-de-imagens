#include "ift.h"



int main(int argc, char *argv[]) 
{
  iftDataSet      *Z=NULL,*Z1=NULL;
  timer           *t1=NULL,*t2=NULL;
  float            stdev, mean, *acc;
  int              i, n;

  /*--------------------------------------------------------*/

  int MemDinInicial, MemDinFinal;
  MemDinInicial = iftMemoryUsed();

  /*--------------------------------------------------------*/


  if (argc!=6)
      iftError("Usage: iftDataClassifyByKNN <dataset.dat> <train_perc> <niters> <distance_table> <k>", "main");
  
  iftRandomSeed(IFT_RANDOM_SEED);

  n    = atoi(argv[3]);  
  acc  = iftAllocFloatArray(n); 

  /* Initialization */

  Z  = iftReadOPFDataSet(argv[1]); // Read dataset Z
  printf("Total number of samples  %d\n",Z->nsamples);
  printf("Total number of features %d\n",Z->nfeats);
  iftDist = iftReadDistanceTable(argv[4]);

  t1     = iftTic();

  for (i=0; i < n; i++) {    
    //    usleep(5);
    iftSelectSupTrainSamples(Z,atof(argv[2])); // Select training samples
    Z1 = iftExtractSamples(Z,IFT_TRAIN);
    iftClassifyByKNN(Z1,Z,atoi(argv[5]));                      // Classify test samples 
    acc[i] = iftTruePositives(Z);            // Compute accuracy on test set

    printf("acc[%d] = %f\n",i,acc[i]);
    iftDestroyDataSet(&Z1);   
  }

  t2     = iftToc();

  fprintf(stdout,"classifications in %f ms\n",iftCompTime(t1,t2));
 
  mean = 0.0;
  for (i=0; i < n; i++) {
    mean += acc[i];
  }
  mean /= n;
  stdev = 0.0;

  for (i=0; i < n; i++) {
    stdev += (acc[i]-mean)*(acc[i]-mean);
  }
  if (n > 1)
    stdev = sqrtf(stdev/(n-1));

  free(acc); 

  fprintf(stdout,"Accuracy of classification is mean=%f, stdev=%f\n",mean,stdev); 

  if (Z->nfeats == 2) {
    iftImage *img = iftDraw2DFeatureSpace(Z,LABEL,0);
    iftWriteImageP6(img,"labels.ppm");
    iftDestroyImage(&img);
    img = iftDraw2DFeatureSpace(Z,CLASS,0);
    iftWriteImageP6(img,"classes.ppm");
    iftDestroyImage(&img);
    img = iftDraw2DFeatureSpace(Z,STATUS,IFT_TRAIN);
    iftWriteImageP6(img,"train.ppm");
    iftDestroyImage(&img);
  }


  iftDestroyDataSet(&Z);
  iftDestroyDistanceTable(&iftDist);
  
  /* ---------------------------------------------------------- */

  MemDinFinal = iftMemoryUsed();
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}




