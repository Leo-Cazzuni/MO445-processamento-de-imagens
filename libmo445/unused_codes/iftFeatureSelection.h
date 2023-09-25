#ifndef IFT_FEATURE_SELECTION_H_
#define IFT_FEATURE_SELECTION_H_

#ifdef __cplusplus
extern "C" {
#endif

//Author: Alan Peixinho

//Warning: This module is under development
#include "iftDataSet.h"
#include "iftMetrics.h"
#include "iftMSPS.h"
#include "iftClassification.h"
#include "iftSVM.h"
#include "iftKmeans.h"
#include "iftClustering.h"

typedef struct ift_feat_selector iftFeatureSelector;

//Type definitions
typedef float (*iftDataSetEvalMetric) (iftDataSet* set);//an interface to evaluate the accuracy of a classifier or cluster
typedef void (*iftDataSetMachineLearning) (iftDataSet* set, iftFeatureSelector* sel);//an interface to classfify, or cluster the database

struct ift_feat_selector
{
	iftDataSet* Z;
	iftDataSetEvalMetric iftEvalMetric;
	iftDataSetMachineLearning iftMachineLearning;//The Classifier/Clustering method
	
	int nruns;//for supervised algorithms only
	float trainPerc;//for supervised algorithms only

	float featsPenalty;//for subset feature evaluation only

	int nclusters;//for unsupervised algorithms only
};

//Public Methods
iftFeatureSelector* iftCreateFeatureSelector(iftDataSet* Z, iftDataSetEvalMetric metric, iftDataSetMachineLearning ml);
void iftDestroyFeatureSelector(iftFeatureSelector** sel);
void iftFeatureRank(iftFeatureSelector* featureSelector);
void iftFeatureCombination(iftFeatureSelector* featureSelector, int nfeats);
void iftFeatureWeighting(iftFeatureSelector* featureSelector);

//iftDataSetMachineLearning
void iftFeatSelOPFClassify(iftDataSet* Z, iftFeatureSelector* sel);
void iftFeatSelSVMRBFClassify(iftDataSet* Z, iftFeatureSelector* sel);

void iftFeatSelKMeansCluster(iftDataSet* Z, iftFeatureSelector* sel);
void iftFeatSelOPFCluster(iftDataSet* Z, iftFeatureSelector* sel);

#ifdef __cplusplus
}
#endif


#endif //IFT_FEATURE_SELECTION_H_
