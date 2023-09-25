#include "ift.h"
#define IMAGE_EXT "ppm"

typedef struct ift_graphnode {
    /**  Maximum arc weight from the node to its neighbors */
    float maxarcw;
    /** Corresponding root node in the graph */
    int   root;
    /** Corresponding training sample in the original dataset */
    int   sample;
    /** List of adjacent nodes */
    iftAdjSet *adj;
    /** List of adjacent nodes on plateaus of density */
    iftSet    *adjplat;
} iftGraphNode;

typedef struct ift_graph {
    /** List of nodes in the graph */
    iftGraphNode *node;
    /** List of path value of the nodes */
    float      *pathval;
    /** List of nodes ordered by its path value */
    int        *ordered_nodes;
    /** Number of nodes of the graph */
    int         nnodes;
    /** Priority queue */
    iftFHeap   *Q;
    /** Corresponding dataset */
    iftDataSet *Z;
} iftGraph;

iftGraph *iftCreateGraph(iftDataSet *Z)
{
  iftGraph *graph=(iftGraph *)iftAlloc(1,sizeof(iftGraph));
  int nnodes=Z->ntrainsamples;

  if (nnodes == 0){
      iftExit("No samples for training", "iftCreateKnnGraph");
  }

  graph->nnodes = nnodes;
  graph->node   = (iftGraphNode *)iftAlloc(nnodes,sizeof(iftGraphNode));

  if (graph->node == NULL){
      iftExit(MSG_MEMORY_ALLOC_ERROR, "iftCreateKnnGraph");
  }

  graph->pathval       = iftAllocFloatArray(nnodes);
  graph->ordered_nodes = iftAllocIntArray(nnodes);
  graph->Q        = iftCreateFHeap(nnodes,graph->pathval);
  graph->Z        = Z;

#pragma omp parallel for
  for (int u=0; u < graph->nnodes; u++){
    graph->node[u].adj      = NULL;
    graph->node[u].adjplat  = NULL;
    graph->node[u].sample   = IFT_NIL;
    graph->node[u].maxarcw  = 0.0;
    graph->node[u].root     = u;
  }

  int u = 0;
  for (int s=0; s < Z->nsamples; s++){
    if (Z->sample[s].status == TRAIN){
      graph->node[u].sample = s;
      u++;
    }
  }

  return(graph);
}

void iftDestroyGraph(iftGraph **graph)
{
  int u;
  iftGraph *aux=(*graph);

  if (aux!=NULL){
    for (u=0; u < aux->nnodes; u++){
      if (aux->node[u].adj != NULL){
        iftDestroyAdjSet(&(aux->node[u].adj));
      }
      if (aux->node[u].adjplat != NULL){
        iftDestroySet(&(aux->node[u].adjplat));
      }
    }
    iftFree(aux->node);
    iftFree(aux->pathval);
    iftFree(aux->ordered_nodes);
    iftDestroyFHeap(&(aux->Q));
    iftFree(aux);
    (*graph) = NULL;
  }
}

void iftPDFWithSpatialConstraint(iftGraph *graph, float df, float di)
{

  iftDataSet   *Z=graph->Z;
  /* float K = 2.0*graph->maxarcw[graph->k]*graph->maxarcw[graph->k]/9.0; */
  float maximum= IFT_INFINITY_FLT_NEG, minimum= IFT_INFINITY_FLT;

  iftAdjRel *A;
  A=iftCircular(di);
  float K=2.0*df/9;
  iftMImage *mimg=(iftMImage *)Z->ref_data;

  // Compute probability density function
#pragma omp parallel for shared(maximum,minimum)
  for (int n=0; n < graph->nnodes; n++) {

    int s = graph->node[n].sample;

    Z->sample[s].weight = 0.0;
    int nb_adjacent_samples = 0;

    iftVoxel u = iftMGetVoxelCoord(mimg, s);
    float sample_dist;

    for (int i = 1; i < A->n; i++) {
      iftVoxel v = iftGetAdjacentVoxel(A, u, i);

      if (iftMValidVoxel(mimg, v)) {
        int t = iftMGetVoxelIndex(mimg, v);

        if (iftDist == NULL)
          sample_dist = Z->iftArcWeight(Z->sample[s].feat, Z->sample[t].feat, Z->alpha, Z->nfeats);
        else
          sample_dist = iftDist->distance_table[s][t];

        if (sample_dist <= df) {
          Z->sample[s].weight += exp(-sample_dist / K);
          nb_adjacent_samples++;
          /*here we save the adjacent node */
          iftInsertAdjSet(&(graph->node[n].adj), t, sample_dist);

          if (sample_dist > graph->node[n].maxarcw)
            graph->node[n].maxarcw = sample_dist;
        }

      }
    }

    Z->sample[s].weight /= nb_adjacent_samples;
#pragma omp critical
    {
      if (Z->sample[s].weight > maximum)
        maximum = Z->sample[s].weight;
      if (Z->sample[s].weight < minimum)
        minimum = Z->sample[s].weight;
    }

  }

  if (maximum > minimum){ /* it is mandatory to keep normalization */
#pragma omp parallel for shared(graph,Z)
    for (int u=0; u < graph->nnodes; u++){
      int s = graph->node[u].sample;
      Z->sample[s].weight = ((IFT_MAXWEIGHT - 1.0) * (Z->sample[s].weight - minimum) / (maximum - minimum)) + 1.0;
    }
  }

  // Add adjacent nodes on density plateaus if one is a neighbor of the other and the contrary doesn't happen
#pragma omp parallel for
  for (int u=0; u < graph->nnodes; u++){
    iftAdjSet   *adj_u,*adj_v=NULL;
    int s = graph->node[u].sample;
    for (adj_u=graph->node[u].adj;adj_u != NULL;adj_u=adj_u->next){
      int v = adj_u->node;
      int t = graph->node[v].sample;

      if (Z->sample[t].weight == Z->sample[s].weight){
        char adjplat=0;
        for (adj_v=graph->node[v].adj;adj_v != NULL; adj_v=adj_v->next){
          if (u == adj_v->node){
            adjplat=1;
            break;
          }
        }
        if (!adjplat)
#pragma omp critical
        {
          iftInsertSet(&(graph->node[v].adjplat),u);
        }

      }
    }
  }

  iftDestroyAdjRel(&A);
}

int iftUnsupOPFNew(iftGraph *graph)
{
  int t,l,j,v;
  float tmp;
  iftAdjSet *adj;
  iftSet    *adjplat;
  iftDataSet *Z=graph->Z;

  // Initialization

  iftResetFHeap(graph->Q);
  iftSetRemovalPolicyFHeap(graph->Q, IFT_MAXVALUE);

  for (int u = 0; u < graph->nnodes; u++){
    int s = graph->node[u].sample;
    graph->pathval[u]     = Z->sample[s].weight-1.0;
    graph->node[u].root   = u;
    Z->sample[s].label    = 0;
    iftInsertFHeap(graph->Q, u);
  }

  // Optimum-Path Forest Computation
  l = 1; j = 0;
  int s,u;
  while (!iftEmptyFHeap(graph->Q)){
    u=iftRemoveFHeap(graph->Q);
    graph->ordered_nodes[j]=u; j++;
    s = graph->node[u].sample;

    if (graph->node[u].root == u){ // root node
      graph->pathval[u]    = Z->sample[s].weight;
      Z->sample[s].label   = l;
      l++;
    }

    // extend optimum paths
    for (adj=graph->node[u].adj; adj != NULL; adj = adj->next) {
      v = adj->node;
      t = graph->node[v].sample;

      if (graph->Q->color[v] != IFT_BLACK ) {
        tmp = iftMin(graph->pathval[u], Z->sample[t].weight);
        if (tmp > graph->pathval[v]){
          graph->node[v].root  = graph->node[u].root;
          Z->sample[t].label   = Z->sample[s].label;
          graph->pathval[v]    = tmp;
          iftGoUpFHeap(graph->Q, graph->Q->pos[v]);
        }
      }
    }

    adjplat = graph->node[u].adjplat;  // extend optimum paths on plateaus
    while (adjplat != NULL){
      v = adjplat->elem;
      t = graph->node[v].sample;
      if (graph->Q->color[v] != IFT_BLACK ) {
        tmp = iftMin(graph->pathval[u], Z->sample[t].weight);
        if (tmp > graph->pathval[v]){
          graph->node[v].root  = graph->node[u].root;
          Z->sample[t].label   = Z->sample[s].label;
          graph->pathval[v]    = tmp;
          iftGoUpFHeap(graph->Q, graph->Q->pos[v]);
        }
      }
      adjplat = adjplat->next;
    }

  }

  iftResetFHeap(graph->Q);

  Z->nlabels = l-1;

  return(Z->nlabels);
}

struct problem{
    char *orig_img_path;
    char *gt_labels_path;
    int count_train_img;
    int limit_nb_superp;
    int limit_secs;
    int type_measure; /* 0 for br; 1 for ue*/
    int train_samples;
};


float find_best_args(void* my_problem, float *theta) {

  /*check the theta params*/
  if ( theta[0] <0.003 || theta[0] > 0.1 || theta[1] < 1 || theta[1] > 20 || theta[2] < 10 || theta[2] > 300 )
    return IFT_INFINITY_FLT_NEG;

  char *orig_img_path = ((struct problem *)my_problem)->orig_img_path;
  char *gt_labels_path = ((struct problem *)my_problem)->gt_labels_path;

  iftDir *image_files = iftLoadFilesFromDirBySuffix(orig_img_path, IMAGE_EXT);
  iftDir *gt_label_files = iftLoadFilesFromDirBySuffix(gt_labels_path, "pgm");

  int file_count = ((struct problem *)my_problem)->count_train_img;
  int limit_nb_superp = ((struct problem *)my_problem)->limit_nb_superp;
  int limit_secs = ((struct problem *)my_problem)->limit_secs;
  int type_measure=((struct problem *)my_problem)->type_measure;

  float measure_acc =0.0;
  int superpixels_acc=0;

  iftImage        *img=NULL,*label=NULL,*aux=NULL,*gt_label=NULL,*border=NULL,*gt_border=NULL;
  iftDataSet      *Z=NULL;
  timer           *t1=NULL,*t2=NULL;
  iftAdjRel *A;
  iftMImage *mimg,*eimg;

  int nb_samples_arg=((struct problem *)my_problem)->train_samples;
  float kmax_arg=theta[0];
  int di=(int)theta[1];
  int area_arg=(int)theta[2];
  bool exited=false;

  for (int i=0;i<file_count;i++){

    img=iftReadImageByExt(image_files->files[i]->path);

    /* convert the image to multi-image*/
    if (!iftIsColorImage(img)) {
      mimg = iftImageToMImage(img, GRAYNorm_CSPACE);
      if (img->zsize > 1)
        A = iftSpheric(1.0);
      else
        A = iftCircular(sqrtf(2.0));
      eimg=iftExtendMImageByAdjacencyAndVoxelCoord(mimg,A,1);
      iftDestroyMImage(&mimg);
      iftDestroyAdjRel(&A);
      mimg = eimg;
    }
    else{
      mimg = iftImageToMImage(img, YCbCrNorm_CSPACE);
    }
    Z= iftMImageToDataSet(mimg);

    t1=iftTic();
    iftImage *mask1 = iftSelectImageDomain(mimg->xsize,mimg->ysize,mimg->zsize);
    iftImage *mask_sampling = iftGridSampling(mimg, mask1, nb_samples_arg);

    int train_samples_nb=0;
#pragma omp parallel for reduction(+:train_samples_nb)
    for (int s=0;s<Z->nsamples;s++){
      int voxel=Z->sample[s].id;
      if (mask_sampling->val[voxel]){
        Z->sample[s].status=TRAIN;
        train_samples_nb++;
      }
    }
    Z->ntrainsamples=train_samples_nb;
    iftDestroyImage(&mask1);
    iftDestroyImage(&mask_sampling);

    int kmax = (int) iftMax((kmax_arg * Z->ntrainsamples), 1);
    iftKnnGraph *knn_graph = iftCreateKnnGraph(Z,kmax);
    iftBestkByKnnGraphCut(knn_graph, iftNormalizedCut);
    float df=knn_graph->maxarcw[knn_graph->k];

    iftSetStatus(Z,TRAIN);
    iftGraph *graph= iftCreateGraph(Z);
    iftPDFWithSpatialConstraint(graph,df,di);
    iftUnsupOPFNew(graph);

    t2= iftToc();
    float time = iftCompTime(t1, t2) / 1000;

    /* if the method execution is more than the max time limit discard this configuration of parameters*/
    if (time > (float)limit_secs){
      measure_acc =IFT_INFINITY_FLT_NEG;
      exited=true;
      break;
    }

    label = iftDataSetToLabelImage(Z,false);

    /* do smoothing in the label image*/
    aux = iftSmoothRegionsByDiffusion(label,img,0.5,5);
    iftDestroyImage(&label);
    label=aux;

    /* eliminate short superpixels*/
    aux = iftSelectAndPropagateRegionsAboveArea(label,area_arg);
    iftDestroyImage(&label);
    label=aux;

    superpixels_acc+=iftMaximumValue(label);

    gt_label=iftReadImageByExt(gt_label_files->files[i]->path);
    aux=iftRelabelGrayScaleImage(gt_label);
    iftDestroyImage(&gt_label);
    gt_label=aux;

    if (type_measure){
      /*computing under segmentation error*/
      /*here we are minimizing not maximizing*/
      measure_acc -= iftUnderSegmentation(gt_label,label);
    }
    else{
      /*compute boundary recall*/
      border  = iftBorderImageWithoutMargins(label);
      gt_border=iftBorderImageWithoutMargins(gt_label);
      measure_acc += iftBoundaryRecall(gt_border, border, 2.0);

      iftDestroyImage(&border);
      iftDestroyImage(&gt_border);
    }

    iftDestroyGraph(&graph);
    iftDestroyKnnGraph(&knn_graph);
    iftDestroyImage(&img);
    iftDestroyMImage(&mimg);
    iftDestroyDataSet(&Z);
    iftDestroyImage(&label);

    iftDestroyImage(&gt_label);
  }

  if (!exited){
    float mean_sup=superpixels_acc/file_count;
    if (mean_sup > (float)limit_nb_superp)
      measure_acc =IFT_INFINITY_FLT_NEG;
    else
      measure_acc /=file_count;
  }

  iftDestroyDir(&image_files);
  iftDestroyDir(&gt_label_files);

  return measure_acc;

}

int main(int argc, char *argv[])
{
  iftRandomSeed(IFT_RANDOM_SEED);

  if (argc != 8)
      iftExit("Usage: iftClusterImageByOPFTestingPDFsBestArgsMSPS <dir_orig_img> <dir_label_img> <count_train_img> <limit_nb_superp> <limit_secs> <optimizing measure 0(BR)/1(UE)> <count_train_samples>",
              "main");

  /*doing MSPS*/
  struct problem *my_problem;
  my_problem=(struct problem *)iftAlloc(1,sizeof(struct problem));

  my_problem->orig_img_path= argv[1];
  my_problem->gt_labels_path=argv[2];
  my_problem->count_train_img=atoi(argv[3]);
  my_problem->limit_nb_superp=atoi(argv[4]);
  my_problem->limit_secs=atoi(argv[5]);
  my_problem->type_measure=atoi(argv[6]);
  my_problem->train_samples=atoi(argv[7]);

  iftMSPS* msps = iftCreateMSPS(3, 2, find_best_args,(void *)my_problem);

  msps->theta[0] =0.01; /*kmax*/
  msps->theta[1] =3; /*di*/
  msps->theta[2]=100; /*area*/

  iftMatrixElem(msps->delta, 0, 0) = 0.005;
  iftMatrixElem(msps->delta, 0, 1) = 0.025;

  iftMatrixElem(msps->delta, 1, 0) = 4;
  iftMatrixElem(msps->delta, 1, 1) = 1;

  iftMatrixElem(msps->delta, 2, 0) = 50;
  iftMatrixElem(msps->delta, 2, 1) = 15;

  msps->verbose=true;
//  msps->stopcriteria=0.001;
  msps->niters=200;
  float best_fun_val=iftMSPSMax(msps);

  printf("Best parameters for %s with %d images an limit number superpixels %d and %d seconds with measure-> %s\n",argv[1],atoi(argv[3]),atoi(argv[4]),atoi(argv[5]),atoi(argv[6])?"ue":"br");

  printf("best_numb_samples_arg -> %.4f\n",msps->theta[0]);
  printf("best_kmax_arg -> %.4f\n",msps->theta[1]);
  printf("best_di_arg -> %.4f\n",msps->theta[2]);
  printf("best_area_arg -> %d\n",(int)msps->theta[3]);
  printf("best_objetive_func -> %.4f\n",atoi(argv[6])?-best_fun_val:best_fun_val);

  iftFree(my_problem);
  iftDestroyMSPS(&msps);

  return(0);
}




