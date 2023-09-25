/**
 * @file
 * @brief Train an One Class OPF and classify a testing dataset.
 * @note See the source code in @ref iftDataClassifyByOneClassOPF.c
 *
 * @example iftDataClassifyByOneClassOPF.c
 * @brief Train an One Class OPF and classify a testing dataset.
 * @author Samuel Martins
 * @date Fev 22, 2018
 */

#include "ift.h"


float iftCohenKappaScoreBinary(int TP, int FN, int FP, int TN) {
    // https://en.wikipedia.org/wiki/Cohen%27s_kappa
    // double checked with python function sklearn.metrics.cohen_kappa_score
    int n_examples = TP + FN + FP + TN;

    float Po = ((float) TP + TN) / n_examples;
    float Ptrue = (((float) TP + FN) / n_examples) * (((float) TP + FP) / n_examples);
    float Pfalse = (((float) FP + TN) / n_examples) * (((float) FN + TN) / n_examples);
    float Pe = Ptrue + Pfalse;

    float kappa = (Po - Pe) / (1 - Pe);
    return kappa;
}



int main(int argc, char const *argv[]) {
    if (argc != 4)
        iftExit("iftDataClassifyByOneClassOPF <train.zip> <test.zip> <kmax (perc. in [0, 1] or value in [1,N])>", "main");

    char *train_path = iftCopyString(argv[1]);
    char *test_path = iftCopyString(argv[2]);
    float kmax = atof(argv[3]);
    if (kmax < 0)
        iftError("Kmax %f < 0... Try > 0", "main", kmax);

    puts("- Reading Training Dataset");
    iftDataSet *Ztrain = iftReadDataSet(train_path);
    iftSetStatus(Ztrain, IFT_TRAIN);

    puts("- Reading Testing Dataset");
    iftDataSet *Ztest = iftReadDataSet(test_path);

    if (kmax < 1)
        kmax = iftMax(kmax * Ztrain->ntrainsamples, 1);

    timer *t1 = iftTic();
    puts("- Unsupervised Training\n");
    iftKnnGraph *graph = iftCreateKnnGraph(Ztrain, kmax);
    iftUnsupTrain(graph,iftNormalizedCut);
    puts(iftFormattedTime(iftCompTime(t1, iftToc())));

    iftIntArray *groupsize = iftCreateIntArray(Ztrain->ngroups);
    for (int s = 0; s < Ztrain->nsamples; s++)
        groupsize->val[Ztrain->sample[s].group-1]++;

    for (int i = 0; i < groupsize->n; i++)
        printf("Group %d has %d samples\n", i+1, groupsize->val[i]);
    puts("");

    // iftFileSet *fset = (iftFileSet *) Ztrain->ref_data;
    // for (int s = 0; s < Ztrain->nsamples; s++) {
    //     if (groupsize->val[Ztrain->sample[s].group-1] < 20) {
    //         printf("%s (group %02d)\n", fset->files[s]->path, Ztrain->sample[s].group);
    //     }
    // }

    puts("- One Class Classification");
    iftSetStatus(Ztest, IFT_TEST);
    timer *ti = iftTic();
    iftClassifyByOneClassOPF(graph, Ztest);
    puts(iftFormattedTime(iftCompTime(ti, iftToc())));


    // [0] Positive class, [1] Negative class
    int n_hits_per_class[2] = {0, 0};
    int n_errors_per_class[2] = {0, 0};

    for (int s = 0; s < Ztest->nsamples; s++) {
        // hit
        if (Ztest->sample[s].label == Ztest->sample[s].truelabel)
            n_hits_per_class[Ztest->sample[s].truelabel - 1]++;
        // error
        else n_errors_per_class[Ztest->sample[s].truelabel - 1]++;
    }

    int TP = n_hits_per_class[0]; // positive classified as positive
    int FN = n_errors_per_class[0]; // positive classified as negative
    int FP = n_errors_per_class[1]; // negative classified as positive
    int TN = n_hits_per_class[1]; // negative classified as negative

    float sensitivity = (float) TP / (TP + FN); // TP + FN = total number of testing positive samples
    float specificity  = (float) TN / (TN + FP); // TN + FP = total number of testing negative samples

    printf("\nTP: %d, FN: %d\nFP: %d, TN: %d\n", TP, FN, FP, TN);
    printf("sensitivity (true positive rate): %f\n", sensitivity);
    printf("specificity (true negative rate): %f\n", specificity);
    printf("* kappa: %f\n\n", iftCohenKappaScoreBinary(TP, FN, FP, TN));

    // plot the training and testing sets
    // iftDataSet *Z   = iftMergeDataSets(Ztrain, Ztest);
    // iftDataSet *Z2d = iftDimReductionByTSNE(Z, 2, 40, 1000);
    // iftImage *img   = iftDraw2DFeatureSpace(Z2d, LABEL, 0); /* GROUP, CLASS, LABEL */
    // iftWriteImageByExt(img, "label.png");
    // iftDestroyImage(&img);
    // iftDestroyDataSet(&Z2d);
    // iftDestroyDataSet(&Z);

    iftFree(train_path);
    iftFree(test_path);
    iftDestroyDataSet(&Ztrain);
    iftDestroyDataSet(&Ztest);
    iftDestroyKnnGraph(&graph);

    return 0;
}




