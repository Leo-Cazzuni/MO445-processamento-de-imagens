//
// Created by peixinho on 6/6/16.
//

#include "ift.h"

int main(int argc, char *argv[]) {
    iftDataSet* dataSet = iftReadOPFDataSet("/home/deangeli/descriptors/base_+4_eggs.zip");
    iftRandomSeed(0);
    const char* buffer1 = "~/eggs_dataSetSplit1_50_1_25.zip";
    const char* buffer2 = "~/eggs_dataSetSplit1_50_2_25.zip";
    const char* buffer3 = "~/eggs_dataSetSplit2_50_1_25.zip";
    const char* buffer4 = "~/eggs_dataSetSplit2_50_2_25.zip";
    iftSelectSupTrainSamples(dataSet,0.5);
    iftDataSet* dataSetSplit1_50 = iftExtractSamples(dataSet,IFT_TRAIN);
    iftDataSet* dataSetSplit2_50 = iftExtractSamples(dataSet,IFT_TEST);
    iftPrintDataSetInfo(dataSet,false);
    iftPrintDataSetInfo(dataSetSplit1_50,false);
    iftPrintDataSetInfo(dataSetSplit2_50,false);
    iftSelectSupTrainSamples(dataSetSplit1_50,0.5);
    iftSelectSupTrainSamples(dataSetSplit2_50,0.5);
    iftPrintDataSetInfo(dataSetSplit1_50,false);
    iftPrintDataSetInfo(dataSetSplit2_50,false);
    iftDataSet* dataSetSplit1_50_1_25 = iftExtractSamples(dataSetSplit1_50,IFT_TRAIN);
    iftDataSet* dataSetSplit1_50_2_25 = iftExtractSamples(dataSetSplit1_50,IFT_TEST);
    iftDataSet* dataSetSplit2_50_1_25 = iftExtractSamples(dataSetSplit2_50,IFT_TRAIN);
    iftDataSet* dataSetSplit2_50_2_25 = iftExtractSamples(dataSetSplit2_50,IFT_TEST);

    //iftPrintDataSetInfo(dataSetSplit1_50_1_25,true);
    //iftPrintDataSetInfo(dataSetSplit1_50_2_25,true);
    //iftPrintDataSetInfo(dataSetSplit2_50_1_25,true);
    //iftPrintDataSetInfo(dataSetSplit2_50_2_25,true);

    iftDataSet* miau = iftReadOPFDataSet("/home/deangeli/data1.zip");
    iftTsne* tsne = iftCreateTsne(miau);
    tsne->theta = 0.1;
    iftComputeTsneProjection(tsne);
    iftPrintDoubleMatrix(miau->projection);
//    iftDoubleMatrix* doubleMatrix = tsne->inputDataSet->projection;
//    int k=0;
//    for (int i = 0; i < doubleMatrix->nrows; ++i) {
//        for (int j = 0; j < doubleMatrix->ncols; ++j) {
//            printf("%f ",doubleMatrix->val[k]);
//            k++;
//        }
//        printf("\n");
//    }
    //iftPrintDoubleMatrix(dataSetSplit1_50_1_25->projection);

    //iftWriteOPFDataSet(dataSetSplit1_50_1_25,buffer1);
    //iftWriteOPFDataSet(dataSetSplit1_50_2_25,buffer2);
    //iftWriteOPFDataSet(dataSetSplit2_50_1_25,buffer3);
    //iftWriteOPFDataSet(dataSetSplit2_50_2_25,buffer4);


}
