#include "ift.h"




int main(int argc, const char *argv[]) {
    if (argc != 4)
        iftError("iftMergeDatasets <dataset1.zip> <dataset2.zip> <out_datasets.zip>", "main");

    char *parent_dir = iftParentDir(argv[3]);
    if (!iftDirExists(parent_dir))
        iftMakeDir(parent_dir);
    iftFree(parent_dir);

    puts("- Reading Dataset 1");
    iftDataSet *Z1 = iftReadDataSet(argv[1]);

    puts("- Reading Dataset 2");
    iftDataSet *Z2 = iftReadDataSet(argv[2]);

    puts("- Merging Datasets");
    iftDataSet *Z3 = iftMergeDataSets(Z1, Z2);

    puts("- Writing Merged Dataset");
    iftWriteDataSet(Z3, argv[3]);

    puts("\nDone...");

    return 0;
}



