#include "ift.h"

/* 
   Compute the mean and standard deviation of kappa for a given number
   of dataset splits into training and testing subsets, as generated
   by iftSplitDataSet.c.

   author: Cesar Castelo
   date:   Aug 28, 2018.
*/

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        iftError("Usage: iftEvaluateOPF <...>\n"
                "[1] input_basename\n"
                "[2] number_of_splits\n"
                "[3] metrics to be used (csv file with one row)\n"
                "[4] Basename for CSV files with results (-1 to not save any results)",
                "main");
    }

    int MemDinInicial = iftMemoryUsed(1);

    iftRandomSeed(time(NULL));

    char *dataSetName = iftCopyString(argv[1]);
    int nsplits = atoi(argv[2]);
    iftCSV *metricsCSV = iftReadCSV(argv[3], ',');
    char *outputBasename = iftCopyString(argv[4]);

    timer *tstart = iftTic();
    iftDataSet *Ztrain = NULL, *Ztest = NULL;
    char filename[2048];

    /* print dataset info */
    printf("Dataset info:\n"); fflush(stdout);
    sprintf(filename, "%s_train_001.zip", dataSetName);
    Ztrain = iftReadDataSet(filename);
    printf("- Training: %d samples, %d feats, %d classes\n", Ztrain->nsamples, Ztrain->nfeats, Ztrain->nclasses); fflush(stdout);
    sprintf(filename, "%s_test_001.zip", dataSetName);
    Ztest = iftReadDataSet(filename);
    printf("- Testing: %d samples, %d feats, %d classes\n", Ztest->nsamples, Ztest->nfeats, Ztest->nclasses); fflush(stdout);
    printf("------------------------------------------------------------------------------------\n"); fflush(stdout);

    int nMetrics = metricsCSV->ncols;
    iftMatrix *metrics = iftCreateMatrix(nsplits, nMetrics);
    int nClasses = Ztrain->nclasses;
    iftMatrix *truePosByClass = iftCreateMatrix(nsplits, nClasses+1);
    iftDestroyDataSet(&Ztrain);
    iftDestroyDataSet(&Ztest);

    iftCplGraph *graph = NULL;

    for (int i = 0; i < nsplits; i++)
    {
        printf("Split %d:\n", i + 1); fflush(stdout);
        sprintf(filename, "%s_train_%03d.zip", dataSetName, i + 1);
        Ztrain = iftReadDataSet(filename);
        if (Ztrain->ntrainsamples == 0)
            iftError("Invalid set with no training samples", "main");
        sprintf(filename, "%s_test_%03d.zip", dataSetName, i + 1);
        Ztest = iftReadDataSet(filename);

        /* perform training and testing with OPF */        
        graph = iftCreateCplGraph(Ztrain); // Create complete graph
        iftSupTrain(graph);                // Train a supervis ed OPF classifier
        iftClassify(graph, Ztest);            // Classify test samples

        /* compute the metrics */
        for(int m = 0; m < nMetrics; m++)
            iftMatrixElem(metrics, i, m) =  iftComputeMetricInDataSetByName(Ztest, metricsCSV->data[0][m]);

        /* compute the true positives by class */
        iftFloatArray *truePosByClassAux = iftTruePositivesByClass(Ztest);
        for(int c = 1; c < nClasses+1; c++)
            iftMatrixElem(truePosByClass, i, c) =  truePosByClassAux->val[c];
        iftDestroyFloatArray(&truePosByClassAux);

        /* print the results */
        for(int m = 0; m < nMetrics; m++) {
            printf("- %s: %f\n", metricsCSV->data[0][m], iftMatrixElem(metrics, i, m)); fflush(stdout);
        }
        printf("- True positives by class:\n"); fflush(stdout);
	for(int c = 1; c < nClasses+1; c++){
	  printf("  class %d: %f\n", c, iftMatrixElem(truePosByClass, i, c)); fflush(stdout);
	}
        printf("------------------------------------------------------------------------------------\n"); fflush(stdout);
        iftDestroyCplGraph(&graph);
        iftDestroyDataSet(&Ztrain);
        iftDestroyDataSet(&Ztest);
    }

    /* compute mean and standard deviaton */
    iftFloatArray *mean = iftCreateFloatArray(nMetrics);
    iftFloatArray *stdev = iftCreateFloatArray(nMetrics);
    for(int m = 0; m < nMetrics; m++) {
        mean->val[m] = iftMean(iftMatrixRowPointer(metrics, m), nsplits);
        stdev->val[m] = iftStd(iftMatrixRowPointer(metrics, m), nsplits);
    }

    iftFloatArray *meanTruePosByClass = iftCreateFloatArray(nClasses+1);
    iftFloatArray *stdevTruePosByClass = iftCreateFloatArray(nClasses+1);
    for(int c = 1; c < nClasses+1; c++) {
        meanTruePosByClass->val[c] = iftMean(iftMatrixRowPointer(truePosByClass, c), nsplits);
        stdevTruePosByClass->val[c] = iftStd(iftMatrixRowPointer(truePosByClass, c), nsplits);
    }

    /* print the mean and stdev */
    float execTime = iftCompTime(tstart, iftToc()) / 1000.0;
    printf("Final classification results:\n"); fflush(stdout);
    for(int m = 0; m < nMetrics; m++) {
        printf("- %s -> mean: %f, stdev: %f\n", metricsCSV->data[0][m], mean->val[m], stdev->val[m]); fflush(stdout);
    }

    printf("- True positives by class:\n"); fflush(stdout);
    for(int c = 1; c < nClasses+1; c++){
        printf("  class %d -> mean: %f, stdev: %f\n", c, meanTruePosByClass->val[c], stdevTruePosByClass->val[c]); fflush(stdout);
    }  
    printf("\nTotal time: %s\n", iftFormattedTime(execTime*1000.0));
    printf("Mean time per split: %s\n", iftFormattedTime(execTime/(float)nsplits*1000.0));

    /* save classification results in a csv file */
    if(strcmp(outputBasename, "-1") != 0) {
        /* metrics */
        iftCSV *csvFileMetrics = iftCreateCSV(2, nMetrics*2 + 1);
        for(int m = 0, m1 = 0; m < nMetrics; m++, m1 += 2) {
            sprintf(csvFileMetrics->data[0][m1], "mean_%s", metricsCSV->data[0][m]);
            sprintf(csvFileMetrics->data[0][m1+1], "stdev_%s", metricsCSV->data[0][m]);
            sprintf(csvFileMetrics->data[1][m1], "%f", mean->val[m]);
            sprintf(csvFileMetrics->data[1][m1+1], "%f", stdev->val[m]);
        }
        sprintf(csvFileMetrics->data[0][nMetrics*2], "mean_exec_time");
        sprintf(csvFileMetrics->data[1][nMetrics*2], "%f", execTime/(float)nsplits);
        sprintf(filename, "%s_classif_results_opf.csv", outputBasename);
        iftWriteCSV(csvFileMetrics, filename, ',');
        iftDestroyCSV(&csvFileMetrics);
        
        /* true positives by class */
        iftCSV *csvFileTruePosByClass = iftCreateCSV(2, nClasses*2);
        for(int c = 1, c1 = 0; c < nClasses+1; c++, c1 += 2) {
            sprintf(csvFileTruePosByClass->data[0][c1], "mean_true_positives_class_%d", c);
            sprintf(csvFileTruePosByClass->data[0][c1+1], "stdev_true_positives_class_%d", c);
            sprintf(csvFileTruePosByClass->data[1][c1], "%f", meanTruePosByClass->val[c]);
            sprintf(csvFileTruePosByClass->data[1][c1+1], "%f", stdevTruePosByClass->val[c]);
        }
        sprintf(filename, "%s_classif_results_true_pos_by_class_opf.csv", outputBasename);
        iftWriteCSV(csvFileTruePosByClass, filename, ',');
        iftDestroyCSV(&csvFileTruePosByClass);
    }

    iftDestroyCSV(&metricsCSV);
    iftDestroyMatrix(&metrics);
    iftDestroyFloatArray(&mean);
    iftDestroyFloatArray(&stdev);

    int MemDinFinal = iftMemoryUsed();
    iftVerifyMemory(MemDinInicial, MemDinFinal);

    return (0);
}
