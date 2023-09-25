#include "ift.h"
float* iftGridSearchSVM(iftDataSet* Z, float percTrainVal, int nsplits){

	iftDataSet *Ztrain, *Ztest,*Ztmp;
	iftSVM      *svm   = NULL;
  iftSampler      *sampler=NULL;
	int i,j,it, split;
	float C, sigma;
	float max_acc = 0;
  float max_kappa = 0;
  float mean, stdev;
  float max_stdev = 0;
	float *values, *kappa;
  iftIntArray     *labels=NULL;
	values = iftAllocFloatArray(2);
	float sigmas[5], Cs[5];

	sigmas[0] = 0.010000;
	sigmas[1] = 0.001000;
	sigmas[2] = 0.000100;
	sigmas[3] = 0.000010;
	sigmas[4] = 0.000001;
	Cs[0] = 1;
	Cs[1] = 10;
	Cs[2] = 100;
	Cs[3] = 1000;
	Cs[4] = 10000;
	C = 100000;
	sigma = 0.1;

	for(i=0; i<5; i++){
		for(j=0; j<5; j++){
			float acc = 0;
      kappa = iftAllocFloatArray(nsplits);	

      Ztmp = iftCopyDataSet(Z, true);
      labels = iftGetDataSetTrueLabels(Ztmp);
      sampler = iftStratifiedRandomSubsampling(labels->val, labels->n, nsplits, percTrainVal*Ztmp->nsamples);
      //sampler = iftRandomSubsampling(Ztmp->nsamples, 1, percTrainVal*Ztmp->nsamples);

      for(split=0; split<nsplits; split++)
      {
        iftDataSetSampling(Ztmp, sampler, split);
        Ztrain = iftExtractSamples(Ztmp,IFT_TRAIN);
        Ztest  = iftExtractSamples(Ztmp,IFT_TEST);


        /* Train RBF Kernel */
        svm = iftCreateRBFSVC(Cs[i], sigmas[j]);
        iftSVMTrain(svm,Ztrain);

        /* Classify RBF Kernel */
        iftSVMClassify(svm, Ztest, IFT_TEST); // Classification
        acc += iftTruePositives(Ztest) / (float) nsplits;
        kappa[split]= iftCohenKappaScore(Ztest);

        iftDestroyDataSet(&Ztrain);
        iftDestroyDataSet(&Ztest);

        iftDestroySVM(svm);

      }
      iftDestroyDataSet(&Ztmp);


      mean = 0.0;
      for (it=0; it < nsplits; it++) {
        mean += kappa[it];
      }
      mean /= nsplits;
      stdev = 0.0;
    
      for (it=0; it < nsplits; it++) {
        stdev += (kappa[it]-mean)*(kappa[it]-mean);
      }
      if (nsplits > 1)
        stdev = sqrtf(stdev/(nsplits-1));
      
      printf("s=%f, C=%f, mean acc: %f, mean kappa: %f\n", Cs[i], sigmas[j], acc, mean);
    
      free(kappa);
      
      if(mean > max_kappa){
        max_kappa = mean;
        max_stdev = stdev;
        C = Cs[i];
        sigma = sigmas[j];
      }
      
      /*
      if(acc > max_acc){
        max_acc = mean;
        C = Cs[i];
        sigma = sigmas[j];
      }*/
      
		}
	}
	values[0] = C;
	values[1] = sigma;
	return values;
}


int main(int argc, char *argv[])
{
  iftDataSet      *Ztrain=NULL;
  iftSVM          *svm=NULL;
  timer           *tstart=NULL;
  int              MemDinInicial, MemDinFinal;
  
  if (argc != 5){
    iftExit("Usage: iftSupTrainBySVM <input-trainingset.zip> <output-classifier.zip> <nsplits> <percTrainVal>", "main");
  }

  MemDinInicial = iftMemoryUsed(1);
  
  int nsplits = atoi(argv[3]);
  float percTrainVal = atof(argv[4]);
  
  tstart  = iftTic();

  Ztrain = iftReadDataSet(argv[1]);
  if (Ztrain->ntrainsamples==0)
    iftExit("Invalid set with no training samples","main");

  // Grid Search
  //timer *tic,*toc;
  float C, sigma;
  float *valuesGrid;
  valuesGrid = iftAllocFloatArray(2);

  valuesGrid = iftGridSearchSVM(Ztrain, percTrainVal, nsplits);
  C = valuesGrid[0];
  sigma = valuesGrid[1];
  //free(valuesGrid);
  printf("C : %f, sigma: %f \n", C, sigma);

  int kernel_type=1;
  int multiclass=1;

  //float sigma = 1.0/Ztrain->nfeats;
  svm    = iftCreateSVM(kernel_type, multiclass, C, sigma); // create SVM
  iftSVMTrain(svm,Ztrain); // Train a supervised SVM classifier
  iftWriteSVM(svm,argv[2]);
    
  iftDestroySVM(svm);
  iftDestroyDataSet(&Ztrain);

  puts("\nDone...");
  puts(iftFormattedTime(iftCompTime(tstart, iftToc())));

  MemDinFinal = iftMemoryUsed();
  iftVerifyMemory(MemDinInicial, MemDinFinal);

  return(0);
}
