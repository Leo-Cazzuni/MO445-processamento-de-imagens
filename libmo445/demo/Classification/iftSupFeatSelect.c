#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftDataSet      *Z=NULL, *Z1[2],*Z2[2];
  iftCplGraph     *graph=NULL;
  timer           *t1=NULL,*t2=NULL;
  float            mean1, stdev1, mean2, stdev2, *acc1, *acc2;
  int              i, n;

  /*--------------------------------------------------------*/

  int MemDinInicial, MemDinFinal;
  MemDinInicial = iftMemoryUsed();

  /*--------------------------------------------------------*/


  if (argc!=4)
    iftError("Usage: iftSupFeatSelect <dataset.dat> <learn_perc> <train_perc>","main");
  
  t1     = iftTic();

  iftRandomSeed(IFT_RANDOM_SEED);

  n = 100;  
  acc1  = iftAllocFloatArray(n); 
  acc2  = iftAllocFloatArray(n); 

  Z  = iftReadOPFDataSet(argv[1]); // Read dataset Z
  iftSetDistanceFunction(Z, 2);

  Z1[0]=Z1[1]=Z2[0]=Z2[1]=NULL;

  for (i=0; i < n; i++) {    
    
    if (Z1[1] != NULL) iftDestroyDataSet(&Z1[1]);
    if (Z2[1] != NULL) iftDestroyDataSet(&Z2[1]);

    iftSelectSupTrainSamples(Z,atof(argv[2])); // Select a percentage of
                                               // samples for learning 

    Z1[0] = iftExtractSamples(Z,IFT_TRAIN);    // Extract the learning samples
    Z2[0] = iftExtractSamples(Z,IFT_TEST);     // Extract the testing samples

    Z1[1] = iftNormalizeDataSet(Z1[0]); // Normalize training set
    Z2[1] = iftNormalizeTestDataSet(Z1[1],Z2[0]); // Normalize testing set

    iftDestroyDataSet(&Z1[0]);
    iftDestroyDataSet(&Z2[0]);

    iftSelectSupTrainSamples(Z1[1],atof(argv[3]));
    graph = iftSupLearn(Z1[1]);// Execute the supervised learning
                                          // with a percentage of training
                                          // samples

    iftClassify(graph,Z2[1]);                // Classify test samples in Z
    acc1[i] = iftSkewedTruePositives(Z2[1]);     // Compute accuracy on test set
    printf("acc1 %f\n",acc1[i]);
    iftDestroyCplGraph(&graph);          // End of traditional OPF
    
    iftSupFeatSelection(Z1[1],atof(argv[3]),2*Z1[1]->nfeats+1); // Execute Supervised
                                               // feature selection with a
                                               // percentage of training
                                               // samples.
 
    iftSelectSupTrainSamples(Z1[1],atof(argv[3]));
    graph = iftSupLearn(Z1[1]);// Execute the supervised learning 
    // with a percentage of training 
    // samples
    iftClassify(graph,Z2[1]);
    acc2[i] = iftSkewedTruePositives(Z2[1]);
    printf("acc2 %f\n",acc2[i]);
    iftDestroyCplGraph(&graph);
  }

  iftDestroyDataSet(&Z);

  t2     = iftToc();

  fprintf(stdout,"classifications in %f ms\n",iftCompTime(t1,t2));
 
  mean1 = mean2 = 0.0;
  for (i=0; i < n; i++) {
    mean1 += acc1[i];
    mean2 += acc2[i];
  }
  mean1 /= n;
  mean2 /= n;
  stdev1 = 0.0;
  stdev2 = 0.0;

  for (i=0; i < n; i++) {
    stdev1 += (acc1[i]-mean1)*(acc1[i]-mean1);
    stdev2 += (acc2[i]-mean2)*(acc2[i]-mean2);;
  }
  stdev1 = sqrtf(stdev1)/n;
  stdev2 = sqrtf(stdev2)/n;

  free(acc1); 
  free(acc2); 

  fprintf(stdout,"Accuracy of classification is mean1=%f, stdev1=%f mean2=%f, stdev2=%f\n",mean1,stdev1,mean2,stdev2); 
  
  
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




