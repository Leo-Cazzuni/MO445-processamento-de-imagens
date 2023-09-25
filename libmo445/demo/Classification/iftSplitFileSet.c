#include "ift.h"

/*
   Split a fileset into a given number of training and testing
   subsets, according to a given splitting method, and by preserving a
   given percentage of training samples per class.

   author: Cesar Castelo
   date  : March, 5 2019.
*/

char *iftSamplingMethodName(int sampMet)
{
    char *metName = NULL;
    switch (sampMet) {
        case 0: // Random
            metName = iftCopyString("random");
            break;

        case 1: // KFold
            metName = iftCopyString("k-fold");
            break;
        
        case 2: // Inverse KFold
            metName = iftCopyString("inverse-k-fold");
            break;

        case 3: // Stratified
            metName = iftCopyString("stratified");
            break;

        default:
            iftError("Invalid sampling method chosen", "iftSamplingMethodName");
    }

    return metName;
}

int main(int argc, char *argv[])
{
    if(argc != 6) {
        iftError("\nUsage: iftSplitFileSet <...>\n"
                    "[1] input_fileset: Input fileset or directory containing the files\n"
                    "[2] n_splits: Number of splits to be created\n"
                    "[3] sampling_method: Method to split the fileset\n"
                    "    0: Random\n"
                    "    1: K-Fold\n"
                    "    2: Inverse K-Fold\n"
                    "    3: Stratified\n"
                    "[4] train_perc: Training percentage (only if samp method is random/stratified, -1 otherwise)\n"
                    "[5] output_basename: Basename for the output files\n",
                 "iftSplitFileSet.c");
    }

    /* read parameters */
    iftFileSet *fileset = iftLoadFileSetFromDirOrCSV(argv[1], 1, true);
    int nSplits = atoi(argv[2]);
    int sampMet = atoi(argv[3]);
    float trainPerc = atof(argv[4]);
    char *outputBasename = iftCopyString(argv[5]);

    int MemDinInicial = iftMemoryUsed(1);

    iftRandomSeed(time(NULL));

    int nFiles = fileset->n;
    printf("Number of files: %d\n", nFiles);
    printf("Number of classes: %d\n", iftFileSetLabelsNumber(fileset));

    timer *tstart = iftTic();

    iftSampler *sampler = NULL;
    int *labels = NULL;
    switch (sampMet) {
        case 0: // Random
            sampler = iftRandomSubsampling(nFiles, nSplits, trainPerc*nFiles);
            break;

        case 1: // KFold
            sampler = iftKFold(nFiles, nSplits);
            break;
        
        case 2: // Inverse KFold
            sampler = iftInverseKFold(nFiles, nSplits);
            break;

        case 3: // Stratified
            labels = iftFileSetLabels(fileset);
            sampler = iftStratifiedRandomSubsampling(labels, nFiles, nSplits, trainPerc*nFiles);
            break;

        default:
            iftError("Invalid sampling method chosen", "iftSplitFileSet.c");
    }

    printf("\n--> Writing filesets ... "); fflush(stdout);

    /* create the output dir */
    char *outputDir = iftConcatStrings(3, outputBasename, "_", iftSamplingMethodName(sampMet));
    if(!iftDirExists(outputDir))
        iftMakeDir(outputDir);

    char filename[2048];
    for (int i = 0; i < nSplits; i++){
        iftSampleFileSet(fileset, sampler, i);
        iftFileSet *filesetTrain = iftExtractFileSamples(fileset, IFT_TRAIN);
        iftFileSet *filesetTest  = iftExtractFileSamples(fileset, IFT_TEST);
        sprintf(filename,"%s/train_%03d.csv", outputDir, i+1);
        iftWriteFileSetAsCSV(filesetTrain, filename);
        sprintf(filename,"%s/test_%03d.csv", outputDir, i+1);
        iftWriteFileSetAsCSV(filesetTest, filename);
        iftDestroyFileSet(&filesetTrain);
        iftDestroyFileSet(&filesetTest);
    }
    puts("OK\n");
    puts(iftFormattedTime(iftCompTime(tstart, iftToc())));

    iftDestroyFileSet(&fileset);
    iftDestroySampler(&sampler);

    int MemDinFinal = iftMemoryUsed();
    iftVerifyMemory(MemDinInicial, MemDinFinal);

    return(0);
}
