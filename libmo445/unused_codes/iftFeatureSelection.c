#include "iftFeatureSelection.h"

//constructor
iftFeatureSelector* iftCreateFeatureSelector(iftDataSet* Z, iftDataSetEvalMetric metric, iftDataSetMachineLearning learn)
{
	iftFeatureSelector* sel = (iftFeatureSelector*) malloc(sizeof(iftFeatureSelector));

	if(sel==NULL)
	{
		iftError("Allocating error.","iftFeatureSelection");
	} 

	sel->Z = Z;
	sel->iftEvalMetric = metric;
	sel->iftMachineLearning = learn;
	sel->nruns = 10;
	sel->trainPerc = 0.75f;
	sel->featsPenalty = 0.2f;
	sel->nclusters = 2;

	return sel;
}

//destructor
void iftDestroyFeatureSelector(iftFeatureSelector ** sel)
{
	iftFree(*sel);
}

//Private methods
void iftOneFeatureDataset(iftDataSet* Z, int feat, iftDataSet* zout)
{
    int i;
    int m;

    m = Z->nsamples;

    for(i=0; i<m; ++i)
    {
        zout->sample[i].feat[0] = Z->sample[i].feat[feat];
    }
}

float a = 0.0;

int run = 0;
float iftOptimizationFitnessFeatureEval(void* problem, float* theta)
{
	run++;

	float featsAmmount, sumArray;
	int i;
	iftFeatureSelector* sel = (iftFeatureSelector*) problem;
	int nrun = sel->nruns;
	float trainPerc = sel->trainPerc;
	float acc = 0.0;

	sumArray = 0.0;
	featsAmmount = 0.0;
	for(i=0; i < sel->Z->nfeats; ++i)
	{
		sumArray += fabs(theta[i]);
	}

	if(sumArray < 0.01)
	{
		return 0.0;
	}

	for (i = 0; i < sel->Z->nfeats; ++i)
	{
		float f = theta[i]/(sumArray);
		featsAmmount += (f)*(f);
	}

	iftCopyFloatArray(sel->Z->alpha, theta, sel->Z->nfeats);
	// iftNormalizeFeatures(sel->Z->alpha, sel->Z->nfeats);//should normalize?

	for (i = 0; i < nrun; ++i)
	{
		iftSelectSupTrainSamples(sel->Z, trainPerc);
		sel->iftMachineLearning(sel->Z, sel);
		acc+=sel->iftEvalMetric(sel->Z);
	}
	acc/=nrun;

	float eval = (1.0 - sel->featsPenalty)*acc + sel->featsPenalty*featsAmmount;

	return eval;
}


void iftInitializeMspsScales(iftMSPS* msps, double lower, double upper)
{
    int i, j;

    float val = lower;
    float inc = (upper-lower)/(msps->m-1.0);
    for (i = 0; i < msps->m; ++i) {
        for (j = 0; j < msps->n; ++j) {
            int index = iftGetMatrixIndex(msps->delta, j, i);
            msps->delta->val[index] = val;
            // printf("%f ", msps->delta->val[index]);
        }
        val+=inc;
        // printf("\n");
    }
}


//Public Methods
void iftFeatureRank(iftFeatureSelector* featureSelector)
{
	int i, j;
	int nrun = featureSelector->nruns;
	float trainPerc = featureSelector->trainPerc;
	int n = featureSelector->Z->nfeats;
	int m = featureSelector->Z->nsamples;
	float eval;

	float* rank = iftAllocFloatArray(n);
	iftDataSet *z = iftCreateDataSet(m, 1);//creates a dataset with only one feature
	iftDataSet *Z = featureSelector->Z;

	//copy labels from original dataset
	z->nclasses = Z->nclasses;
	for (i = 0; i < m; ++i)
	{
        z->sample[i].truelabel = Z->sample[i].truelabel;
	}

	for (i = 0; i < n; ++i)
	{
		iftOneFeatureDataset(Z, i, z);
		eval = 0.0;
		for(j=0; j<nrun; ++j)
		{
			iftSelectSupTrainSamples(z, trainPerc);
			featureSelector->iftMachineLearning(z, featureSelector);
			eval += featureSelector->iftEvalMetric(z);
		}
		rank[i] = eval/nrun;
	}

	iftNormalizeFeatures(rank, Z->nfeats);
	iftCopyFloatArray(Z->alpha, rank, Z->nfeats);
	iftFree(rank);
}

void iftFeatureWeighting(iftFeatureSelector* featureSelector)
{
	iftMSPS* msps = iftCreateMSPS(featureSelector->Z->nfeats, 10, iftOptimizationFitnessFeatureEval, featureSelector);//review parameters

	int i;
	for(i=0; i<msps->n; ++i)
		msps->theta[i] = 0.5f;

	// iftNormalizeDataSet(featureSelector->Z);
	iftInitializeMspsScales(msps, 0.05f, 2.5f);
	float eval = iftMSPSMax(msps);

	printf("Best feature subset Eval = %f\n", eval);

	iftCopyFloatArray(featureSelector->Z->alpha, msps->theta, featureSelector->Z->nfeats);
	iftNormalizeFeatures(featureSelector->Z->alpha, featureSelector->Z->nfeats);//should normalize?
}

void iftFeatSelOPFClassify(iftDataSet* Z, iftFeatureSelector* sel)
{
	iftCplGraph* g = iftCreateCplGraph(Z);
	iftSupTrain(g);
	iftClassify(g, Z);
}

void iftFeatSelSVMRBFClassify(iftDataSet* Z, iftFeatureSelector* sel)
{
	iftDataSet* Ztrain = iftExtractSamples(Z, TRAIN);
	//iftDataSet* Ztest = iftExtractSamples(Z, TEST);
	//iftSVM *svm = iftCreateLinearSVC(1e5);
    iftSVM* svm = iftCreateRBFSVC(1e5, 0.1);

    iftSVMTrainOVO(svm, Ztrain);
    iftSVMClassifyOVO(svm, Z, TEST);

    iftDestroyDataSet(&Ztrain);
}

void iftFeatSelKMeansCluster(iftDataSet* Z, iftFeatureSelector* sel)
{
  	iftDataSet* Zk = iftKmeansInitCentroidsFromSamples(Z, sel->nclusters);
  	iftKmeansRun(0, Z, &Zk, 10000, 1e-5);
  	iftDestroyDataSet(&Zk);
}

void iftFeatSelOPFCluster(iftDataSet* Z, iftFeatureSelector* sel)
{
  	iftSetStatus(Z, TRAIN);
  	iftKnnGraph* graph = iftCreateKnnGraph(Z, sel->nclusters);
	iftUnsupTrain(graph, iftNormalizedCut);
  	iftDestroyKnnGraph(&graph);
}
