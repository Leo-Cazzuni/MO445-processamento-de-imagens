#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftDataSet      *Z=NULL, *Z1[2],*Z2[2];
  iftCplGraph     *graph=NULL;
  timer           *t1=NULL,*t2=NULL;
  float            stdev, mean, *acc;
  int              i, n;

  /*--------------------------------------------------------*/

  int MemDinInicial, MemDinFinal;
  MemDinInicial = iftMemoryUsed();

  /*--------------------------------------------------------*/


  if (argc!=5)
    iftError("Usage: iftDataClassify <dataset.dat> <learn_perc> <train_perc> <niters>","main");

  iftRandomSeed(IFT_RANDOM_SEED);  

  n = atoi(argv[4]);  
  acc  = iftAllocFloatArray(n); 

  Z  = iftReadOPFDataSet(argv[1]);
  printf("Total number of samples %d\n",Z->nsamples);
  printf("Total number of features %d\n",Z->nfeats);
  iftSetDistanceFunction(Z, 1);

  t1     = iftTic();
  
  Z1[0]=Z1[1]=NULL;
  Z2[0]=Z2[1]=NULL;
  
  for (i=0; i < n; i++) {    
    //    usleep(1);

    if (Z1[1] != NULL) iftDestroyDataSet(&Z1[1]);
    if (Z2[1] != NULL) iftDestroyDataSet(&Z2[1]);

    iftSelectSupTrainSamples(Z,atof(argv[2])); // Select a percentage of
                                               // samples for learning 


    Z1[0]= iftExtractSamples(Z,IFT_TRAIN);     // Extract training samples
    iftSetStatus(Z1[0],IFT_TRAIN);
    Z2[0]= iftExtractSamples(Z,IFT_TEST);      // Extract testing  samples

    Z1[1]= iftNormalizeDataSet(Z1[0]);     // Normalize training set
    Z2[1]= iftNormalizeTestDataSet(Z1[1],Z2[0]); // Normalize testing set
    iftDestroyDataSet(&Z1[0]);
    iftDestroyDataSet(&Z2[0]);

    iftSelectSupTrainSamples(Z1[1],atof(argv[3]));
    graph = iftSupLearn(Z1[1]);// Execute the supervised learning 
                                          // with a percentage of training 
                                          // samples

    iftClassify(graph,Z2[1]);                // Classify test samples in Z
    acc[i] = iftSkewedTruePositives(Z2[1]);     // Compute accuracy on test set
    //    printf("acc[%d] = %f\n",i,acc[i]);
    iftDestroyCplGraph(&graph);          // End of traditional OPF 
  }

  t2     = iftToc();

  iftDestroyDataSet(&Z);

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
  
  
  if ((Z2[1]->nfeats == 2)&&(iftDist==NULL)){
    iftImage *img = iftDraw2DFeatureSpace(Z2[1],LABEL,0);
    iftWriteImageP6(img,"labels.ppm");
    iftDestroyImage(&img);
    img = iftDraw2DFeatureSpace(Z2[1],CLASS,0);
    iftWriteImageP6(img,"classes.ppm");
    iftDestroyImage(&img);
    img = iftDraw2DFeatureSpace(Z1[1],STATUS,IFT_TRAIN);
    iftWriteImageP6(img,"train.ppm");
    iftDestroyImage(&img);
    img = iftDraw2DFeatureSpace(Z2[1],WEIGHT,0);
    iftWriteImageP6(img,"weight.ppm");
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




