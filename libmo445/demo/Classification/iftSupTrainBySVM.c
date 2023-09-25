#include "ift.h"

/* Modified by A.X. Falcão (Jan 2018) */

int main(int argc, char *argv[])
{
    if (argc != 7)
    {
      iftError("Usage: iftSupTrainBySVM <...>\n"
	       "[1] input_dataset.zip\n"
	       "[2] kernel_type (0=Linear,1=RBF)\n"
	       "[3] multiclass_type (0=OVO,1=OVA)\n"
	       "[4] constant C (e.g., 1e2, 1e3, 1e5)\n"
	       "[5] kernelization (0-NO, 1-YES)\n"
	       "[6] output_classifier.zip\n", "main");
    }

    iftDataSet *Z   = iftReadDataSet(argv[1]);
		   
    int kernel_type = atoi(argv[2]);
    int multiclass  = atoi(argv[3]);
    float C         = atof(argv[4]);
    int kernelize   = atoi(argv[5]);

    /* print input parameters */
    printf("- kernel: %s, multiclass: %s, constant: %.2e, kernelization: %s\n", kernel_type == IFT_LINEAR ? "Linear" : "RBF",
                                                                                multiclass == IFT_OVO ? "OVO" : "OVA",
                                                                                C,
                                                                                kernelize == 1 ? "Yes" : "No");
    printf("- nsamples: %d, nfeats: %d, nclasses: %d\n", Z->nsamples, Z->nfeats, Z->nclasses);

    iftSVM *svm = NULL;

    float sigma = 1.0 / Z->nfeats;

    svm         = iftCreateSVM(kernel_type, multiclass, C, sigma); // create SVM

    timer *t1 = iftTic();
    // kernelization is implemented only for the Linear SVM
    if (kernelize && (kernel_type == 0))
    {
      uchar traceNormalize = 0;
      svm->kernelization = kernelize;
      float ktrace;
      iftDataSet *Zaux = iftKernelizeDataSet(Z, Z, kernel_type, traceNormalize, &ktrace);
      printf("training ... "); fflush(stdout);
      if (multiclass == IFT_OVO)
        iftSVMTrainOVO(svm, Zaux, Z);
      else
        iftSVMTrainOVA(svm, Zaux, Z);

      iftDestroyDataSet(&Zaux);
    }
    else
    {
      printf("training ... "); fflush(stdout);
      iftSVMTrain(svm, Z);
    }
    printf("OK\n");

    printf("saving classifier ... "); fflush(stdout);
    iftWriteSVM(svm, argv[6]);
    iftDestroySVM(svm);
    iftDestroyDataSet(&Z);
    printf("OK\n");

    printf("Done ... %s\n", iftFormattedTime(iftCompTime(t1, iftToc())));

    return (0);
}
