#include "ift.h"

iftKnnGraph *iftMSTtoKnnGraph(iftMST* mst, int number_neigh);

int main(int argc, char *argv[]) {

    if (argc != 4)
        iftError("Arguments must be: iftMSAdjacency <intput dataset> <number of adjacents> <output file>",
                 "iftMSTAdjacency");

    int k = atoi(argv[2]);
    iftDataSet *Z = iftReadDataSet(argv[1]);

    iftSetStatus(Z, IFT_TRAIN);
    iftMST *mst = iftCreateMST(Z);

    iftKnnGraph *graph = iftMSTtoKnnGraph(mst, k);
    iftBestkByKnnGraphCut(graph, iftNormalizedCut);

    printf("Best k %d\n", graph->k);

    iftPDFByRange(graph);

    iftImage *img = iftDraw2DFeatureSpace(Z, WEIGHT, IFT_TRAIN);
    iftWriteImageByExt(img, "pdf.png");

    int n_groups = iftUnsupOPF(graph);
    printf("Number of groups: %d\n", n_groups);

    iftWriteDataSet(Z, "out.zip");

    iftDestroyImage(&img);
    iftDestroyDataSet(&Z);
    iftDestroyMST(&mst);
    iftDestroyKnnGraph(&graph);

    return 0;
}
