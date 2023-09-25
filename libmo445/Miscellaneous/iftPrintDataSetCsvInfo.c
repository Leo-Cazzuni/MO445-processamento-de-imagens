//
// Created by Cesar Castelo on Jun 29, 2018
//

#include <ift.h>

int main(int argc, char** argv)  {

    if(argc != 2) {
        iftError("\nUsage: iftPrintDataSetCsvInfo <...>\n"
                         "[1] dataset_csv: input dataset in CSV format\n",
                 "iftPrintDataSetCsvInfo.c");
    }

    iftFileSet *fileset = iftLoadFileSetFromDirOrCSV(argv[1], 1, 1);
    int nClasses = iftFileSetLabelsNumber(fileset);

    printf("- Number of Samples: %ld\n", fileset->n);
    printf("- Number of Classes: %d\n\n", nClasses);

    int *labels = iftCountSamplesPerClassFileSet(fileset);

    for (int c = 1; c < nClasses+1; c++) {
      printf("truelabel %d: %d samples\n", c, labels[c]);
    }

    iftDestroyFileSet(&fileset);
    iftFree(labels);

    return 0;
}