//
// Created by aemontero7 on 11/28/16.
//

#ifndef IFT_IFTCLUSTERING2_H
#define IFT_IFTCLUSTERING2_H

#endif //IFT_IFTCLUSTERING2_H

#include "iftCommon.h"
#include "iftDataSet.h"
#include "iftAdjSet.h"
#include "iftSet.h"
#include "iftFHeap.h"
#include "iftSet.h"
#include "iftBMap.h"
#include "iftMSPS.h"
#include "iftMST.h"
#include "iftSort.h"
#include "iftClustering.h"

/**
 * @brief Clusters a big dataset using a divide and conquer strategy and the unsupervised OPF method.
 * @note The samples of each partition are selected randomly without repetition. The representatives of each level go through the nex level as the mean of the feature vector of its cluster.
 * @param dataset A big dataset to be clustered
 * @param num_elements_for_partition Size of the partitions in each level except the last
 * @param iftGraphCutFun A graph cut function
 * @param k_max_percent The maximum number of neighbors possible for each node will be this percent value of the number of training samples in the each partitioned dataset
 * @param hierarchy_level_count Number of levels in the hierarchy
  * @returns The number of clusters found
 */
int iftDivideAndConquerMeanUnsupOPF(iftDataSet *dataset, int num_elements_for_partition, iftKnnGraphCutFun iftGraphCutFun, float k_max_percent, int hierarchy_level_count);

/**
 * @brief Clusters a big image using a divide and conquer strategy, tile division and the unsupervised OPF method.
 * @note The representatives of each level go through the nex level as the mean of the feature vector of its cluster.
 * @param orig_dataset A big dataset (that references an image) to be clustered
 * @param num_blocks Number of tiles to partition the image
 * @param iftGraphCutFun A graph cut function
 * @param k_max_percent The maximum number of neighbors possible for each node will be this percent value of the number of training samples in the each partitioned dataset
 * @param hierarchy_level_count Number of levels in the hierarchy
 * @returns The number of clusters found
 */
int iftImageDivideAndConquerMeanByBlocksUnsupOPF(iftDataSet *orig_dataset, int num_blocks, iftKnnGraphCutFun iftGraphCutFun, float k_max_percent, int hierarchy_level_count);

/**
 * @brief Clusters a big dataset using a divide and conquer strategy and the OPF method. The dataset is partitioned in bigs partitions where each one has <b>num_elements_for_dataset</b> samples. Each big partition is clustered by the iftUnsupTrain and iftUnsupClassify functions. The last level of the hierarchy is only clustered by the iftUnsupTrain method.
 * @note The samples of each partition are selected randomly without repetition.
 * @param orig_dataset A big dataset to be clustered
 * @param num_elements_for_partition Size of the partitions in each level except the last. This number must be big, otherwise use iftDivideAndConquerRandomlyUnsupOPF
 * @param train_percent Percent of the samples in each partition that will be training samples
 * @param iftGraphCutFun A graph cut function
 * @param k_max_perc1 k max percent in the first levels
 * @param k_max_perc2 k max percent in the last level
 * @param hierarchy_level_count Number of levels in the hierarchy
  * @returns The number of clusters found
 */
int iftDivideAndConquerRandomlyUnsupOPF2(iftDataSet *orig_dataset, int num_elements_for_dataset, float train_perc,
                                         iftKnnGraphCutFun iftGraphCutFun, float k_max_perc1, float k_max_perc2);