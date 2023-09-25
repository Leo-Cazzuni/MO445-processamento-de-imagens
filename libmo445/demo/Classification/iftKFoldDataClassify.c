#include <ift.h>

int main(int argc, const char** argv) {

    iftDataSet      *Z=NULL;
    iftCplGraph     *graph=NULL;
    timer           *t1=NULL,*t2=NULL;
    float            stdev, mean, *acc;
    int              i, n;

    /*--------------------------------------------------------*/

    int MemDinInicial, MemDinFinal;
    MemDinInicial = iftMemoryUsed();

    /*--------------------------------------------------------*/

    if (argc<3)
        iftError("Usage: iftDataClassify <dataset.dat> <n_folds>","main");

    iftRandomSeed(IFT_RANDOM_SEED);

    n = atoi(argv[2]);
    acc  = iftAllocFloatArray(n);

    Z  = iftReadOPFDataSet(argv[1]);
    printf("Total number of samples %d\n",Z->nsamples);
    printf("Total number of features %d\n",Z->nfeats);
    iftSetDistanceFunction(Z, 1);

    t1     = iftTic();

    iftSampler* kfold = iftKFold(Z->nsamples, n);

    for (i=0; i < kfold->niters; i++) {
        iftSampleDataSet(Z, kfold, i);
        graph = iftSupLearn(Z);// Execute the supervised learning
        iftClassify(graph,Z);                // Classify test samples in Z
        acc[i] = iftSkewedTruePositives(Z);     // Compute accuracy on test set
        iftDestroyCplGraph(&graph);
    }

    t2     = iftToc();

    iftDestroyDataSet(&Z);
    iftDestroySampler(&kfold);

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

    MemDinFinal = iftMemoryUsed();
    if (MemDinInicial!=MemDinFinal)
        printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
               MemDinInicial,MemDinFinal);

    return 0;
}
