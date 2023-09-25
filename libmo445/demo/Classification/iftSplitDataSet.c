#include "ift.h"

/*
   Split a dataset into a given number of training and testing
   subsets, according to a given splitting method, and by preserving a
   given percentage of training samples per class.

   author: A.X. Falcão.
   date  : March, 18th 2018.
*/

int main(int argc, char *argv[])
{
    iftDataSet      *Z=NULL,*Ztrain=NULL,*Ztest=NULL;
    iftSampler      *sampler=NULL;
    int              nsplits;
    float            train_perc;
    char             filename[200];
    timer           *tstart=NULL;
    int              MemDinInicial, MemDinFinal;
    iftIntArray     *labels=NULL;

    if (argc != 6){
        iftError(
                "Usage: iftSplitDataSet <input-dataset.zip> <input-train_perc in (0,1)> <input-number-of-splits> <input-method (0-Random, 1-KFold, 2-Inverse KFold, 3-Stratified)> <output-basename>",
                "main");
    }

    MemDinInicial = iftMemoryUsed(1);

    iftRandomSeed(time(NULL));

    Z           = iftReadDataSet(argv[1]);
    
    
    nsplits     = atoi(argv[3]);
    train_perc  = atof(argv[2]);

    printf("Total number of samples  %d\n",Z->nsamples);
    printf("Total number of features %d\n",Z->nfeats);
    printf("Total number of classes  %d\n",Z->nclasses);

    tstart = iftTic();

    switch (atoi(argv[4])) {

        case 0: // Random

            sampler = iftRandomSubsampling(Z->nsamples, nsplits, train_perc*Z->nsamples);

            break;

        case 1: // KFold

            sampler = iftKFold(Z->nsamples, nsplits);
            break;
        
        case 2: // Inverse KFold

            sampler = iftInverseKFold(Z->nsamples, nsplits);
            break;

        case 3: // Stratified

            labels = iftGetDataSetTrueLabels(Z);
            sampler = iftStratifiedRandomSubsampling(labels->val, labels->n, nsplits, train_perc*Z->nsamples);
            break;

        default:
            iftError(
                    "Usage: iftSplitDataSet <input-dataset.zip> <input-train_perc in (0,1)> <input-number-of-splits> <input-method (0-Random, 1-KFold, 2-Inverse KFold, 3-Stratified)> <output-basename>",
                    "main");

    }
    printf("\nWriting datasets:\n");

    for (int i=0; i < nsplits; i++){
        printf("- Split %d ...\n", i+1); fflush(stdout);
        iftDataSetSampling(Z, sampler, i);
        Ztrain = iftExtractSamples(Z,IFT_TRAIN);
        Ztest  = iftExtractSamples(Z,IFT_TEST);
        sprintf(filename,"%s_train_%03d.zip",argv[5],i+1);
        iftWriteDataSet(Ztrain,filename);
        sprintf(filename,"%s_test_%03d.zip",argv[5],i+1);
        iftWriteDataSet(Ztest,filename);
        iftDestroyDataSet(&Ztrain);
        iftDestroyDataSet(&Ztest);
    }
    puts("\nDone...");
    puts(iftFormattedTime(iftCompTime(tstart, iftToc())));

    iftDestroyDataSet(&Z);
    iftDestroySampler(&sampler);

    MemDinFinal = iftMemoryUsed();
    iftVerifyMemory(MemDinInicial, MemDinFinal);

    return(0);
}
