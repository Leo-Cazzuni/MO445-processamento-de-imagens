#include "suputils.h"
void iftIGraphISFRecomputeSeeds_UpdXYZ(iftIGraph *igraph, int *seed, int nseeds, iftSet **trees_for_removal, iftSet **new_seeds, float *new_seeds_flag, float **seed_features);
int  *iftIGraphSuperpixelCenters_UpdXYZ(iftIGraph *igraph, int *seed, int nseeds, float **seed_features);

int *iftIGraphSuperpixelCenters_UpdXYZ(iftIGraph *igraph, int *seed, int nseeds, float **seed_features)
{
    int    i, j, p, q, s, *center, ndim;
    float  **feat,  *nelems, dist1, dist2, *featp, *featq;
    iftVoxel u, v;
    ndim = 3; // X,Y and Z

    /* compute average feature vector for each superpixel */

    featp = iftAllocFloatArray(ndim);
    featq = iftAllocFloatArray(ndim);

    feat   = (float **)calloc(nseeds,sizeof(float *));
    nelems = iftAllocFloatArray(nseeds);
    center = iftAllocIntArray(nseeds);
    for (i=0; i < nseeds; i++){
        feat[i] = iftAllocFloatArray(ndim);
        center[i] = seed[i];
        /* update seed features NEW */
        for (j = 0; j < igraph->nfeats; ++j) 
          seed_features[i][j] = 0;
    }

    for (s=0; s < igraph->nnodes; s++) {
        p = igraph->node[s].voxel;
        i = igraph->label[igraph->root[p]]-1;
        nelems[i]++;

        u = iftGetVoxelCoord(igraph->index,p);

        feat[i][0] += u.x;
        feat[i][1] += u.y;
        feat[i][2] += u.z;
        /* update seed features NEW */
        for (j = 0; j < igraph->nfeats; ++j) 
          seed_features[i][j] += igraph->feat[p][j];
    }

    for (i=0; i < nseeds; i++) {
        for (j=0; j < ndim; j++)
          feat[i][j] /= nelems[i];
        /* update seed features NEW */
        for (j = 0; j < igraph->nfeats; ++j) 
          seed_features[i][j] /= nelems[i];
    }

    /* compute the closest node to each superpixel center */

    for (s=0; s < igraph->nnodes; s++) {
        p     = igraph->node[s].voxel;
        i     = igraph->label[igraph->root[p]]-1;
        q     = igraph->node[center[i]].voxel;

        u = iftGetVoxelCoord(igraph->index,p);
        v = iftGetVoxelCoord(igraph->index,q);

        featp[0] = u.x;
        featp[1] = u.y;
        featp[2] = u.z;

        featq[0] = v.x;
        featq[1] = v.y;
        featq[2] = v.z;

        dist1 = iftFeatDistance(feat[i],featq,ndim);
        dist2 = iftFeatDistance(feat[i],featp,ndim);
        if (dist2 < dist1)
            center[i]=s;
    }

    for (i=0; i < nseeds; i++) {
        free(feat[i]);
    }
    free(feat);
    free(nelems);

    return(center);
}

void iftIGraphISFRecomputeSeeds_UpdXYZ(iftIGraph *igraph, int *seed, int nseeds, iftSet **trees_for_removal, iftSet **new_seeds, float *new_seeds_flag, float **seed_features)
{
    int      i, *center, p, q, s, min_area;
    iftVoxel u, v;
    float    distColor, distVoxel, distColorThres, distVoxelThres;
    int    *area;

    /* Compute superpixel centers (i.e., the closest node to the center in the feature space) */

    center = iftIGraphSuperpixelCenters_UpdXYZ(igraph, seed, nseeds, seed_features);

    min_area  =  (int)((igraph->nnodes / (float)nseeds) / 5.0);
    area = iftAllocIntArray(nseeds);

    /* Estimate distColorThres */

    distColorThres = 0.0;  distVoxelThres = 0.0;
    for (s=0; s < igraph->nnodes; s++) {
        p     = igraph->node[s].voxel;
        i     = igraph->label[igraph->root[p]]-1;
        q     = igraph->node[seed[i]].voxel;
        u = iftGetVoxelCoord(igraph->index,p);
        v = iftGetVoxelCoord(igraph->index,q);
        distColor = iftFeatDistance(igraph->feat[p],igraph->feat[q],igraph->nfeats);
        distColorThres += distColor;
        distVoxel = iftVoxelDistance(u,v);
        distVoxelThres += distVoxel;
        area[i]++;
    }
    distColorThres /= igraph->nnodes;
    distVoxelThres /= igraph->nnodes;
    distColorThres = sqrtf(distColorThres);
    distVoxelThres = sqrtf(distVoxelThres);

    //printf("distColorThres %f distVoxelThres %f\n", distColorThres, distVoxelThres);

    /* Verify if the centers can be new seeds */
    //distColorThres = 5.0;
    //distVoxelThres = 2.0;

    *new_seeds_flag = 0;
    for (i=0; i < nseeds; i++) {
        p = igraph->node[seed[i]].voxel;
        q = igraph->node[center[i]].voxel;
        u = iftGetVoxelCoord(igraph->index,p);
        v = iftGetVoxelCoord(igraph->index,q);
        distColor = iftFeatDistance(igraph->feat[p],igraph->feat[q],igraph->nfeats);
        distVoxel = iftVoxelDistance(u,v);
        //if ((distColor >= distColorThres) && (distVoxel >= distVoxelThres)) {
        //if ((distColor >= distColorThres) || (distVoxel >= distVoxelThres)) {
        if ((distColor > distColorThres) || (distVoxel > distVoxelThres) || area[i] < min_area) {
            seed[i] = center[i];
            iftInsertSet(new_seeds,center[i]);
            iftInsertSet(trees_for_removal,seed[i]);
            *new_seeds_flag+= 1;
        }
    }
    *new_seeds_flag /= (float)nseeds;

    free(area);
    free(center);
}


int iftIGraphISF1(iftIGraph *igraph, iftImage *seeds, double alpha, double beta, int niters)
{
  double      tmp;
  int        s, t, i, p, q, I, *seed, nseeds, j, index_seed;
  float      new_seeds_flag;
  iftVoxel   u, v;
  iftDHeap  *Q;
  double    *pvalue = iftAllocDoubleArray(igraph->nnodes);
  iftSet    *adj=NULL, *S=NULL, *new_seeds=NULL, *frontier_nodes=NULL, *trees_for_removal=NULL;
  iftSet    *T =NULL; /* Uncomment for the differential version */
  float     **seed_features; // NEW

  timer *t1, *t2;


  new_seeds_flag = 1.0;

  /* Initial seed set and trivial path initialization to infinity
     cost */

  Q      = iftCreateDHeap(igraph->nnodes, pvalue);

  nseeds = 0;
  for (s=0; s < igraph->nnodes; s++) {
    p               = igraph->node[s].voxel;
    pvalue[s]       = igraph->pvalue[p] = IFT_INFINITY_DBL;
    igraph->pred[p] = IFT_NIL;
    if (seeds->val[p]!=0){
      iftInsertSet(&new_seeds,s);
      nseeds++;
    }
  }

  /* Alloc seed features NEW */
  seed_features = (float**) calloc(nseeds ,sizeof(float*));
  for (i = 0; i < nseeds; ++i)
    seed_features[i] = iftAllocFloatArray(igraph->nfeats);

  seed  = iftAllocIntArray(nseeds);
  S     = new_seeds; i = 0;
  while (S != NULL) {
    seed[i] = S->elem;
    p       = igraph->node[seed[i]].voxel;
    igraph->label[p] = i+1;

    // set initial seed feature NEW
    for (j = 0; j < igraph->nfeats; ++j)
      seed_features[i][j] = igraph->feat[p][j];

    i++; S = S->next;
  }

  iftImage *seed_img=iftCreateImage(igraph->index->xsize,igraph->index->ysize,igraph->index->zsize);

  /* differential optimum-path forest computation */

  //for (I=0; (I < niters)&&(new_seeds_flag >= 0.1); I++) {  // If 90% of the seed do not change much (satisfy both the criteria in LAB and XY) we stop
  for (I=0; (I < niters); I++) {  
    
    printf("iteration %d\n",I+1);
    t1 = iftTic();

    if (trees_for_removal != NULL)
      frontier_nodes = iftIGraphTreeRemoval(igraph, &trees_for_removal, pvalue, IFT_INFINITY_DBL);

    while (new_seeds != NULL) { /* insert seeds in the priority queue Q with cost zero */
      s = iftRemoveSet(&new_seeds);
      p = igraph->node[s].voxel;
      pvalue[s] = igraph->pvalue[p] = 0;
      igraph->root[p] = p;
      igraph->pred[p] = IFT_NIL;
      iftInsertDHeap(Q,s);
    }
      
    while (frontier_nodes != NULL) { /* insert frontier nodes in Q to represent the previous seeds */
      s = iftRemoveSet(&frontier_nodes);
      p = igraph->node[s].voxel;
      if (Q->color[s] == IFT_WHITE)
  iftInsertDHeap(Q,s);
    }
    
    for (int s=0; s < igraph->nnodes; s++) {
      p = igraph->node[s].voxel;
      if (igraph->root[p]==p)
  seed_img->val[p]=1;
      else
  seed_img->val[p]=0;
    }

    switch(igraph->type) {

    case IMPLICIT:

      
      while (!iftEmptyDHeap(Q)) {
  s = iftRemoveDHeap(Q);
  p = igraph->node[s].voxel;
  //r = igraph->root[p];
  index_seed = igraph->label[p] - 1;
  //iftVoxel w = iftGetVoxelCoord(igraph->index,r);
  igraph->pvalue[p] = pvalue[s];
  u = iftGetVoxelCoord(igraph->index,p);
  for (i=1; i < igraph->A->n; i++) {
    v = iftGetAdjacentVoxel(igraph->A,u,i);
    if (iftValidVoxel(igraph->index,v)){
      q   = iftGetVoxelIndex(igraph->index,v);
      t   = igraph->index->val[q];
      if ((t != IFT_NIL) && (Q->color[t] != IFT_BLACK)){

        //tmp = pvalue[s] + pow(alpha*(double)iftFeatDistance(igraph->feat[r],igraph->feat[q],igraph->nfeats),beta) + (double)iftVoxelDistance(w,v); // dist from root
        //tmp = pvalue[s] + pow(alpha*(double)iftFeatDistance(igraph->feat[r],igraph->feat[q],igraph->nfeats),beta) + (double)iftVoxelDistance(u,v); 
        tmp = pvalue[s] + pow(alpha*(double)iftFeatDistance(seed_features[index_seed],igraph->feat[q],igraph->nfeats),beta) + (double)iftVoxelDistance(u,v); 

        if (tmp < pvalue[t])
    {
      pvalue[t]            = tmp;
      /* Uncomment for the differential version */
      if (igraph->label[p] != igraph->label[q]){ /* voxels
                  that
                  have
                  changed
                  labels */
        iftInsertSet(&T, q);
      }
      /* end Uncomment */

      igraph->root[q]      = igraph->root[p];
      igraph->label[q]     = igraph->label[p];
      igraph->pred[q]      = p;
      if (Q->color[t] == IFT_GRAY){     
        iftGoUpDHeap(Q, Q->pos[t]);
      } else {
        iftInsertDHeap(Q,t);
      }     
    }
      }
    }
  }
      }

      break;

    case EXPLICIT:

      while (!iftEmptyDHeap(Q)) {
  s = iftRemoveDHeap(Q);
  p = igraph->node[s].voxel;
  //r = igraph->root[p];
  index_seed = igraph->label[p] - 1;
  u = iftGetVoxelCoord(igraph->index,p);
  igraph->pvalue[p] = pvalue[s];

  for (adj=igraph->node[s].adj; adj != NULL; adj=adj->next) {
    t   = adj->elem;
    if (Q->color[t] != IFT_BLACK){      
      q   = igraph->node[t].voxel;
      v   = iftGetVoxelCoord(igraph->index,q);
      //tmp = pvalue[s] + pow(alpha*(double)iftFeatDistance(igraph->feat[r],igraph->feat[q],igraph->nfeats),beta) + (double)iftVoxelDistance(u,v); 
      tmp = pvalue[s] + pow(alpha*(double)iftFeatDistance(seed_features[index_seed],igraph->feat[q],igraph->nfeats),beta) + (double)iftVoxelDistance(u,v); 
        if (tmp < pvalue[t]) {
        /* Uncomment for the differential version */
          if (igraph->label[p] != igraph->label[q]){ /* voxels
                        that have
                        changed
                        labels */
          iftInsertSet(&T, q);
        }
        /* end Uncomment */
        pvalue[t]            = tmp;
        igraph->root[q]      = igraph->root[p];
        igraph->label[q]     = igraph->label[p];
        igraph->pred[q]      = p;
        if (Q->color[t] == IFT_GRAY){     
    iftGoUpDHeap(Q, Q->pos[t]);
        } else {
    iftInsertDHeap(Q,t);
        }     
      }
    }
  }
      }
      break;
      
    case COMPLETE:
      iftError("Not implemented for complete graphs","iftISF");
    }

    
    /* Uncomment this block and comment the one below for differential
       IFT */
    
    
    iftResetDHeap(Q);
    if (I>0)
      iftIGraphFixLabelRootMap(igraph, &T);
    iftDestroySet(&T);
    

    /* End of comment */

    /* Recompute new seeds */


    //iftIGraphISFRecomputeSeeds(igraph, seed, nseeds, &trees_for_removal, &new_seeds, &new_seeds_flag);
    iftIGraphISFRecomputeSeeds_UpdXYZ(igraph, seed, nseeds, &trees_for_removal, &new_seeds, &new_seeds_flag, seed_features);

    t2 = iftToc();
    iftPrintCompTime(t1,t2,"Computational time for iteration %d",I+1);

    /* Uncomment this block and comment the one above for
       non-differential IFT */
    /*
    iftResetDHeap(Q); 
    iftDestroySet(&trees_for_removal);
    iftDestroySet(&new_seeds);

    for (s=0; s < igraph->nnodes; s++) { 
       p = igraph->node[s].voxel; 
       pvalue[s]       = IFT_INFINITY_DBL; 
    } 
    for (int i = 0; i < nseeds; ++i) { 
       s = seed[i]; 
       iftInsertSet(&new_seeds,s); 
       p = igraph->node[s].voxel; 
       igraph->label[p] = i+1; 
    } 
    */

    /* End of comment */
 
  }

  /* Free seed_features NEW */
  for (i = 0; i < nseeds; ++i)
    free(seed_features[i]);
  free(seed_features);

  iftDestroySet(&adj);
  iftDestroySet(&S);
  iftDestroySet(&new_seeds);
  iftDestroySet(&frontier_nodes);
  iftDestroySet(&trees_for_removal);
  iftDestroyDHeap(&Q);
  free(pvalue);
  free(seed);

  return I;
}
