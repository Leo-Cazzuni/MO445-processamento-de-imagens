#include "ift.h"

/* 
   Author: Alexandre Falcão 

   Description: This code computes cluster purity and class
   separability from local feature space reductions based on PCA for
   each sample s in a given training set. Local datasets centered at
   each s and a sparse distance table from s to its K closest
   neighbors are created and then used for cluster purity and class
   separability computation. Actually, the reduction from the original
   feature space to local feature spaces of dimension N < K is
   optional. It is done only when N is greater than 0.

   Date: January 1st, 2020. 

*/

/*
********************** Beginning of the code *************************
*/

/* 
   Creates one dataset per sample from the original training set
   Z. These local datasets are centered at each sample s and its
   samples are the K closest neighbors in the original feature
   space. The id information of each K closest sample is its
   corresponding sample in the original feature space. For 0 < N < K,
   it also applies dimensionality reduction by PCA to the local
   datasets. 
*/

iftDataSet **iftLocalDataSetsWithKClosestSamples(iftDataSet *Z, int K, int N)
{
  iftDataSet **Zlocal = (iftDataSet **) calloc(Z->nsamples, sizeof(iftDataSet *));

  if (Z->nsamples != Z->ntrainsamples)
    iftError("It requires a training set","iftLocalDataSetsWithKClosestSamples");
  
  if ((K < 1) || (K > Z->nsamples))
    iftError("Invalid number for the K=%d closest samples --- K must be in [1,%d].","iftLocalDataSetsWithKClosestSamples",K,Z->nsamples);

  if (N >= Z->nfeats)
    iftError("Invalid number of features %d for the reduced space --- N must be less than %d.","iftLocalDataSetsWithKClosestSamples",Z->nfeats);

  if (K <= N)
    iftError("The dimension %d of the reduced space must be less than K = %d","iftLocalDataSetsWithKClosestSamples",N,K); 

  /* (1) Creates the local datasets with the K closest samples in the
     original feature space. */

#pragma omp parallel for
  for (int s=0; s < Z->nsamples; s++){
    
    int  nn[K+1];
    float d[K+1];
  
    for (int k=0; k < K; k++){
      d[k]  = IFT_INFINITY_FLT;
      nn[k] = -1;
    }
  
    int  i;
    int  k = 0;
    
    for (int t=0; t < Z->nsamples; t++){
      if (s != t){
	d[k]   = iftSquaredFeatDistance(Z->sample[s].feat,Z->sample[t].feat,Z->nfeats);
	nn[k]  = t;
	i      = k;
	while ((i >= 1) && (d[i]<d[i-1])){ /* sort in the increasing order of distance from s */
	  float dist = d[i];
	  int      j = nn[i];
	  d[i]       = d[i-1]; nn[i]   = nn[i-1];
	  d[i-1]     = dist;   nn[i-1] = j;
	  i--;
	}
	if (k<K) k++;
      }
    }
    
    Zlocal[s] = iftCreateDataSet(K,Z->nfeats);
  
    for (int k=0; k < K; k++){
      Zlocal[s]->sample[k].id = nn[k];
      for (int f=0; f < Z->nfeats; f++)
	Zlocal[s]->sample[k].feat[f] = Z->sample[nn[k]].feat[f]-Z->sample[s].feat[f]; 
    }
    iftSetStatus(Zlocal[s],IFT_TRAIN);  
    Zlocal[s]->fsp.mean   = iftAllocFloatArray(Z->nfeats);
    Zlocal[s]->fsp.nfeats = Z->nfeats;
    for (int f=0; f < Z->nfeats; f++)
      Zlocal[s]->fsp.mean[f] = Z->sample[s].feat[f];    
  }
  
  /* (2) If this is an option, apply local feature space reduction by
     PCA. */

  if (N > 0) { /* Apply local feature space reduction by PCA. */
    
    iftDataSet **Zreduc = (iftDataSet **) calloc(Z->nsamples, sizeof(iftDataSet *));

    for (int s=0; s < Z->nsamples; s++){    
      Zreduc[s] = iftTransFeatSpaceByPCA(Zlocal[s], N);
      iftDestroyDataSet(&Zlocal[s]);
    }
    iftFree(Zlocal);
    
    return(Zreduc);
  }
  
  return(Zlocal);
}

/* 
   Computes a sparse distance table with the distance from each sample
   s and its K closest samples in the local datasets. It stores
   IFT_NIL when a sample is not among the K closest ones from
   s. Therefore, it cannot be used as regular distance tables. It
   requires special treatment to avoid those disconnections between
   samples.

*/

iftDistanceTable *iftDistanceTableFromLocalDataSets(iftDataSet **Zlocal, int nelems)
{
  iftDistanceTable *dist = iftCreateDistanceTable(nelems,nelems);

  for (int s=0; s < nelems; s++){
    for (int t=0; t < nelems; t++){
      if (s != t)
  	dist->distance_table[s][t]=IFT_NIL;
      else
  	dist->distance_table[s][t]=0.0;
    }
  }

  for (int s=0; s < nelems; s++){  
    for (int t=0; t < Zlocal[s]->nsamples; t++){
      float d = 0.0;
      for (int f=0; f < Zlocal[s]->nfeats; f++)
	d += Zlocal[s]->sample[t].feat[f]*Zlocal[s]->sample[t].feat[f];
      d = sqrtf(d);
      dist->distance_table[s][Zlocal[s]->sample[t].id] = d;
    }
  }
  
  return(dist);
}

/* Creates a K nearest neighbor graph from a sparse distance table,
   which contains IFT_NIL on nonexistent arcs. */

iftKnnGraph *iftCreateKnnGraphFromSparseDistanceTable(iftDataSet *Z, int kmax, iftDistanceTable *dist)
 {
   iftKnnGraph *graph=(iftKnnGraph *)iftAlloc(1,sizeof(iftKnnGraph));
   int          nnodes=Z->ntrainsamples;
   iftAdjSet   *adj;

    if (nnodes == 0){
        iftError("No samples for training", "iftCreateKnnGraphFromSparseDistanceTable");
    }
    if ((kmax >= nnodes)||(kmax < 0))
        iftError("Invalid number kmax of arcs %d", "iftCreateKnnGraphFromSparseDistanceTable", kmax);

    if (kmax==0)
        kmax=1;

    graph->nnodes = nnodes;
    graph->node   = (iftKnnNode *)iftAlloc(nnodes,sizeof(iftKnnNode));
    if (graph->node == NULL){
        iftError(MSG_MEMORY_ALLOC_ERROR, "iftCreateKnnGraphFromSparseDistanceTable");
    }

    graph->pathval       = iftAllocFloatArray(nnodes);
    graph->ordered_nodes = iftAllocIntArray(nnodes);
    graph->Q        = iftCreateFHeap(nnodes,graph->pathval);
    graph->maxarcw  = iftAllocFloatArray(kmax+1);
    graph->kmax     = kmax;
    graph->k        = kmax;
    graph->Z        = Z;
    
    for (int u=0; u < graph->nnodes; u++){
        graph->node[u].adj      = NULL;
        graph->node[u].adjplat  = NULL;
        graph->node[u].sample   = IFT_NIL;
        graph->node[u].maxarcw  = 0.0;
        graph->node[u].root     = u;
    }

    int j = 0;
    for (int s=0; s < Z->nsamples; s++){
      if (iftHasSampleStatus(Z->sample[s], IFT_TRAIN)){
	  graph->node[j].sample = s;
	  j++;
        }
    }


#pragma omp parallel for
    for (int u=0; u < graph->nnodes; u++){

        float *d=iftAllocFloatArray(kmax+2);
        int *nn=iftAllocIntArray(kmax+2);

        int s = graph->node[u].sample;

        for (int k=1; k <= kmax; k++){
            d[k]= IFT_INFINITY_FLT;
            nn[k]=-1;
        }

        // Compute the k closest training nodes of node u
        int  i;
        int k  = 2;
        for (int v=0; v < graph->nnodes; v++){
            if (u != v){
                int t = graph->node[v].sample;
		if (dist->distance_table[s][t]!=IFT_NIL)
		  d[k] = iftDist->distance_table[s][t];		                   
                nn[k]  = v;
                i      = k;
                while ((i > 1) && (d[i]<d[i-1])){ // sort in the increasing
                    // order of distance
                    float dist   = d[i];
                    int n = nn[i];
                    d[i]   = d[i-1]; nn[i]   = nn[i-1];
                    d[i-1] = dist;   nn[i-1] = n;
                    i--;
                }
                if (k<=kmax) k++;
            }
        }

        /* Set an initial maximum arc weight for each node and insert
	   k-nearest neighbors in the adjacency list, taking into
	   account that insertion must keep their increasing order of
	   distance. Median value makes it more robust to
	   outliers. iftPDFByRange updates this parameter with the
	   best maximum arc weight for each node. */

        graph->node[u].maxarcw = d[iftMax(kmax / 2, 1)]; 
	
        for (k=kmax; k >= 1; k--){ // Insertion in AdjSet is LIFO
            iftInsertAdjSet(&(graph->node[u].adj),nn[k],d[k]);
        }
	
        iftFree(d);
        iftFree(nn);
    }

    // Compute the maximum arc weight in the graph for each value of
    // k.
    for (int k=1; k <= kmax; k++){
        graph->maxarcw[k] = IFT_INFINITY_FLT_NEG;
    }

    int k;
    for (int u=0; u < graph->nnodes; u++){
        for (adj = graph->node[u].adj,k=1; k <= kmax; k++, adj = adj->next){
            if (adj->arcw > graph->maxarcw[k]){
                graph->maxarcw[k] = adj->arcw;
            }
        }
    }

    return(graph);
}

/* Destroy local datasets */

void iftDestroyLocalDataSets(iftDataSet ***Z, int nelems)
{
  if (*Z != NULL) {
    iftDataSet **aux = *Z;
    for (int s = 0; s < nelems; s++)
      iftDestroyDataSet(&aux[s]);
    iftFree(aux);
    *Z = NULL;
  }
}

/* 
   Computes class separabilities and their average based on a sparse
   distance table. The parameter K indicates that there are only K
   columns with existing arcs (i.e., distances) from each sample (row)
   s and its K closest neighbors in the dataset. 

*/

iftFloatArray *iftClassSeparabilityFromSparseDistanceTable(iftDataSet *Z, int K, iftDistanceTable *dist)
{
  if (Z->nclasses == 0)
    iftError("It requires an annotated dataset","iftClassSeparabilityFromSparseDistanceTable");
  
  iftFloatArray *separ    = iftCreateFloatArray(Z->nclasses+1); 
  int *nsamples_per_class = iftCountSamplesPerClassDataSet(Z);

  for (int s=0; s < Z->nsamples; s++){
    for (int t=0; t < Z->nsamples; t++){
      if (dist->distance_table[s][t]!=IFT_NIL)
	if (Z->sample[s].truelabel != Z->sample[t].truelabel)
	  separ->val[Z->sample[s].truelabel] += 1;
    }
  }
  
  for (int c=1; c <= Z->nclasses; c++){
    separ->val[c] /= (K*(Z->nsamples-nsamples_per_class[c]));
    separ->val[c] = 1.0 - separ->val[c];
    separ->val[0] += separ->val[c];
  }
  separ->val[0] /= Z->nclasses;

  iftFree(nsamples_per_class);
  
  return(separ);
}

int main(int argc, char *argv[])
{
    timer *tstart = NULL;

    if (argc != 5)
        iftError("Usage: iftClusterPurity <...>\n"
		 "[1] Input training data set.\n"
		 "[2] Input value of the K > 0 closest neighbors.\n"
		 "[3] Input number N < K of features for local feature space reduction\n"
                 "    by PCA, when N > 0. N < 0 implies in no reduction.\n"
		 "[4] Output text file with purity and separability measures.",
                 "main");

    tstart = iftTic();

    iftDataSet    *Ztrain = iftReadDataSet(argv[1]);
    int            K      = atoi(argv[2]);
    int            N      = atoi(argv[3]);
    printf("(1) Computing local feature spaces...\n");
    iftDataSet **Zlocal   = iftLocalDataSetsWithKClosestSamples(Ztrain, K, N);
    printf("(2) Computing sparse distance table...\n");
    iftDist               = iftDistanceTableFromLocalDataSets(Zlocal, Ztrain->nsamples);
    iftDestroyLocalDataSets(&Zlocal,Ztrain->nsamples);
    
    /* Compute cluster purity  and class separability */
    
    printf("(3) Creating knn-graph from sparse distance table... \n");
    iftKnnGraph   *graph  = iftCreateKnnGraphFromSparseDistanceTable(Ztrain,K,iftDist);
    printf("(4) Computing cluster purity\n");    
    iftFloatArray *purity = iftClusterPurity(graph,true);
    printf("(5) Computing class separability\n");
    iftFloatArray *separ  = iftClassSeparabilityFromSparseDistanceTable(Ztrain, K, iftDist);
    iftDestroyKnnGraph(&graph);
    iftDestroyDistanceTable(&iftDist);

    FILE *fp = fopen(argv[4],"w");

    fprintf(fp,"mean purity: %f\n",purity->val[0]);
    for (int c=1; c <= Ztrain->nclasses; c++)
      fprintf(fp,"class %d, purity %f\n",c,purity->val[c]);
    iftDestroyFloatArray(&purity);
    
    fprintf(fp,"mean separability: %f\n",separ->val[0]);
    for (int c=1; c <= Ztrain->nclasses; c++)
      fprintf(fp,"class %d, separability %f\n",c,separ->val[c]);
    iftDestroyFloatArray(&separ);
    fclose(fp);
    iftDestroyDataSet(&Ztrain);

    printf("Done ... %s\n", iftFormattedTime(iftCompTime(tstart, iftToc())));
    return(0);
}
