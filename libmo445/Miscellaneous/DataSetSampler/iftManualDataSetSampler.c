//
// Created by azael on 03/12/19.
//

#include <ift.h>

iftIntArray *iftSListToIntArray(iftSList *SL)
{
    iftIntArray *arr = iftCreateIntArray(SL->n);

    iftSNode *node = SL->head;
    int i = 0;
    while (node != NULL) {
        arr->val[i++] = atoi(node->elem);
        node = node->next;
    }

    return arr;
}

int main(int argc, char** argv)  {

    if(argc != 4) {
        iftError("\nUsage: iftManualDataSetSampler <...>\n"
                         "[1] dataset: dataset to be sampled\n"
                         "[2] sample_status: list of 0s, for train, and 1s, for test (\"0,0,0,1,1...\")\n"
                         "[3] dataset_new: sampled dataset\n",
                 "iftManualDataSampler.c");
    }

    iftSList *sl = iftSplitString(argv[2], ",");
    iftIntArray *arr = iftSListToIntArray(sl);
    iftDestroySList(&sl);

    iftDataSet *Z  = iftReadDataSet(argv[1]);
    if (arr->n != Z->nsamples){
        printf("Number of status (%ld) must be equal to the number of samples in the dataset (%d)\n",arr->n,Z->nsamples);
        iftDestroyDataSet(&Z);
        iftDestroyIntArray(&arr);
        return 0;
    }

    printf("Number of samples in dataset: %d\n",Z->nsamples);
    printf("Number of status: %ld\n",arr->n);

    iftSampler *sampler = iftCreateSampler(arr->n,1);

    for (int i = 0; i < arr->n; i++) {
        if (arr->val[i] == 0)
            sampler->status[0][i] = IFT_TRAIN;
        else if (arr->val[i] == 1)
            sampler->status[0][i] = IFT_TEST;
    }

    iftDataSetSampling(Z, sampler, 0);
    iftWriteDataSet(Z, argv[3]);

    iftDestroySampler(&sampler);
    iftDestroyDataSet(&Z);

    return 0;
}