////
//// Created by aemontero7 on 11/28/16.
////
//
//#include "iftClustering2.h"
//
///* In this function I'm using all samples as train samples in the cluster process, I mean I'm not propagating labels through the function iftUnsupClassify. The representatives are passing to the next level with the mean of the feature vector of its cluster. The divide and conquer solution takes the samples with a random strategy.*/
//int iftDivideAndConquerMeanUnsupOPF(iftDataSet *orig_dataset, int num_elements_for_partition, iftKnnGraphCutFun iftGraphCutFun, float k_max_percent, int hierarchy_level_count) {
//
//  /* check that the number of hierarchy level dont't be less than one*/
//  if (hierarchy_level_count < 1)
//    iftError("The hierarchy levels can't be less than 1","iftImageDivideAndConquerUnsupOPF");
//
//  /* if the hierarchy level is one then we don't need to do the divide and conquer strategy, simply cluster the entire dataset */
//  if (hierarchy_level_count ==1){
//
//    iftSetStatus(orig_dataset,TRAIN);
//    if (orig_dataset->ntrainsamples == 0)
//      iftError("No samples for training in dataset","iftDivideAndConquerUnsupOPF");
//    int kmax= (int) iftMax(k_max_percent*orig_dataset->ntrainsamples,1);
//    iftKnnGraph *graph=iftCreateKnnGraph(orig_dataset,kmax);
//    int cluster_count=iftUnsupTrain(graph, iftGraphCutFun);
//    printf("The resultant k in the dataset is %d\n",graph->k);
//
//    /* destroys the KNN Graph */
//    iftDestroyKnnGraph(&graph);
//
//    return cluster_count;
//  }
//
//  /* the hierarchy levels is higher or equal than two. We organize the datasets of the hierarchy levels of the following way: datasets_hierarchy[i] contains the datasets of the i level. Ex:
//   * datasets_hierarchy[0] contains the datasets of the first level
//   * datasets_hierarchy[1] contains the datasets of the second level and so on
//   * */
//  iftDataSet ***datasets_hierarchy = (iftDataSet ***)iftAlloc(hierarchy_level_count-1,sizeof(iftDataSet **));
//
//  /* dataset and cluster count in each level of hierarchy */
//  int *datasets_count=iftAllocIntArray(hierarchy_level_count);
//  int *clusters_count=iftAllocIntArray(hierarchy_level_count);
//
//  timer *t1=NULL,*t2=NULL;
//
//  printf("The original dataset has %d samples\n",orig_dataset->nsamples);
//
//  iftDataSet *last_level_dataset=orig_dataset;
//
//  /* grow up the hierarchy of datasets */
//  for (int l=0; l< hierarchy_level_count-1;l++){
//
//    t1=iftTic();
//    datasets_count[l]= iftDatasetToRandomlyDataSetArray(last_level_dataset, num_elements_for_partition, &datasets_hierarchy[l]);
//    t2=iftToc();
//    printf("There were created %d datasets in level %d in %f ms \n",datasets_count[l],l,iftCompTime(t1,t2));
//
//    int *tmp_cluster_count= iftAllocIntArray(datasets_count[l]);
//
//    /* cluster the datasets of the current level*/
//#pragma omp parallel for shared(datasets_hierarchy,tmp_cluster_count)
//    for (int i=0;i<datasets_count[l];i++){
//
//      /* all samples will be considered as train samples */
//      iftSetStatus (datasets_hierarchy[l][i],TRAIN);
//
//      if (datasets_hierarchy[l][i]->ntrainsamples == 0)
//        iftError("No samples for training in dataset %d of level %d","iftDivideAndConquerUnsupOPF",i,l);
//
//      printf("The number of train samples in dataset %d of level %d is %d\n",i,l,datasets_hierarchy[l][i]->ntrainsamples);
//      int kmax= (int) iftMax(k_max_percent*datasets_hierarchy[l][i]->ntrainsamples,1);
//      iftKnnGraph *graph=iftCreateKnnGraph(datasets_hierarchy[l][i],kmax);
//
//      tmp_cluster_count[i]=iftUnsupTrainRootMap(graph, iftGraphCutFun);
//      printf("The resultant k in the dataset %d of level %d is %d\n",i,l,graph->k);
//      printf("The resultant cluster count in the dataset %d of level %d is %d\n",i,l,tmp_cluster_count[i]);
//
//      /* destroys the KNN Graph */
//      iftDestroyKnnGraph(&graph);
//    }
//
//    clusters_count[l]=0;
//    for (int i=0;i<datasets_count[l];i++)
//      clusters_count[l]+=tmp_cluster_count[i];
//
//    /* creates a new dataset with the cluster representatives of the current level. This will be the dataset in the next level in the hierarchy. Delete the dataset of the current level if it's not the original dataset */
//    if (last_level_dataset != orig_dataset)
//      iftDestroyDataSet(&last_level_dataset);
//
//    last_level_dataset=iftCreateDataSet(clusters_count[l],orig_dataset->nfeats);
//
//    last_level_dataset->ref_data_type=orig_dataset->ref_data_type;
//    last_level_dataset->ref_data=orig_dataset->ref_data;
//
////    if (orig_dataset->nfeats == 3 && orig_dataset->ref_data_type == IFT_REF_DATA_IMAGE){
////      last_level_dataset->alpha[0] = orig_dataset->alpha[0];
////      last_level_dataset->alpha[1] = orig_dataset->alpha[1];
////      last_level_dataset->alpha[2] = orig_dataset->alpha[2];
////    }
//
//    int found_roots=0;
//#pragma omp parallel for shared(datasets_hierarchy,last_level_dataset,tmp_cluster_count)
//    for (int i=0;i< datasets_count[l];i++){
//      int q=0;
//      int initial_pos=0;
//      for (int k=0;k < i;k++)
//        initial_pos+=tmp_cluster_count[k];
//
//      iftDict *dict = iftCreateDict();
//
//      for (int s=0;s <datasets_hierarchy[l][i]->nsamples;s++) {
//        /* check if the sample s is a cluster root in its dataset */
//        if (s == datasets_hierarchy[l][i]->sample[s].label){
//          /*insert s in the dictionary */
//          iftInsertIntoDict(s,q,dict);
//
//          /* copy all data from the sample. Note that we'are copying the same features, we'll overwrite this afterward*/
//          iftCopySample(&datasets_hierarchy[l][i]->sample[s],&last_level_dataset->sample[initial_pos+q],orig_dataset->nfeats,true);
////          printf("copying sample with id: %d of dataset %d to the next level to position %d\n",datasets_hierarchy[l][i]->sample[s].id,i,q);
//          q++;
//
//          /* check if there were already found all cluster roots of the current dataset */
//          if (q == tmp_cluster_count[i]){
//#pragma omp critical
//            found_roots+=q;
//            break;
//          }
//
//        }
//      }
//
//      /* create vectors to accumulate the mean features of each cluster. The representatives of each cluster will
//       * pass to the next level with these mean features*/
//      float **mean_feats=(float **)iftAlloc(tmp_cluster_count[i],sizeof(float *));
//      for (int c=0;c<tmp_cluster_count[i];c++)
//        mean_feats[c]=iftAllocFloatArray(orig_dataset->nfeats);
//
//      int *samples_count=iftAllocIntArray(tmp_cluster_count[i]);
//
//      int index;
//      int label;
//      for (int s=0;s <datasets_hierarchy[l][i]->nsamples;s++) {
//        label=datasets_hierarchy[l][i]->sample[s].label;
//        index=(int)iftGetLongValFromDict(label,dict);
//        samples_count[index]++;
//        /* accumulate the mean feature values */
//        for (int f = 0; f < orig_dataset->nfeats; f++)
//          mean_feats[index][f] += datasets_hierarchy[l][i]->sample[s].feat[f];
//      }
//
//      /* update the samples in the next level with the mean of features of these cluster*/
//      for (int c=0;c<tmp_cluster_count[i];c++){
//        for (int f=0;f<orig_dataset->nfeats;f++){
//          last_level_dataset->sample[initial_pos+c].feat[f]=mean_feats[c][f]/samples_count[c];
//        }
////        printf("modified features of sample with id: %d of dataset %d to the next level to position %d\n",last_level_dataset->sample[initial_pos+c].id,i,c);
//      }
//
//      iftDestroyDict(&dict);
//      iftFree(samples_count);
//      for (int c=0;c<tmp_cluster_count[i];c++)
//        iftFree(mean_feats[c]);
//      iftFree(mean_feats);
//    }
//
////    printf("Dataset in the level %d\n",l+1);
////    for (int i=0;i<last_level_dataset->nsamples;i++)
////      printf("id: %d",last_level_dataset->sample[i].id);
////    printf("\n");
//
//    printf("The dataset in the level %d has %d samples\n",l+1,last_level_dataset->nsamples);
//    printf("The id of the last sample in level %d is %d\n",l+1,last_level_dataset->sample[clusters_count[l]-1].id);
//    printf("The dataset in level %d has all dataset roots of the level %d -> %d\n",l+1,l,found_roots==clusters_count[l]);
//
//    iftFree(tmp_cluster_count);
//  }
//
//  /* We create the last level, in this level we don't have to break the dataset, we cluster it directly. Check also that the dataset is stored in the last_level_dataset variable */
////  datasets_hierarchy[hierarchy_level_count-1]=&last_level_dataset;
//
//  datasets_count[hierarchy_level_count-1]=1;
//  /* Mark all samples as train samples in the last level */
//  iftSetStatus(last_level_dataset,TRAIN);
//
//  int kmax= (int) iftMax(k_max_percent*last_level_dataset->ntrainsamples,1);
//  iftKnnGraph *graph=iftCreateKnnGraph(last_level_dataset,kmax);
//
//  /* cluster the samples of the the last level dataset propagating the regularly cluster labels*/
//  clusters_count[hierarchy_level_count-1]=iftUnsupTrain(graph, iftGraphCutFun);
//
//
//  printf("The resultant k in the dataset of level %d is %d\n",hierarchy_level_count-1,graph->k);
//  printf("The resultant cluster count in the dataset of level %d is %d\n",hierarchy_level_count-1,clusters_count[hierarchy_level_count-1]);
//
//  /* Checks if the original dataset is supervised. If it is, propagates in the dataset of the second level, the true labels of the roots instead of the cluster labels */
//  if (orig_dataset->nclasses > 0){
//    iftPropagateClusterTrueLabels(graph);
//  }
//
//  /* destroys the last level graph */
//  iftDestroyKnnGraph(&graph);
//
//  /* propagates the label of the upper level dataset */
//#pragma omp parallel for shared(orig_dataset,last_level_dataset)
//  for (int s=0;s<last_level_dataset->nsamples;s++){
//    int sample_id=last_level_dataset->sample[s].id;
//    orig_dataset->sample[sample_id].label=last_level_dataset->sample[s].label;
//  }
//  orig_dataset->nlabels=last_level_dataset->nlabels;
//
////  printf("Orig dataset after propagating the upper level labels\n");
////  for(int i=0;i<orig_dataset->nsamples;i++)
////    if (orig_dataset->sample[i].label == 1)
////      printf("%d ",orig_dataset->sample[i].id);
////  printf("\n");
//
//
//  iftDestroyDataSet(&last_level_dataset);
//  /* Propagates down the labels in the remaining hierarchy. Traverse the datasets of each level, begining with the upper and finishing in the first level. For each dataset i, all samples of i contain as label the root index of the clustering result in this dataset. This roots already have its real label in the original dataset, because these labels were propagated from the upper levels of the hierarchy. So, for each sample s of dataset i, we find the label of its root in the original dataset and assign it as the label of s in the original dataset*/
//
//  for (int l=hierarchy_level_count-2;l>=0;l--){
//#pragma omp parallel for shared(orig_dataset,datasets_hierarchy,datasets_count)
//    for (int d=0;d<datasets_count[l];d++){
//      for (int s=0;s<datasets_hierarchy[l][d]->nsamples;s++){
//        int sample_id=datasets_hierarchy[l][d]->sample[s].id;
//        int root=datasets_hierarchy[l][d]->sample[s].label;
//        int root_id=datasets_hierarchy[l][d]->sample[root].id;
//        orig_dataset->sample[sample_id].label=orig_dataset->sample[root_id].label;
//      }
//    }
//
//    /* destroy the dataset array of the current level*/
//    iftDestroyDataSetArray(&datasets_hierarchy[l],datasets_count[l],true);
//  }
//
//  iftFree(datasets_count);
//  iftFree(clusters_count);
//  iftFree(datasets_hierarchy);
//
//  /* return the found cluster number */
//  return orig_dataset->nlabels;
//
//}
//
///* In this function I'm using all samples as train samples int the cluster process, I means I'm not propagating labels through the function iftUnsupClassify. This method assumes that the dataset is an image. */
//int iftImageDivideAndConquerMeanByBlocksUnsupOPF(iftDataSet *orig_dataset, int num_blocks, iftKnnGraphCutFun iftGraphCutFun, float k_max_percent, int hierarchy_level_count){
//
//  if (orig_dataset->ref_data_type != IFT_REF_DATA_IMAGE && orig_dataset->ref_data_type != IFT_REF_DATA_MIMAGE && orig_dataset->ref_data_type != IFT_REF_DATA_FIMAGE){
//    iftError("The referenced data type of the dataset is not an image ","iftImageDivideAndConquerByBlocksUnsupOPF");
//  }
//
//  /* check that the number of hierarchy level dont't be less than one*/
//  if (hierarchy_level_count < 1 || hierarchy_level_count > 2)
//    iftError("The hierarchy levels can't be less than 1 or higher than two","iftImageDivideAndConquerUnsupOPF");
//
//  /* if the hierarchy level is one then we don't need to do the divide and conquer strategy, simply cluster the entire dataset */
//  if (hierarchy_level_count ==1){
//
//    iftSetStatus(orig_dataset,TRAIN);
//    if (orig_dataset->ntrainsamples == 0)
//      iftError("No samples for training in dataset","iftDivideAndConquerByBlocksUnsupOPF");
//    int kmax= (int) iftMax(k_max_percent*orig_dataset->ntrainsamples,1);
//    iftKnnGraph *graph=iftCreateKnnGraph(orig_dataset,kmax);
//    int cluster_count=iftUnsupTrain(graph, iftGraphCutFun);
//    printf("The resultant k in the dataset is %d\n",graph->k);
//
//    /* destroys the KNN Graph */
//    iftDestroyKnnGraph(&graph);
//
//    return cluster_count;
//  }
//
//  iftMImage *mimg=(iftMImage*)orig_dataset->ref_data;
//  iftDataSet **first_level_datasets;
//  timer *t1=NULL,*t2=NULL;
//
//  t1=iftTic();
//  int first_level_dataset_count=iftMImageTilesToDataSetArray(mimg,num_blocks,&first_level_datasets);
//  t2=iftToc();
//  printf("There were created %d first level datasets in %f\n",first_level_dataset_count,iftCompTime(t1,t2));
//
////  if (orig_dataset->nfeats == 3){
////    for (int i=0;i<first_level_dataset_count;i++){
////      first_level_datasets[i]->alpha[0] = orig_dataset->alpha[0];
////      first_level_datasets[i]->alpha[1] = orig_dataset->alpha[1];
////      first_level_datasets[i]->alpha[2] = orig_dataset->alpha[2];
////    }
////
////  }
//
//  /* train and classify the first level of datasets*/
//
//  /* the total number of clusters in the first level */
//  int first_level_cluster_total=0;
//  /* saves the number of clusters of each dataset of the first level */
//  int *first_level_cluster_count= iftAllocIntArray(first_level_dataset_count);
//
//#pragma omp parallel for reduction(+:first_level_cluster_total)
//  for (int i=0;i<first_level_dataset_count;i++){
//
//    /* all samples will be considered as train samples */
//    iftSetStatus(first_level_datasets[i],TRAIN);
//
//    if (first_level_datasets[i]->ntrainsamples == 0)
//      iftError("No samples for training in dataset %d of level %d","iftDivideAndConquerByBlocksUnsupOPF",i,0);
//
//    printf("The number of train samples in dataset %d of level %d is %d\n",i,0,first_level_datasets[i]->ntrainsamples);
//
//    int kmax= (int) iftMax(k_max_percent*first_level_datasets[i]->ntrainsamples,1);
//    iftKnnGraph *graph=iftCreateKnnGraph(first_level_datasets[i],kmax);
//
//    first_level_cluster_count[i]=iftUnsupTrainRootMap(graph, iftGraphCutFun);
//    printf("The resultant k in the dataset %d of level %d is %d\n",i,0,graph->k);
//    printf("The resultant cluster count in the dataset %d of level %d is %d\n",i,0,first_level_cluster_count[i]);
//
//    first_level_cluster_total+=first_level_cluster_count[i];
//
//    /* destroys the KNN Graph */
//    iftDestroyKnnGraph(&graph);
//  }
//
//  /* creates a new dataset with the representatives of the first level clusters. This will be the second level in the hierarchy*/
//  iftDataSet *second_level_dataset=iftCreateDataSet(first_level_cluster_total,orig_dataset->nfeats);
//
//  printf("The dataset in the level %d has %d samples\n",1,first_level_cluster_total);
//  second_level_dataset->ref_data_type=orig_dataset->ref_data_type;
//  second_level_dataset->ref_data=orig_dataset->ref_data;
//
////  if (orig_dataset->nfeats == 3){
////    second_level_dataset->alpha[0] = orig_dataset->alpha[0];
////    second_level_dataset->alpha[1] = orig_dataset->alpha[1];
////    second_level_dataset->alpha[2] = orig_dataset->alpha[2];
////  }
//
//  int found_roots=0;
//#pragma omp parallel for shared(found_roots)
//  for (int i=0;i< first_level_dataset_count;i++) {
//    int q = 0;
//    int initial_pos = 0;
//    for (int k = 0; k < i; k++)
//      initial_pos += first_level_cluster_count[k];
//
//    iftDict *dict = iftCreateDict();
//    for (int s = 0; s < first_level_datasets[i]->nsamples; s++) {
//
//      /* check if the sample is a cluster root in its dataset */
//      if (s == first_level_datasets[i]->sample[s].label) {
//        /*insert s in the dictionary */
//        iftInsertIntoDict(s, q, dict);
//
//        /* copy all data from the sample. Note that we'are copying the same features, we'll overwrite this afterward*/
//        iftCopySample(&first_level_datasets[i]->sample[s], &second_level_dataset->sample[initial_pos + q],
//                      orig_dataset->nfeats, true);
//        q++;
//        /* check if we already found all cluster roots in the current dataset */
//        if (q == first_level_cluster_count[i]) {
//#pragma omp critical
//          found_roots += q;
//          break;
//        }
//
//      }
//    }
//
//    /* create vectors to accumulate the mean features of each cluster. The representatives of each cluster will pass to the next level with these mean features*/
//    float **mean_feats = (float **) iftAlloc(first_level_cluster_count[i], sizeof(float *));
//    for (int c = 0; c < first_level_cluster_count[i]; c++)
//      mean_feats[c] = iftAllocFloatArray(orig_dataset->nfeats);
//
//    int *samples_count = iftAllocIntArray(first_level_cluster_count[i]);
//
//    int index;
//    int label;
//    for (int s = 0; s < first_level_datasets[i]->nsamples; s++) {
//      label = first_level_datasets[i]->sample[s].label;
//      index = (int) iftGetLongValFromDict(label, dict);
//      samples_count[index]++;
//      /* accumulate the mean feature values */
//      for (int f = 0; f < orig_dataset->nfeats; f++)
//        mean_feats[index][f] += first_level_datasets[i]->sample[s].feat[f];
//    }
//
//    /* update the samples in the next level with the mean of features of these cluster*/
//    for (int c = 0; c < first_level_cluster_count[i]; c++)
//      for (int f = 0; f < orig_dataset->nfeats; f++)
//        second_level_dataset->sample[initial_pos + c].feat[f] = mean_feats[c][f] / samples_count[c];
//
//    iftDestroyDict(&dict);
//    iftFree(samples_count);
//    for (int c=0;c<first_level_cluster_count[i];c++)
//      iftFree(mean_feats[c]);
//    iftFree(mean_feats);
//
//  }
//
//  iftFree(first_level_cluster_count);
//  printf("The last sample id of the second level dataset is %d\n",second_level_dataset->sample[first_level_cluster_total-1].id);
//  printf("The second level dataset has all the roots of the first level datasets %d\n",found_roots==first_level_cluster_total);
//
//  /* Mark all samples as train samples in the second level */
//  iftSetStatus(second_level_dataset,TRAIN);
//
//  int kmax= (int) iftMax(k_max_percent*second_level_dataset->ntrainsamples,1);
//
//  iftKnnGraph *graph=iftCreateKnnGraph(second_level_dataset,kmax);
//
//  /* cluster the samples of the the second level dataset propagating the regularly cluster labels*/
//  int second_level_cluster_number=iftUnsupTrain(graph, iftGraphCutFun);
//
//  printf("The resultant k in the second level dataset is %d\n",graph->k);
//  printf("The resultant cluster count in the second level dataset is %d\n",second_level_cluster_number);
//
//  /* Checks if the original dataset is supervised. If it is, propagates in the dataset of the second level, the true labels of the roots instead of the cluster labels */
//  if (orig_dataset->nclasses > 0){
//    iftPropagateClusterTrueLabels(graph);
//  }
//
//  /* destroys the second level graph */
//  iftDestroyKnnGraph(&graph);
//
//  /* Propagates down the labels of the dataset of the second level */
//#pragma omp parallel for
//  for (int s=0;s<second_level_dataset->nsamples;s++){
//    int sample_id=second_level_dataset->sample[s].id;
//    orig_dataset->sample[sample_id].label=second_level_dataset->sample[s].label;
//  }
//  orig_dataset->nlabels=second_level_dataset->nlabels;
//
//  /* Destroys the second level dataset */
//  iftDestroyDataSet(&second_level_dataset);
//
//  /* Traverse the datasets of the first level of the hierarchy. For each dataset i, all samples of i contain as label the root index of the clustering result in this dataset. This roots already have its real label in the original dataset, because these labels were propagated from the second level of the hierarchy. So, for each sample s of dataset i, we find the label of its root in the original dataset and assign it as the label of s in the original dataset*/
//#pragma omp parallel for
//  for (int i=0;i< first_level_dataset_count;i++){
//    for (int s=0;s< first_level_datasets[i]->nsamples;s++){
//      int sample_id=first_level_datasets[i]->sample[s].id;
//      int sample_root=first_level_datasets[i]->sample[s].label;
//      int root_id=first_level_datasets[i]->sample[sample_root].id;
//      orig_dataset->sample[sample_id].label=orig_dataset->sample[root_id].label;
//    }
//  }
//
//  /* destroys all datasets of the first level of the hierarchy*/
//  iftDestroyDataSetArray(&first_level_datasets,first_level_dataset_count,true);
//  iftFree(first_level_datasets);
//
//  return orig_dataset->nlabels;
//}

/* In this function I'm using all samples as train samples int the cluster process, I means I'm not propagating labels through the function iftUnsupClassify. This method assumes that the dataset contains an image. This is a divide and conquer approach but here I'm staying at the first level */
//int iftImageDivideAndConquerByBlocksOnlyFirstLevelUnsupOPF(iftDataSet *orig_dataset, int num_blocks,
//                                                           iftKnnGraphCutFun iftGraphCutFun, float k_max_percent){
//
//  if (orig_dataset->ref_data_type != IFT_REF_DATA_IMAGE && orig_dataset->ref_data_type != IFT_REF_DATA_MIMAGE && orig_dataset->ref_data_type != IFT_REF_DATA_FIMAGE){
//    iftError("The referenced data type of the dataset is not an image ","iftImageDivideAndConquerByBlocksUnsupOPF");
//  }
//
//  iftDataSet **first_level_datasets;
//  timer *t1=NULL,*t2=NULL;
//
//  t1=iftTic();
//  int first_level_dataset_count=iftMImageTilesToDataSetArray(orig_dataset,num_blocks,&first_level_datasets,false);
//  t2=iftToc();
//  printf("There were created %d first level datasets in %f\n",first_level_dataset_count,iftCompTime(t1,t2));
//
//  /* the total number of clusters in the first level */
//  int first_level_cluster_total=0;
//  /* saves the number of clusters of each dataset of the first level */
//  int *first_level_cluster_count= iftAllocIntArray(first_level_dataset_count);
//
//#pragma omp parallel for reduction(+:first_level_cluster_total)
//  for (int i=0;i<first_level_dataset_count;i++){
//
//    /* all samples will be considered as train samples */
//    iftSetStatus(first_level_datasets[i],TRAIN);
//
//    if (first_level_datasets[i]->ntrainsamples == 0)
//      iftError("No samples for training in dataset %d of level %d","iftDivideAndConquerByBlocksUnsupOPF",i,0);
//
//    printf("The number of samples in dataset %d of level %d is %d\n",i,0,first_level_datasets[i]->nsamples);
//
//    printf("The number of train samples in dataset %d of level %d is %d\n",i,0,first_level_datasets[i]->ntrainsamples);
//
//    int kmax= (int) iftMax(k_max_percent*(first_level_datasets[i]->ntrainsamples),1);
//    iftKnnGraph *graph=iftCreateKnnGraph(first_level_datasets[i],kmax);
//
//    first_level_cluster_count[i]=iftFastUnsupTrain(graph, iftGraphCutFun);
//    printf("The resultant k in the dataset %d of level %d is %d\n",i,0,graph->k);
//    printf("The resultant cluster count in the dataset %d of level %d is %d\n",i,0,first_level_cluster_count[i]);
//
//    first_level_cluster_total+=first_level_cluster_count[i];
//
//    if (orig_dataset->nclasses > 0){
//      iftPropagateClusterTrueLabels(graph);
//      iftUnsupClassify(graph, first_level_datasets[i]);
//      printf("The purity of the clustering in dataset %d is %f\n",i,iftTruePositives(first_level_datasets[i]));
//    }
//
//    /* destroys the KNN Graph */
//    iftDestroyKnnGraph(&graph);
//  }
//
//  //  for (int s=0;s<first_level_datasets[83]->nsamples;s++){
////    for (int f=0;f<first_level_datasets[83]->nfeats;f++)
////      printf("%f ",first_level_datasets[83]->sample[s].feat[f]);
////    printf("\n");
////  }
////  printf("-------------------------------------------------------------------\n");
////  for (int s=0;s<first_level_datasets[82]->nsamples;s++){
////    for (int f=0;f<first_level_datasets[82]->nfeats;f++)
////      printf("%f ",first_level_datasets[82]->sample[s].feat[f]);
////    printf("\n");
////  }
//
//  if (orig_dataset->nclasses>0){
//    /*propagate the labels as they are, there is no problem in sharing the labels between the partitions because the true labels were already propagated in each partition independently*/
//#pragma omp parallel for
//    for (int i=0;i< first_level_dataset_count;i++){
//      for (int s=0;s< first_level_datasets[i]->nsamples;s++){
//        int sample_id=first_level_datasets[i]->sample[s].id;
//        orig_dataset->sample[sample_id].label=first_level_datasets[i]->sample[s].label;
//      }
//    }
//    orig_dataset->nlabels=orig_dataset->nclasses;
//
//  }
//  else{
//    /* Traverse the datasets of the first level and propagate their labels to the original dataset. Note that we can't mix labels between differents datasets*/
//#pragma omp parallel for
//    for (int i=0;i< first_level_dataset_count;i++){
//      int initial_label=0;
//      for(int k=0;k<i;k++)
//        initial_label+=first_level_cluster_count[k];
//
//      for (int s=0;s< first_level_datasets[i]->nsamples;s++){
//        int sample_id=first_level_datasets[i]->sample[s].id;
//        orig_dataset->sample[sample_id].label=first_level_datasets[i]->sample[s].label+initial_label;
//      }
//    }
//    orig_dataset->nlabels=first_level_cluster_total;
//
//  }
//
//  /* destroys all datasets of the first level of the hierarchy*/
//  iftDestroyDataSetArray(&first_level_datasets,first_level_dataset_count,false);
//  iftFree(first_level_datasets);
//  iftFree(first_level_cluster_count);
//
//  return first_level_cluster_total;
//}

///* In this function I'm using all samples as train samples int the cluster process, I means I'm not propagating labels through the function iftUnsupClassify. This method assumes that the dataset is an image. */
//int iftImageDivideAndConquerByBlocksUnsupOPF(iftDataSet *orig_dataset, int num_blocks, iftKnnGraphCutFun iftGraphCutFun, float k_max_perc1, float k_max_perc2){
//
//    if (orig_dataset->ref_data_type != IFT_REF_DATA_IMAGE && orig_dataset->ref_data_type != IFT_REF_DATA_MIMAGE && orig_dataset->ref_data_type != IFT_REF_DATA_FIMAGE){
//        iftExit("The referenced data type of the dataset is not an image ", "iftImageDivideAndConquerByBlocksUnsupOPF");
//    }
//
//    iftDataSet **first_level_datasets;
//
//    int first_level_dataset_count=iftMImageTilesToDataSetArray(orig_dataset,num_blocks,&first_level_datasets,false);
//    /*print the real block number*/
//    printf("%d\t",first_level_dataset_count);
//
//
//    /* train and classify the first level of datasets*/
//
//    /* the total number of clusters in the first level */
//    int first_level_cluster_total=0;
//    /* saves the number of clusters of each dataset of the first level */
//    int *first_level_cluster_count= iftAllocIntArray(first_level_dataset_count);
//
//    printf("%d\t",orig_dataset->nsamples);
//
//#pragma omp parallel for reduction(+:first_level_cluster_total)
//    for (int i=0;i<first_level_dataset_count;i++){
//
//        /* all samples will be considered as train samples */
//        iftSetStatus(first_level_datasets[i],TRAIN);
//
//        if (first_level_datasets[i]->ntrainsamples == 0)
//            iftExit("No samples for training in dataset %d of level %d", "iftDivideAndConquerByBlocksUnsupOPF", i, 0);
//
////    printf("The number of train samples in dataset %d of level %d is %d\n",i,0,first_level_datasets[i]->ntrainsamples);
//
//        int kmax= (int) iftMax(k_max_perc1 * first_level_datasets[i]->ntrainsamples, 1);
//
//        iftKnnGraph *graph=iftCreateKnnGraph(first_level_datasets[i],kmax);
//        first_level_cluster_count[i]=iftUnsupTrainRootMap(graph, iftGraphCutFun);
////    printf("The resultant k in the dataset %d of level %d is %d\n",i,0,graph->k);
////    printf("The resultant cluster count in the dataset %d of level %d is %d\n",i,0,first_level_cluster_count[i]);
//
//        first_level_cluster_total+=first_level_cluster_count[i];
//
//        /* destroys the KNN Graph */
//        iftDestroyKnnGraph(&graph);
//    }
//
//    /* creates a new dataset with the representatives of the first level clusters. This will be the second level of the hierarchy*/
//    iftDataSet *second_level_dataset=iftCreateDataSetWithNoFeatVectors(first_level_cluster_total,orig_dataset->nfeats);
//
//    printf("%d\t",second_level_dataset->nsamples);
//
////  printf("The dataset in the level %d has %d samples\n",1,first_level_cluster_total);
//    second_level_dataset->ref_data_type=orig_dataset->ref_data_type;
//    second_level_dataset->ref_data=orig_dataset->ref_data;
//
//    int found_roots=0;
//#pragma omp parallel for shared(found_roots)
//    for (int i=0;i< first_level_dataset_count;i++){
//        int q=0;
//        int initial_pos=0;
//        for (int k=0;k < i;k++)
//            initial_pos+=first_level_cluster_count[k];
//
//        for (int s=0;s <first_level_datasets[i]->nsamples;s++){
//
//            /* check if the sample is a cluster root in its dataset */
//            if (s == first_level_datasets[i]->sample[s].label){
//                iftCopySample(&first_level_datasets[i]->sample[s],&second_level_dataset->sample[initial_pos+q],
//                              orig_dataset->nfeats,false);
//                q++;
//                /* check if we already found all cluster roots in the current dataset */
//                if (q == first_level_cluster_count[i]){
//#pragma omp atomic
//                    found_roots+=q;
//                    break;
//                }
//
//            }
//        }
//    }
//
//    /* Mark all samples as train samples in the second level */
//    iftSetStatus(second_level_dataset,TRAIN);
//
//    int kmax= (int) iftMax(k_max_perc2 * second_level_dataset->ntrainsamples, 1);
//
//    iftKnnGraph *graph=iftCreateKnnGraph(second_level_dataset,kmax);
//
//    /* cluster the samples of the the second level dataset propagating the regularly cluster labels*/
//    iftFastUnsupTrain(graph, iftGraphCutFun);
//
////  printf("The resultant k in the second level dataset is %d\n",graph->k);
////  printf("The resultant cluster count in the second level dataset is %d\n",second_level_cluster_number);
//
//    /* Checks if the original dataset is supervised. If it is, propagates in the dataset of the second level, the true labels of the roots instead of the cluster labels */
//    int cluster_nb=second_level_dataset->nlabels;
//
//    if (orig_dataset->nclasses > 0){
//        iftPropagateClusterTrueLabels(graph);
//    }
//    orig_dataset->nlabels=second_level_dataset->nlabels;
//
//    /* destroys the second level graph */
//    iftDestroyKnnGraph(&graph);
//
//    /* Propagates down the labels of the dataset of the second level */
//#pragma omp parallel for
//    for (int s=0;s<second_level_dataset->nsamples;s++){
//        int sample_id=second_level_dataset->sample[s].id;
//        orig_dataset->sample[sample_id].label=second_level_dataset->sample[s].label;
//    }
//
//    /* Destroys the second level dataset */
//    iftDestroyDataSetWithNoFeatVectors(&second_level_dataset);
//
//    /* Traverse the datasets of the first level of the hierarchy. For each dataset i, all samples of i contain as label the root index of the clustering result in this dataset. This roots already have its real label in the original dataset, because these labels were propagated from the second level of the hierarchy. So, for each sample s of dataset i, we find the label of its root in the original dataset and assign it as the label of s in the original dataset*/
//#pragma omp parallel for
//    for (int i=0;i< first_level_dataset_count;i++){
//        for (int s=0;s< first_level_datasets[i]->nsamples;s++){
//            int sample_id=first_level_datasets[i]->sample[s].id;
//            int sample_root=first_level_datasets[i]->sample[s].label;
//            int root_id=first_level_datasets[i]->sample[sample_root].id;
//            orig_dataset->sample[sample_id].label=orig_dataset->sample[root_id].label;
//        }
//    }
//
//    /* destroys all datasets of the first level of the hierarchy*/
//    iftDestroyDataSetArray(&first_level_datasets,first_level_dataset_count,false);
//    iftFree(first_level_datasets);
//    iftFree(first_level_cluster_count);
//
//    return cluster_nb;
//}

int iftDivideAndConquerRandomlyUnsupOPF2(iftDataSet *orig_dataset, int nb_partitions, float train_perc,
                                         iftKnnGraphCutFun iftGraphCutFun, float k_max_perc1, float k_max_perc2) {

  iftDataSet **first_level_datasets;

  int first_level_ds_count= iftSplitDatasetIntoRandomlyDataSetArray(orig_dataset, nb_partitions,
                                                                    &first_level_datasets, false);

  int *first_level_cluster_count= iftAllocIntArray(first_level_ds_count);
  int first_level_cluster_total=0;

  /* cluster the datasets of the first level level*/
#pragma omp parallel for reduction(+:first_level_cluster_total)
  for (int i=0;i<first_level_ds_count;i++){

    /* select training samples */
    iftSelectUnsupTrainSamples(first_level_datasets[i],train_perc,100);

    if (first_level_datasets[i]->ntrainsamples == 0)
      iftExit("No samples for training in dataset %d of level %d", "iftDivideAndConquerRandomlyUnsupOPF2Levels", i, 1);

    int kmax= (int) iftMax((k_max_perc1 * first_level_datasets[i]->ntrainsamples), 1);
    iftKnnGraph *graph=iftCreateKnnGraph(first_level_datasets[i],kmax);

    first_level_cluster_count[i]= iftUnsupTrainRootMap(graph, iftGraphCutFun, 0);

    /* classify the test samples in the dataset*/
    iftUnsupClassify(graph, first_level_datasets[i]);

    first_level_cluster_total+=first_level_cluster_count[i];

    /* destroys the KNN Graph */
    iftDestroyKnnGraph(&graph);
  }

  /* creates a new dataset with the representatives of the first level clusters. This will be the second level of the hierarchy*/
  iftDataSet *second_level_dataset= iftCreateDataSetWithoutFeatsVector(first_level_cluster_total,
                                                                       orig_dataset->nfeats);

  second_level_dataset->ref_data_type=orig_dataset->ref_data_type;
  second_level_dataset->ref_data=orig_dataset->ref_data;

  int found_roots=0;
#pragma omp parallel for shared(found_roots)
  for (int i=0;i< first_level_ds_count;i++){
    int q=0;
    int initial_pos=0;
    for (int k=0;k < i;k++)
      initial_pos+=first_level_cluster_count[k];

    for (int s=0;s <first_level_datasets[i]->nsamples;s++){

      /* check if the sample is a cluster root in its dataset */
      if (s == first_level_datasets[i]->sample[s].label){
        iftCopySample(&first_level_datasets[i]->sample[s],&second_level_dataset->sample[initial_pos+q],
                      orig_dataset->nfeats,false);
        q++;
        /* check if we already found all cluster roots in the current dataset */
        if (q == first_level_cluster_count[i]){
#pragma omp atomic
          found_roots+=q;
          break;
        }

      }
    }
  }

  /* Mark all samples as train samples in the second level */
  iftSetStatus(second_level_dataset,TRAIN);

  int kmax= (int) iftMax((k_max_perc2 * second_level_dataset->ntrainsamples), 1);

  iftKnnGraph *graph=iftCreateKnnGraph(second_level_dataset,kmax);

  /* cluster the samples of the the second level dataset propagating the regularly cluster labels*/
  iftFastUnsupTrain(graph, iftGraphCutFun);

  /* Checks if the original dataset is supervised. If it is, propagates in the dataset of the second level, the true labels of the roots instead of the cluster labels */
  int cluster_nb=second_level_dataset->nlabels;

  if (orig_dataset->nclasses > 0){
    iftPropagateClusterTrueLabels(graph);
  }
  orig_dataset->nlabels=second_level_dataset->nlabels;

  /* destroys the second level graph */
  iftDestroyKnnGraph(&graph);

  /* Propagates down the labels of the dataset of the second level */
#pragma omp parallel for
  for (int s=0;s<second_level_dataset->nsamples;s++){
    int sample_id=second_level_dataset->sample[s].id;
    orig_dataset->sample[sample_id].label=second_level_dataset->sample[s].label;
  }

  /* Destroys the second level dataset */
  iftDestroyDataSetWithoutFeatVectors(&second_level_dataset);

  /* Traverse the datasets of the first level of the hierarchy. For each dataset i, all samples of i contain as label the root index of the clustering result in this dataset. This roots already have its real label in the original dataset, because these labels were propagated from the second level of the hierarchy. So, for each sample s of dataset i, we find the label of its root in the original dataset and assign it as the label of s in the original dataset*/
#pragma omp parallel for
  for (int i=0;i< first_level_ds_count;i++){
    for (int s=0;s< first_level_datasets[i]->nsamples;s++){
      int sample_id=first_level_datasets[i]->sample[s].id;
      int sample_root=first_level_datasets[i]->sample[s].label;
      int root_id=first_level_datasets[i]->sample[sample_root].id;
      orig_dataset->sample[sample_id].label=orig_dataset->sample[root_id].label;
    }
  }

  /* destroys all datasets of the first level of the hierarchy*/
  iftDestroyDataSetArray(&first_level_datasets,first_level_ds_count,false);
  iftFree(first_level_cluster_count);

  return cluster_nb;

}
