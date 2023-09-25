#include <ift.h>

int main(int argc, char** argv) {

  if ((argc != 6)&&(argc != 7)) {
    iftError("Usage: iftCSVToDataset <input csv> <output zip> <normalize_dataset (0:NO/1:YES)> <separator (e.g., ';')> <NumberColumnsAfterFeatures (0- no truelabels,1- truelabel only, or 2- truelabel and sample id)> [optional fileset.csv related to the sample ids]",
	     "iftCSVToDataSet");
  }

    char *separator = argv[4];

    iftDataSet* Z;
    if (argc == 7)
      Z = iftReadCSVDataSet(argv[1], separator[0], atoi(argv[5]), argv[6]);
    else
      Z = iftReadCSVDataSet(argv[1], separator[0], atoi(argv[5]), NULL);
      
    if (atoi(argv[3]) && !iftIsNormalizedDataSet(Z)){
      iftSetStatus(Z,IFT_TRAIN);
      iftDataSet *Z_temp= iftNormalizeDataSetByZScore(Z,NULL);
      iftDestroyDataSet(&Z);
      Z=Z_temp;
      printf("Dataset normalized\n");
    }

    printf("Number of samples -> %d\n", Z->nsamples);
    printf("Number of classes -> %d\n", Z->nclasses);
    printf("Number of feats -> %d\n", Z->nfeats);

    for (int s=0;s<20;s++){
        for (int f=0;f<Z->nfeats;f++)
            printf("%.3f ",Z->sample[s].feat[f]);
        printf("%d\n",Z->sample[s].truelabel);
    }

    iftWriteDataSet(Z, argv[2]);

    iftDestroyDataSet(&Z);

    return 0;
}
