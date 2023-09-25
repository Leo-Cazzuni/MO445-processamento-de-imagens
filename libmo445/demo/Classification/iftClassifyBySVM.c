#include "ift.h"

/* Classify a dataset by using a pre-trained SVM classifier and add
   the kappa result to a given file (by A.X. Falcao, March 23rd
   2018) */

int main(int argc, char *argv[])
{
  iftDataSet      *Ztest=NULL;
  iftSVM          *svm=NULL;
  timer           *tstart=NULL;
  FILE            *fp=NULL;
	
  if (argc != 4){
    iftError("Usage: iftClassifyBySVM <...>\n"
	     "[1] input_dataset.zip\n"
	     "[2] input_classifier.zip\n"
	     "[3] output_folder\n", "main");
  }
  
  Ztest = iftReadDataSet(argv[1]);
  svm   = iftReadSVM(argv[2]);
  iftMakeDir(argv[3]);
  char filename[500];
  sprintf(filename,"%s/results.csv",argv[3]);
  fp    = fopen(filename,"a");
  
  printf("- nsamples: %d, nfeats: %d, nclasses: %d\n", Ztest->nsamples, Ztest->nfeats, Ztest->nclasses);
  
  tstart = iftTic();

  if (svm->kernelization){ /* Apply kernelization */
    printf("kernelizing ... "); fflush(stdout);
    uchar traceNormalize = 0;
    float ktrace;
    iftDataSet *Zaux = iftKernelizeDataSet(svm->Z, Ztest, LINEAR, traceNormalize, &ktrace);
    iftSetStatus(Zaux,IFT_TEST);
    iftCopyRefData(Zaux,Ztest->ref_data,Ztest->ref_data_type);
    iftDestroyDataSet(&Ztest);
    Ztest = iftCopyDataSet(Zaux, true);
    iftDestroyDataSet(&Zaux);
  }
  iftSetStatus(Ztest,IFT_TEST);
  printf("classifying ... "); fflush(stdout);
  iftSVMClassify(svm, Ztest, IFT_TEST);
  
  float kappa = iftCohenKappaScore(Ztest);
  float truePos = iftTruePositives(Ztest);
  printf("kappa: %f, accuracy: %f\n",kappa,truePos);

  iftFloatArray *TP = iftTruePositivesByClass(Ztest);
  int *nSampPerClass = iftCountSamplesPerClassDataSet(Ztest);


  /* Write csv file with misclassified images; truelabel; and assigned
     label */
  if (Ztest->ref_data != NULL) {
    if (Ztest->ref_data_type == IFT_REF_DATA_FILESET) {
      sprintf(filename,"%s/%s-classified-images.csv",argv[3],iftFilename(argv[1],".zip"));
      puts(filename);
      FILE *fp_class = fopen(filename,"w");
      iftFileSet *fs = Ztest->ref_data;
      for (int s=0; s < Ztest->nsamples; s++) {
	fprintf(fp_class,"%s;%d;%d\n",fs->files[Ztest->sample[s].id]->path,Ztest->sample[s].truelabel,Ztest->sample[s].label);
      }
      fclose(fp_class);
    }
  }
  
  for (int i=1; i < TP->n; i++) {
    printf("- class %d: %f (%d samples)\n",i,TP->val[i],nSampPerClass[i]);
    fprintf(fp,"%f;",TP->val[i]);
  }
  iftFree(TP);
  fprintf(fp,"\n %f;%f\n",kappa,truePos);

  fclose(fp);
  iftDestroySVM(svm);
  iftDestroyDataSet(&Ztest);
  
  printf("Done ... %s\n", iftFormattedTime(iftCompTime(tstart, iftToc())));

  return(0);
}
