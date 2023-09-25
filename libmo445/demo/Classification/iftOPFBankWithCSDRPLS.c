#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftDataSet      *Z=NULL,*Z1[2],*Z2[2];
  iftCSDRPLS      *csdr=NULL;
  iftOPFBank      *opfbank=NULL;
  timer           *t1=NULL,*t2=NULL;
  float            stdev, mean, *acc;
  int              i, n;

  /*--------------------------------------------------------*/

  int MemDinInicial, MemDinFinal;
  MemDinInicial = iftMemoryUsed();

  /*--------------------------------------------------------*/


  if (argc!=5)
    iftError("Usage: iftOPFBankWithCSDRPLS <dataset.dat> <train_perc> <niters> <num_of_comps>","main");

  iftRandomSeed(IFT_RANDOM_SEED);  

  n = atoi(argv[3]);  
  acc  = iftAllocFloatArray(n); 

  /* Initialization */

  Z  = iftReadOPFDataSet(argv[1]); // Read dataset Z
  printf("Total number of samples  %d\n",Z->nsamples);
  printf("Total number of features %d\n",Z->nfeats);
  iftSetDistanceFunction(Z, 1);


  t1     = iftTic();
  
  Z1[0]=Z1[1]=Z2[0]=Z2[1]=NULL;

  for (i=0; i < n; i++) {    
    //    usleep(5);

    if (Z1[1] != NULL) iftDestroyDataSet(&Z1[1]);
    if (Z2[1] != NULL) iftDestroyDataSet(&Z2[1]);

    iftSelectSupTrainSamples(Z,atof(argv[2])); // Select training samples
                                               // Training

    Z1[0]  = iftExtractSamples(Z,IFT_TRAIN);
    Z2[0]  = iftExtractSamples(Z,IFT_TEST);

    Z1[1] = iftNormalizeDataSet(Z1[0]); // Normalize 


    Z2[1] = iftNormalizeTestDataSet(Z1[1],Z2[0]);
    
    iftDestroyDataSet(&Z1[0]);
    iftDestroyDataSet(&Z2[0]);


    csdr     = iftCreateCSDRPLS(Z1[1],atoi(argv[4]));


    opfbank  = iftTrainOPFBankWithCSDRPLS(Z1[1],csdr);
    
    iftClassifyByOPFBankWithCSDRPLS(opfbank,csdr,Z2[1]);

    acc[i] = iftSkewedTruePositives(Z2[1]);  // Compute accuracy on test set
    printf("acc[%d] = %f\n",i,acc[i]);

    iftDestroyCSDRPLS(&csdr);               
    iftDestroyOPFBank(&opfbank);
  }

  iftDestroyDataSet(&Z);

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
  
  if (Z2[1]->nfeats == 2){
    iftImage *img = iftDraw2DFeatureSpace(Z2[1],LABEL,0);
    iftWriteImageP6(img,"labels.ppm");
    iftDestroyImage(&img);
    img = iftDraw2DFeatureSpace(Z2[1],CLASS,0);
    iftWriteImageP6(img,"classes.ppm");
    iftDestroyImage(&img);
    img = iftDraw2DFeatureSpace(Z1[1],STATUS,IFT_TRAIN);
    iftWriteImageP6(img,"train.ppm");
    iftDestroyImage(&img);
  }

  iftDestroyDataSet(&Z1[1]);
  iftDestroyDataSet(&Z2[1]);
  
  /* ---------------------------------------------------------- */

  MemDinFinal = iftMemoryUsed();
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}


