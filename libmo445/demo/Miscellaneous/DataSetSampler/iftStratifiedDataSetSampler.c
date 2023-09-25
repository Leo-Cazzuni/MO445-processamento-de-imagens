//
// Created by Cesar Castelo on Mar 26, 2018
//

#include <ift.h>

int main(int argc, char** argv)  {

    if(argc != 4) {
        iftError("\nUsage: iftStratifiedDataSetSampler <...>\n"
                         "[1] dataset: dataset to be sampled\n"
                         "[2] Sampling percentage [0..1]\n"
                         "[3] dataset_new: sampled dataset\n",
                 "iftStratifiedDataSetSampler.c");
    }

    /* read parameters */
    float percSampl = atof(argv[2]);

    if(percSampl >= 1 || percSampl <= 0) {
        iftError("percSampl must be between 0 and 1 ", "iftStratifiedDataSetSampler.c");
    }

    /* read the dataset and split it into training and testing sets */
    fprintf(stdout, "============================================================================================================================\n"); fflush(stdout);
    fprintf(stdout, "--> Reading the dataset ...\n"); fflush(stdout);
    iftDataSet *Z  = iftReadDataSet(argv[1]);
    fprintf(stdout, "Num. Samples: %d\n", Z->nsamples); fflush(stdout);
    fprintf(stdout, "Num. Features: %d\n", Z->nfeats); fflush(stdout);
    fprintf(stdout, "Num. Samples (sampled dataset): %d\n", (int)(Z->nsamples*percSampl)); fflush(stdout);

    iftIntArray *labels = iftGetDataSetTrueLabels(Z);
    iftSampler *trainSampler = iftStratifiedRandomSubsampling(labels->val, labels->n, 1, (float)(labels->n)*percSampl);
    iftDataSetSampling(Z, trainSampler, 0);
    iftDataSet *Ztrain = iftExtractSamples(Z, IFT_TRAIN);
    iftWriteDataSet(Ztrain, argv[3]);

    iftDestroyDataSet(&Z);
    iftDestroyIntArray(&labels);
    iftDestroySampler(&trainSampler);
    iftDestroyDataSet(&Ztrain);

    return 0;
}
