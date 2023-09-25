#include "ift.h"


void iftMyPDFByRange(iftKnnGraph *graph)
{
  int           u,v,i,j,s,t;
  iftAdjSet    *adj_u=NULL,*adj_v=NULL;
  iftDataSet   *Z=graph->Z;
  //float K = 2.0*graph->maxarcw[graph->k]*graph->maxarcw[graph->k]/9.0;
  float maximum=-INFINITY_FLT, minimum=INFINITY_FLT;

  if (graph->kmax == 0)
    iftError("This is not a Knn-Graph\n","iftPDFByRange");

  // Compute probability density function

  for (u=0; u < graph->nnodes; u++){
    s = graph->node[u].sample;

    if (graph->node[u].adjplat!=NULL)
      iftDestroySet(&(graph->node[u].adjplat)); 
    Z->sample[s].weight = 0.0; 

    for (adj_u=graph->node[u].adj,i=1;i <= graph->k;i++,adj_u = adj_u->next) {
      Z->sample[s].weight += adj_u->arcw; /* simplification to speed
					     up computation */
      //Z->sample[s].weight += expf((-adj_u->arcw*adj_u->arcw)/K);
      graph->node[u].maxarcw = adj_u->arcw; // update radius for the distance to the farthest among the best k closest neighbors
    }
    Z->sample[s].weight = graph->maxarcw[graph->k] - (Z->sample[s].weight/graph->k);
    //Z->sample[s].weight = Z->sample[s].weight / graph->k;
    if (Z->sample[s].weight > maximum) 
      maximum = Z->sample[s].weight;
    if (Z->sample[s].weight < minimum)
      minimum = Z->sample[s].weight;
  }
  
  if (maximum > minimum){ /* it is mandatory to keep normalization */
    for (u=0; u < graph->nnodes; u++){
      s = graph->node[u].sample;
      Z->sample[s].weight = ((MAXWEIGHT-1.0)*(Z->sample[s].weight-minimum)/(maximum-minimum))+1.0;
    }
  }
  
  // Add adjacent nodes on density plateaus
  
  for (u=0; u < graph->nnodes; u++){
    s = graph->node[u].sample;    
    for (adj_u=graph->node[u].adj,i=1;i <= graph->k;i++,adj_u=adj_u->next){
      v = adj_u->node; 
      t = graph->node[v].sample;    

      if (Z->sample[t].weight == Z->sample[s].weight){
	for (adj_v=graph->node[v].adj,j=1;j <= graph->k; j++,adj_v=adj_v->next){
	  if (u == adj_v->node)
	    break;
	}
	if (j > graph->k)
	  iftInsertSet(&(graph->node[v].adjplat),u);	  
      }      
    }
  }

}

int iftMyUnsupTrain(iftKnnGraph *graph, iftKnnGraphCutFun iftGraphCutFun)
{
  int ngroups;
  
  //  fprintf(stdout,"Estimating the best k from [1-%d]\n",graph->kmax);
  //iftBestkByKnnGraphCutMSPS(graph,iftGraphCutFun);
  iftBestkByKnnGraphCut(graph,iftGraphCutFun);
  //  fprintf(stdout,"The best k is %d\n",graph->k);  
  // fprintf(stdout,"Computing PDF\n");
  iftMyPDFByRange(graph);
  // fprintf(stdout,"Creating the OPF unsupervised classifier\n");
  ngroups=iftUnsupOPF(graph);

  return(ngroups);
}


iftKnnGraph *iftMyCreateKnnGraph(iftDataSet *Z, int kmax)
{
    iftKnnGraph *graph=(iftKnnGraph *)calloc(1,sizeof(iftKnnGraph));
    int          k,n,u,v,s,t,nnodes=Z->ntrainsamples;
    float        dist, *d;
    int         *nn;
    iftAdjSet   *adj;
   
    if (nnodes == 0){
      iftError("No samples for training","iftCreateKnnGraph");
    }
    if ((kmax > nnodes)||(kmax < 0))
      iftError("Invalid number kmax of arcs","iftCreateKnnGraph");

    if (kmax==0)
      kmax=1;

    graph->nnodes = nnodes;
    graph->node   = (iftKnnNode *)calloc(nnodes,sizeof(iftKnnNode));
    if (graph->node == NULL){
      iftError(MSG1,"iftCreateKnnGraph");
    }
    
    graph->pathval       = iftAllocFloatArray(nnodes);
    graph->ordered_nodes = iftAllocIntArray(nnodes);
    graph->Q        = iftCreateFHeap(nnodes,graph->pathval);
    graph->maxarcw  = iftAllocFloatArray(kmax+1);
    graph->kmax     = kmax;
    graph->k        = kmax;
    graph->Z        = Z;

    for (u=0; u < graph->nnodes; u++){
      graph->node[u].adj      = NULL;
      graph->node[u].adjplat  = NULL;
      graph->node[u].sample   = NIL;
      graph->node[u].maxarcw  = 0.0;
      graph->node[u].root     = u;
    }

    d=iftAllocFloatArray(kmax+2);
    nn=iftAllocIntArray(kmax+2);

    u = 0;
    for (s=0; s < Z->nsamples; s++){
      if (Z->sample[s].status == IFT_TRAIN){
	graph->node[u].sample = s;
	u++;
      }
    }

    for (u=0; u < graph->nnodes; u++){

      s = graph->node[u].sample;

      for (k=1; k <= kmax; k++){
	d[k]=INFINITY_FLT;
	graph->maxarcw[k] = -INFINITY_FLT;
      }
      
      // Compute the k closest training nodes of u
      
      for (v=0; v < graph->nnodes; v++){
	if (u != v){
	  t = graph->node[v].sample;
	  if (iftDist == NULL)
	    d[kmax+1] = Z->iftArcWeight(Z->sample[s].feat,Z->sample[t].feat,Z->alpha,Z->nfeats);
	  else
	    d[kmax+1] = iftDist->distance_table[Z->sample[s].id][Z->sample[t].id];
	  nn[kmax+1] = v;
	  k          = kmax+1;  
	  while ((k > 1)&&(d[k]<d[k-1])){ // sort in the increasing
	                                    // order of distance	  
	    dist   = d[k];    n      = nn[k];
	    d[k]   = d[k-1]; nn[k]   = nn[k-1];
	    d[k-1] = dist;   nn[k-1] = n;
	    k--;
	  }
	}
      }

      /* Set an initial maximum arc weight for each node and insert k-nearest
	 neighbors in the adjacency list, taking into account that
	 insertion must keep their increasing order of distance.
      */

      graph->node[u].maxarcw = d[MAX(kmax/2,1)]; // Median value makes it more robut to outliers. iftPDFByRange updates this parameter with the best maximum arc weight for each node. 
 
      for (k=kmax; k >= 1; k--){ // Insertion in AdjSet is LIFO
	iftInsertAdjSet(&(graph->node[u].adj),nn[k],d[k]);
      }
    }

    // Compute the maximum arc weight in the graph for each value of k.

    for (u=0; u < graph->nnodes; u++){
      for (adj = graph->node[u].adj, k=1; k <= kmax; k++, adj = adj->next){
	if (adj->arcw > graph->maxarcw[k]){
	  graph->maxarcw[k] = adj->arcw;
	}
      }
    }


  free(d);
  free(nn);
  
  return(graph);
}

iftKnnGraph *iftMyUnsupLearn(iftDataSet *Z, float kmax_perc, iftKnnGraphCutFun iftGraphCutFun, int niterations)
{
  int          kmax,  t, I, noutliers, new_samples;
  iftKnnGraph *graph=NULL;
  float        train_perc = (float)Z->ntrainsamples/Z->nsamples;

  if (Z->ntrainsamples == 0)
    iftError("No samples for training","iftUnsupLearn");

  kmax  = (int)(kmax_perc*Z->ntrainsamples);
  noutliers = Z->ntrainsamples + 1;

  for (I=1; (I <= niterations)&&(noutliers > Z->ntrainsamples); I++) {
    // Computing new graph
    if (graph != NULL)
      iftDestroyKnnGraph(&graph);
    graph = iftMyCreateKnnGraph(Z,kmax);
    iftUnsupTrain(graph,iftGraphCutFun);
    noutliers=iftUnsupClassify(graph,Z);

    if( noutliers > Z->ntrainsamples ) {
      // Take at last a new sample.

      new_samples = noutliers * train_perc;

      if( new_samples == 0 )
	new_samples = 1;
      
      printf("Iteration %d: training samples %d, noutliers %d, new samples %d\n", I, Z->ntrainsamples ,noutliers, new_samples );
      
      // Insert outliers in the training set.
      while(new_samples > 0) {
	t = iftRandomInteger(0,Z->nsamples-1);
	if (Z->sample[t].status == IFT_OUTLIER) {
	  Z->sample[t].status = IFT_TRAIN;
	  new_samples--;
	  Z->ntrainsamples++;
	}
      }

      // Reset the remaining outliers to the IFT_TEST state.
      for (t=0; t < Z->nsamples; t++) {
	if (Z->sample[t].status == IFT_OUTLIER)
	  Z->sample[t].status = IFT_TEST;
      }

      kmax  = (int)(kmax_perc*Z->ntrainsamples);
     
    }
  }
  
  return(graph);
}

iftMMKernel *iftMyUnsupLearnKernelsFromDataSet(iftDataSet* Z,iftAdjRel* A, int nsubsamples,float kmax_perc, char whitening)
{
  iftDataSet  *Zp,*Zk=NULL;
  iftKnnGraph *graph=NULL;
  iftMMKernel *K;
  int          nkernels,nbands;

  if ( nsubsamples == 0 ) nsubsamples = Z->nsamples;

  if ((kmax_perc >  1.0))
    iftError("Invalid percentage of adjacent samples","iftUnsupLearnKernelsFromDataSet");

  if (whitening)
    Zp = iftWhiteningTransform(Z);
  else
    Zp = Z;

  if (kmax_perc >= 0) {
    /* Compute clustering using normalized graph-cut for best k estimation */
    if (Zp->nsamples <= nsubsamples) {
      fprintf(stdout,"starting with %d and kmax (%d)\n",Zp->ntrainsamples,(int)(kmax_perc*Zp->ntrainsamples+0.5));
      graph = iftMyCreateKnnGraph(Zp,(int)(kmax_perc*Zp->nsamples+0.5));
      fprintf(stdout,"starting with %d and kmax (%d)\n",Zp->ntrainsamples,(int)(kmax_perc*Zp->ntrainsamples+0.5));
      iftMyUnsupTrain(graph,iftNormalizedCut);
    } else {
      iftSelectUnsupTrainSamples(Zp,(float)nsubsamples/(float)Zp->nsamples);
      fprintf(stdout,"starting with %d and kmax (%d)\n",Zp->ntrainsamples,(int)(kmax_perc*Zp->ntrainsamples+0.5));
      graph = iftMyUnsupLearn(Zp,kmax_perc,iftNormalizedCut,10);
    }
    if (Z != Zp)
      for(int s = 0; s < Zp->nsamples; s++)
	Z->sample[s].label = Zp->sample[s].label;
  }
  else {
    /* Initialization of Centroids */
    if (whitening)
      Zk = iftKmeansInitCentroidsRandomNormal(Zp,(int)(-kmax_perc+0.5));
    else
      // Zk = iftKmeansInitCentroidsRandomNormal(Zp,(int)(-kmax_perc+0.5));
      Zk = iftKmeansInitCentroidsFromSamples(Zp,(int)(-kmax_perc+0.5));

    /* Kmeans */
    iftKmeansRun(Zp, &Zk, 50,1E-5);
    // labels from Zp to Z
    if (Z != Zp)
      for(int s = 0; s < Zp->nsamples; s++)
	Z->sample[s].label = Zp->sample[s].label;
  }
  
  if ( Zk ==NULL) {
    /* Select kernels from root samples */
    nkernels = 0;
    for (int u=0; u < graph->nnodes; u++)  {
      if (graph->node[u].root==u) {
	nkernels ++;
      }
    }
  } else {
    nkernels = Zk->nsamples;
  }
  nbands = Zp->nfeats/A->n;

  printf("Creating bank with %d kernels \n",nkernels);

  K = iftCreateMMKernel(A,nbands,nkernels); //img->m 
  if (Zk == NULL) {
    int k=0;
    for (int u=0; u < graph->nnodes; u++)  {
      if (graph->node[u].root == u) {
	int s = graph->node[u].sample;
	int j = 0;
	for (int b=0; b < nbands; b++) { // img->m  
	  for (int i=0; i < A->n; i++) {
	    K->weight[k][b].val[i] = Zp->sample[s].feat[j];
	    j++;
	  }
	}
	k++;
      }
    }
  } else {
    for (int s=0; s < Zk->nsamples; s++)  {
      int j = 0;
      for (int b=0; b < nbands; b++) {
	for (int i=0; i < A->n; i++) {
	  K->weight[s][b].val[i] = Zk->sample[s].feat[j];
	  j++;
	}
      }
    }
  }

  if (Zp->fsp.W != NULL){
    K->W     = iftCopyMatrix(Zp->fsp.W);
    K->mean  = iftAllocFloatArray(Zp->nfeats);
    K->stdev = iftAllocFloatArray(Zp->nfeats);
    for (int i=0; i < Zp->nfeats; i++) {
      K->mean[i]  = Zp->fsp.mean[i];
      K->stdev[i] = Zp->fsp.stdev[i];
    }
  }

  if (graph != NULL)
    iftDestroyKnnGraph(&graph);
  if (Z != Zp)
    iftDestroyDataSet(&Zp);
  if (Zk != NULL)
    iftDestroyDataSet(&Zk);

  return(K);
}


int main(int argc, char *argv[]) 
{
  FILE* fp;
  char  *pP,base[200],path[200],database[200],filenameOut[200];
  float kmax_perc;
  int   split,layer,patchsize,nsubsamples,nsamples;

  iftDataSet      *Z=NULL;
  iftMMKernel     *K=NULL;
  iftKnnGraph     *graph=NULL;
  timer           *t1=NULL,*t2=NULL;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc != 4)
      iftError("Usage: iftDataClusterByOPF <database.split.9.layer.9.patches.999999.999.dat> <subsampling> <kmax_perc [0,1]>","main");

  if ( (pP=strrchr(argv[1],'/')) ) {
    strcpy(base,pP+1);
    strcpy(path,argv[1]);
    path[strrchr(path,'/')-path+1]='\0';
  } else {
    strcpy(base,argv[1]);
    strcpy(path,"");
  }
  fprintf(stdout,"%s: %s\n",path,base);

  nsubsamples   = atoi(argv[2]);
  kmax_perc     = atof(argv[3]);

  // iftReadOPFDataSetRAW
  if (sscanf(base,"%[^.].split.%d.layer.%d.patches.%d.%d.dat",database,&split,&layer,&nsamples,&patchsize) != 5)
    iftError("input data name is out of format","iftDataClusterByOPFraw");

  fprintf(stdout,"database: %s, split: %d, layer: %d, nsamples: %d, patchsize: %d\n",database,split,layer,nsamples,patchsize);
  if ( (fp=fopen(argv[1],"rb"))==NULL )
    iftError("Opening input data error","iftDataClusterByOPFraw");

  Z = iftCreateDataSet(nsamples,patchsize);

  //  Z->nclasses = 0;
  /* Read features of each sample */
  for (int s=0; s < nsamples; s++){
    if(fread((Z->sample[s].feat), sizeof(float), patchsize, fp)!= patchsize)
      iftError("Reading error","iftDataCusterByOPFraw");

    Z->sample[s].id    = s;
  }
  fclose(fp);
  fprintf(stdout,"%d samples read!\n",nsamples);
  // iftReadOPFDataSetRAW

  // asserting reading
  if (patchsize == 9) { 
    fprintf(stdout,"patches[%d,%d] = %f\n",0,0,Z->sample[0].feat[0]);
    if (nsamples >=  19180) fprintf(stdout,"patches[%d,%d] = %f\n",19180,5,Z->sample[19180].feat[5]);
    if (nsamples >= 100000) fprintf(stdout,"patches[%d,%d] = %f\n",99999,8,Z->sample[99999].feat[8]);
  } else if (patchsize == 1600) {
    fprintf(stdout,"patches[%d,%d] = %f\n",0,0,Z->sample[0].feat[0]);
    if (nsamples >=  62745) fprintf(stdout,"patches[%d,%d] = %f\n",62745,715,Z->sample[62745].feat[715]);
    if (nsamples >= 100000) fprintf(stdout,"patches[%d,%d] = %f\n",99999,1599,Z->sample[99999].feat[1599]);
  } else if (patchsize == 3200) {
    fprintf(stdout,"patches[%d,%d] = %f\n",0,0,Z->sample[0].feat[0]);
    if (nsamples >=  97262) fprintf(stdout,"patches[%d,%d] = %f\n",97262,2329,Z->sample[97262].feat[2329]);
    if (nsamples >= 100000) fprintf(stdout,"patches[%d,%d] = %f\n",99999,3199,Z->sample[99999].feat[3199]);
  }
  fflush(stdout);

  iftSetStatus(Z,IFT_TRAIN);

  iftRandomSeed(IFT_RANDOM_SEED);

  int kmax_init = (int)(kmax_perc*nsubsamples+0.5);

  iftAdjRel* A;
  /*
  if (patchsize == 9)
    A = iftRectangular(3,3);
  else if (patchsize == 1600)
    A = iftRectangular(40,40);
  else if (patchsize == 3200)
    A = iftRectangular(40,80);
  if (A->n != patchsize) {
    char msg[200];
    sprintf(msg,"Irregular patchsize (%d)",A->n);
    iftError(msg,"iftDataClusterByOPFraw");
  }
  */
  A = iftCreateAdjRel(patchsize);

  t1      = iftTic();
  K = iftMyUnsupLearnKernelsFromDataSet(Z,A,nsubsamples,kmax_perc,0);
  //  graph = iftCreateKnnGraph(Z,(int)((kmax_perc*Z->nsamples)+0.5));
  //  iftUnsupTrain(graph,iftNormalizedCut);
  t2     = iftToc();

  iftDestroyAdjRel(&A);

  float time = iftCompTime(t1,t2);
  if (kmax_perc > 0)
    fprintf(stdout,"clustering in %f ms (%f mins) with %d groups\n",time,time/60000.,Z->nlabels);
  else
    fprintf(stdout,"clustering in %f ms (%f mins) with %d groups\n",time,time/60000.,K->nkernels);

  // iftWriteOPFDataSetRAW
  fprintf(stdout,"nkernels: %d, nbands: %d\n",K->nkernels,K->nbands);

  if (kmax_perc > 0)
    sprintf(filenameOut,"%s%s.split.%d.layer.%d.kernels.%d.%d.opf.dist.%d.%d.dat",path,database,split,layer,K->nkernels,patchsize,nsubsamples,kmax_init);
  else
    sprintf(filenameOut,"%s%s.split.%d.layer.%d.kernels.%d.%d.kmeans.dist.%d.dat",path,database,split,layer,K->nkernels,patchsize,nsubsamples);
  
  if ( (fp=fopen(filenameOut,"wb"))==NULL )
    iftError("Opening output data error","iftDataClusterByOPFraw");
  
  for (int k=0; k < K->nkernels; k++) {
    for (int b=0; b < K->nbands; b++) {	  
      if (fwrite(K->weight[k][b].val,sizeof(float),K->A->n,fp) != K->A->n) 
	iftError("Writing error","iftWriteMMKernel");
    }
  }
  fclose(fp);

  iftDestroyDataSet(&Z);
  iftDestroyKnnGraph(&graph);
  iftDestroyMMKernel(&K);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}
