//
// Created by peixinho on 6/6/16.
//

#include "ift.h"

void computeTest(iftDataSet* trainDataset,char* fileName){
    FILE* pFileAux = fopen(fileName,"r");
    if(pFileAux != NULL){
        return;
    }
        FILE* pFile = fopen (fileName,"w");
    fprintf(pFile,"%s\n",fileName);
    fprintf(pFile,"Mislabeling\n");
    //check mislabeling
    iftFileSet* fileSet = (iftFileSet*)trainDataset->ref_data;
    int nSupervised = 0;
    int nLabelPropagated = 0;
    int nMislabeling = 0;
    unsigned int nChecked = 0;
    int ntrainSamples = 0;
    for (int i = 0; i < trainDataset->nsamples; ++i) {

        if(  (trainDataset->sample[i].isSupervised == true)  ){
            nSupervised++;
            trainDataset->sample[i].status = IFT_TRAIN | IFT_TEST;
        }else if(trainDataset->sample[i].isLabelPropagated == true){
            nLabelPropagated++;
            trainDataset->sample[i].status = IFT_TRAIN | IFT_TEST;
        }
        else{
            trainDataset->sample[i].status = IFT_TEST;
        }

        nChecked += trainDataset->sample[i].numberTimesChecked;
        if( (trainDataset->sample[i].status & IFT_TRAIN) > 0)
        {
            int* fileInfo = iftGetImageSampleInfo(fileSet->files[trainDataset->sample[i].id]->path);
            int trueLabel = fileInfo[0];
            ntrainSamples++;
            if(trueLabel != trainDataset->sample[i].truelabel){
                nMislabeling++;
                fprintf(pFile,"truelabel_verdadeiro:%d truelabel_atribuido:%d %s sampleIndex:%d\n",trueLabel,trainDataset->sample[i].truelabel,fileSet->files[trainDataset->sample[i].id]->path,i);
            }
            free(fileInfo);
        }
    }
    trainDataset->ntrainsamples = ntrainSamples;
    fprintf(pFile,"nSupervised:%d nLabelPropagated:%d ntrainSamples:%d nChecked:%d nMislabeling:%d\n",nSupervised,nLabelPropagated,ntrainSamples,nChecked,nMislabeling);
    iftGenericVector* datasetVector = iftCreateGenericVector(16,sizeof(iftDataSet*));
    iftGenericVector* accVector = iftCreateGenericNullVector(16,sizeof(float));
    iftGenericVector* accNormVector = iftCreateGenericNullVector(16,sizeof(float));
    iftGenericVector* kappaVector = iftCreateGenericNullVector(16,sizeof(float));
    iftGenericVectorPushBack(iftDataSet*,datasetVector,(iftReadDataSet("/home/deangeli/deangeliTest/MNIST/MNIST_1_16.zip")));
    iftGenericVectorPushBack(iftDataSet*,datasetVector,(iftReadDataSet("/home/deangeli/deangeliTest/MNIST/MNIST_2_16.zip")));
    iftGenericVectorPushBack(iftDataSet*,datasetVector,(iftReadDataSet("/home/deangeli/deangeliTest/MNIST/MNIST_3_16.zip")));
    iftGenericVectorPushBack(iftDataSet*,datasetVector,(iftReadDataSet("/home/deangeli/deangeliTest/MNIST/MNIST_4_16.zip")));
    iftGenericVectorPushBack(iftDataSet*,datasetVector,(iftReadDataSet("/home/deangeli/deangeliTest/MNIST/MNIST_5_16.zip")));
    iftGenericVectorPushBack(iftDataSet*,datasetVector,(iftReadDataSet("/home/deangeli/deangeliTest/MNIST/MNIST_6_16.zip")));
    iftGenericVectorPushBack(iftDataSet*,datasetVector,(iftReadDataSet("/home/deangeli/deangeliTest/MNIST/MNIST_7_16.zip")));
    iftGenericVectorPushBack(iftDataSet*,datasetVector,(iftReadDataSet("/home/deangeli/deangeliTest/MNIST/MNIST_8_16.zip")));
    iftGenericVectorPushBack(iftDataSet*,datasetVector,(iftReadDataSet("/home/deangeli/deangeliTest/MNIST/MNIST_9_16.zip")));
    iftGenericVectorPushBack(iftDataSet*,datasetVector,(iftReadDataSet("/home/deangeli/deangeliTest/MNIST/MNIST_10_16.zip")));
    iftGenericVectorPushBack(iftDataSet*,datasetVector,(iftReadDataSet("/home/deangeli/deangeliTest/MNIST/MNIST_11_16.zip")));
    iftGenericVectorPushBack(iftDataSet*,datasetVector,(iftReadDataSet("/home/deangeli/deangeliTest/MNIST/MNIST_12_16.zip")));
    iftGenericVectorPushBack(iftDataSet*,datasetVector,(iftReadDataSet("/home/deangeli/deangeliTest/MNIST/MNIST_13_16.zip")));
    iftGenericVectorPushBack(iftDataSet*,datasetVector,(iftReadDataSet("/home/deangeli/deangeliTest/MNIST/MNIST_14_16.zip")));
    iftGenericVectorPushBack(iftDataSet*,datasetVector,(iftReadDataSet("/home/deangeli/deangeliTest/MNIST/MNIST_15_16.zip")));
    iftGenericVectorPushBack(iftDataSet*,datasetVector,(iftReadDataSet("/home/deangeli/deangeliTest/MNIST/MNIST_16_16.zip")));

    iftGenericClassifier* classifier = iftCreateGenericClassifier(CLASSIFIER_OPF_SUP);
    iftTrainGenericClassifier(classifier,trainDataset);
    iftPredictGenericClassifier(classifier,trainDataset);
    float trueLabelAccTrain = iftTruePositives(trainDataset);
    float trueLabelAccNormTrain = iftNormAccuracy(trainDataset);
    float trueLabelKappaTrain = iftCohenKappaScore(trainDataset);

    for (int j = 0; j < datasetVector->size; ++j) {
        iftDataSet* dataSet = iftVectorAt(iftDataSet*,datasetVector,j);
        iftSetStatus(dataSet,IFT_TEST);
        iftPredictGenericClassifier(classifier,dataSet);

        iftVectorAt(float,accVector,j)  = iftTruePositives(dataSet);
        iftVectorAt(float,accNormVector,j) = iftNormAccuracy(dataSet);
        iftVectorAt(float,kappaVector,j) = iftCohenKappaScore(dataSet);
    }

    fprintf(pFile,"Train | acc:%f acc_norm:%f kappa:%f\n",trueLabelAccTrain,trueLabelAccNormTrain,trueLabelKappaTrain);
    for (int k = 0; k < datasetVector->size; ++k) {
        float acc = iftVectorAt(float,accVector,k);
        float accNorm = iftVectorAt(float,accNormVector,k);
        float kappa = iftVectorAt(float,kappaVector,k);
        fprintf(pFile,"split%d | acc:%f acc_norm:%f kappa:%f\n",k+1,acc,accNorm,kappa);
    }

    for (int j = 0; j < datasetVector->size; ++j) {
        iftDataSet* dataSet = iftVectorAt(iftDataSet*,datasetVector,j);
        iftDestroyDataSet(&dataSet);
    }
    iftDestroyGenericVector(&datasetVector);
    iftDestroyGenericVector(&accVector);
    iftDestroyGenericVector(&accNormVector);
    iftDestroyGenericVector(&kappaVector);
    iftDestroyGenericClassifier(&classifier);
    iftDestroyDataSet(&trainDataset);
    fclose (pFile);
}


int main(int argc, char *argv[]) {
    //iftDataSet* trainDataset = iftReadDataSet("/home/deangeli/deangeliTest/MNIST/peixinho_full_VA/dataset0_7.zip");
    //iftPrintDataSetInfo(trainDataset,false);
    iftFileSet* fileSetDir = iftLoadFileSetFromDirBySuffix("/home/deangeli/deangeliTest/MNIST/danielOasku_full_VA/",".zip");
#pragma omp parallel for
    for (int i = 0; i < fileSetDir->n; ++i) {
        //char* suffix = iftImageSuffix(fileSetDir->files[i]->path);
        printf("step: %s\n",fileSetDir->files[i]->path);
        iftDataSet* trainDataset = iftReadDataSet(fileSetDir->files[i]->path);
        char* name = iftRemoveSuffix(fileSetDir->files[i]->path,".zip");
        computeTest(trainDataset,name);
        printf("\n");
    }

    return 0;
}
