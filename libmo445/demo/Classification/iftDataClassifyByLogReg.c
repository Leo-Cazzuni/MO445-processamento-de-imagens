//
// Created by peixinho on 4/4/16.
//

#include <ift.h>

/************************** SOURCES **************************/
iftDict *iftGetArguments(int argc, const char *argv[]) {
    iftCmdLineOpt cmd_line_opts[] = {
            {.short_name = "-d", .long_name = "--dataset", .has_arg=true, .arg_type=IFT_STR_TYPE,
                    .required=true, .help="Dataset to be classified."},
            {.short_name = "-t", .long_name = "--train", .has_arg=true,  .arg_type=IFT_DBL_TYPE,
                    .required=true, .help="Train percentage [0,1)."},
            {.short_name = "-n", .long_name = "--iterations-number", .has_arg=true,  .arg_type=IFT_LONG_TYPE,
                    .required=false, .help="Number of iterations."}
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

    char program_description[IFT_STR_DEFAULT_SIZE] = "This program show an usage example of the Logistic Regressor classification algorithm.";

    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}

int main(int argc, const char** argv) {
    
    size_t mem1 = iftMemoryUsed();

    iftDict* args = iftGetArguments(argc, argv);

    int n = 1;
    char* path = iftGetStrValFromDict("--dataset", args);
    float trainPerc = iftGetDblValFromDict("--train", args);
    if(iftDictContainKey("--iterations-number", args, NULL))
        n = iftGetLongValFromDict("--iterations-number", args);

    iftDataSet* Z = iftReadOPFDataSet(path);
    iftIntArray *labels = iftGetDataSetTrueLabels(Z);
    // iftSampler* sampler = iftRandomSubsampling(Z->nsamples, n, Z->nsamples*trainPerc);
    iftSampler* sampler = iftStratifiedRandomSubsampling(labels->val, labels->n, n, Z->nsamples*trainPerc);
    iftDestroyIntArray(&labels);

    printf("Total number of samples  %d\n", Z->nsamples);
    printf("Total number of features %d\n", Z->nfeats);
    printf("Total number of classes %d\n", Z->nclasses);

    iftFloatArray* acc = iftCreateFloatArray(n);

    timer* t1 = iftTic();

    for (int i = 0; i < sampler->niters; ++i) {
		iftDataSetSampling(Z, sampler, i);

        iftLogReg* logreg = iftLogRegTrain(Z);
        iftLogRegClassify(logreg, Z);

        acc->val[i] = iftSkewedTruePositives(Z);
        printf("Acc[%d] = %f\n", i, acc->val[i]);

        iftDestroyLogReg(&logreg);
    }
    timer* t2 = iftToc();
    printf("Classifications in ");
    iftPrintFormattedTime(iftCompTime(t1, t2));

    printf("Mean accuracy: %f, Standard Deviation: %f.\n", iftMean(acc->val, acc->n), iftStddevFloatArray(acc->val, acc->n));

    iftGC();

    iftDestroyDataSet(&Z);
    iftDestroyFloatArray(&acc);
    iftDestroySampler(&sampler);
    iftDestroyDict(&args);
    iftFree(path);

    size_t mem2 = iftMemoryUsed();

    iftVerifyMemory(mem1, mem2);

    return 0;
}