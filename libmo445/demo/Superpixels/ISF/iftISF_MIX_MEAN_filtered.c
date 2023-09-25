#include "ift.h"

int filteringThreshold = 0.0; // Set through input argument

iftImage *iftExtract_ISF_MIX_MEAN_Superpixels(iftImage *img, int nsuperpixels, float alpha, float beta, int niters, int smooth_niters, int *nseeds, int *finalniters);

void iftIGraphSubTreeRemoval(iftIGraph *igraph, int s, double *pvalue, double INITIAL_PATH_VALUE, iftDHeap *Q);

int iftIGraphISF_Mean_filtered(iftIGraph *igraph, iftImage *seeds, double alpha, double beta, int niters);

int iftIGraphISF_Root_filtered(iftIGraph *igraph, iftImage *seeds, double alpha, double beta, int niters);

void iftFilterSmallSuperpixels(iftIGraph *igraph, int *seed, int nseeds, iftSet **trees_for_removal, iftSet **new_seeds);

int main(int argc, char *argv[]) 
{
  iftImage  *img, *label;
  iftImage  *gt_borders=NULL, *gt_regions=NULL, *border=NULL;
  iftAdjRel *A;
  int        nautoiters, nseeds;
  char       labelfilename[256];
  char       borderfilename[256];
  iftColor   RGB, YCbCr;
  int        normvalue;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc<9 || argc>11 )
    iftError("Usage: iftISF <image.[pgm,ppm,scn,png]> <nsamples> <alpha (e.g., [0.005-0.2])> <beta (e.g., 12)> <niters (e.g., 10)> <smooth niters (e.g., 2)> <output_label> <filtering_threshold> <ground-truth border image (OPTIONAL)> <ground-truth region image (OPTIONAL)>","main");

  filteringThreshold = atof(argv[8]);

  img  = iftReadImageByExt(argv[1]);

  if (argc >= 10){
    gt_borders = iftReadImageByExt(argv[9]);
    gt_regions = iftReadImageByExt(argv[10]);
  }
  
  normvalue =  iftNormalizationValue(iftMaximumValue(img)); 

  label     = iftExtract_ISF_MIX_MEAN_Superpixels(img, atoi(argv[2]), atof(argv[3]), atof(argv[4]), atoi(argv[5]), atoi(argv[6]), &(nseeds), &(nautoiters));
  
  border  = iftBorderImage(label,0);
  
  A = iftCircular(0.0);
  
  nseeds = iftMaximumValue(label);
  
  // Compute metrics
  if (argc>=10){
    float br, ue, comp, topology;
    br        = iftBoundaryRecall(gt_borders, border, 2.0);
    printf("BR: %f \n", br);
    
    ue       = iftUnderSegmentation(gt_regions, label);
    printf("UE: %f \n", ue);

    comp     = iftCompactness2D(label);
    printf("Comp: %f \n", comp);

    topology = iftTopologyMeasure(label);
    printf("Top: %f \n", topology);
  }
  
  // Write output image
  if (!iftIs3DImage(img)){
    if (argc >= 10){
      RGB.val[0] = normvalue;
      RGB.val[1] = 0;
      RGB.val[2] = normvalue;
      YCbCr      = iftRGBtoYCbCr(RGB, normvalue);
      iftDrawBorders(img,gt_borders,A,YCbCr,A);
    }

    RGB.val[0] = 0;
    RGB.val[1] = 0;
    RGB.val[2] = normvalue;
    
    YCbCr      = iftRGBtoYCbCr(RGB, normvalue);
    sprintf(labelfilename, "%s.pgm", argv[7]);
    iftWriteImageP2(label,labelfilename);
    iftDrawBorders(img,border,A,YCbCr,A);
    sprintf(borderfilename, "%s.png", argv[7]);        
    iftWriteImageByExt(img, borderfilename);
  } else {
    printf("%d\n",iftMaximumValue(label));
    sprintf(labelfilename, "%s.scn", argv[7]);
    iftWriteImage(label,labelfilename);
    RGB.val[2] = RGB.val[1] = RGB.val[0] = iftMaximumValue(img);
    YCbCr      = iftRGBtoYCbCr(RGB, RGB.val[0]);
    iftLabeledSet* Centers = iftGeodesicCenters(label);
    iftDestroyAdjRel(&A);
    A = iftSpheric(1.0);
    while (Centers != NULL) {
      int trash;
      int p = iftRemoveLabeledSet(&Centers,&trash);
      iftVoxel u = iftGetVoxelCoord(img,p);
      iftDrawPoint(img, u, YCbCr, A, iftNormalizationValue(iftMaximumValue(img)));
    }
    sprintf(borderfilename, "%s-centers.scn", argv[7]);
    iftWriteImage(img,borderfilename);
  }

  // Print number of iterations and superpixels
  printf("Number of iterations = %d \n", nautoiters);
  printf("Number of superpixels = %d \n", nseeds);
  
  
  // Free
  iftDestroyImage(&img);
  if (argc>=10) {
    iftDestroyImage(&gt_borders);
    iftDestroyImage(&gt_regions);
  }
  iftDestroyImage(&label);
  iftDestroyImage(&border);
  iftDestroyAdjRel(&A);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
     MemDinInicial,MemDinFinal);   

  return(0);
}

iftImage *iftExtract_ISF_MIX_MEAN_Superpixels(iftImage *img, int nsuperpixels, float alpha, float beta, int niters, int smooth_niters, int *nseeds, int *finalniters) {
  iftImage  *mask1, *seeds, *label;
  iftMImage *mimg;
  iftAdjRel *A;
  iftIGraph *igraph;
  timer     *t1=NULL,*t2=NULL;

  /* Compute ISF superpixels */
  if (iftIs3DImage(img)){
    A      = iftSpheric(1.0);
  } else {
    A      = iftCircular(1.0);
  }

  if (iftIsColorImage(img)){
    mimg   = iftImageToMImage(img,LABNorm_CSPACE);
  } else {
    mimg   = iftImageToMImage(img,GRAY_CSPACE);
  }

  mask1  = iftSelectImageDomain(mimg->xsize,mimg->ysize,mimg->zsize);

  /* minima of a basins manifold in that domain */
  igraph = iftImplicitIGraph(mimg,mask1,A);
  
  t1 = iftTic();
  /* seed sampling for ISF */
  //seeds   = iftGridSampling(mimg,mask1,nsuperpixels);
  seeds   = iftAltMixedSampling(mimg,mask1,nsuperpixels);

  *nseeds = iftNumberOfElements(seeds);

  iftDestroyImage(&mask1);
  iftDestroyMImage(&mimg);

  *finalniters = iftIGraphISF_Mean_filtered(igraph,seeds,alpha,beta,niters);
  //*finalniters = iftIGraphISF_Root_filtered(igraph,seeds,alpha,beta,niters);

  /* Smooth regions in the label map of igraph */  
  if (smooth_niters > 0){
    iftIGraphSetWeightForRegionSmoothing(igraph, img);
    iftIGraphSmoothRegions(igraph, smooth_niters);
  }
  label   = iftIGraphLabel(igraph);
  t2 = iftToc();
  printf("ISF proc time im ms: %f\n", iftCompTime(t1,t2));

  iftImage *res = iftRelabelRegions(label, A);

  iftDestroyImage(&seeds);
  iftDestroyIGraph(&igraph);
  iftDestroyImage(&label);
  iftDestroyAdjRel(&A);

  return res;
}

int iftIGraphISF_Mean_filtered(iftIGraph *igraph, iftImage *seeds, double alpha, double beta, int niters)
{
    double      tmp;
    int        s, t, i, p, q, it, *seed, nseeds, nseeds_init, j, index_seed;
    float      new_seeds_flag;
    iftVoxel   u, v;
    iftDHeap  *Q;
    double    *pvalue = iftAllocDoubleArray(igraph->nnodes);
    iftSet    *adj=NULL, *S=NULL, *new_seeds=NULL, *frontier_nodes=NULL, *trees_for_removal=NULL;
    /* iftSet    *T =NULL; /\* This is part of the old version of the DIFT *\/ */
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
    nseeds_init = nseeds;

    /* Alloc seed features NEW */
    seed_features = (float**) iftAlloc(nseeds ,sizeof(float*));
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


    /* differential optimum-path forest computation */
    for (it=0; (it < niters); it++) {

        printf("iteration %d\n",it+1);
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
            if (Q->color[s] == IFT_WHITE)
                iftInsertDHeap(Q,s);
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

                                tmp = pvalue[s] + pow(alpha*(double)iftFeatDistance(seed_features[index_seed],igraph->feat[q],igraph->nfeats),beta) + (double)iftVoxelDistance(u,v);

                                if (tmp < pvalue[t]){
                                    pvalue[t]            = tmp;

                                    /* /\* This is used for the old differential version *\/ */
                                    /* if (igraph->label[p] != igraph->label[q]){ /\* voxels */
                                    /* 						that */
                                    /* 						have */
                                    /* 						changed */
                                    /* 						labels *\/ */
                                    /*   iftInsertSet(&T, q); */
                                    /* } */

                                    igraph->root[q]      = igraph->root[p];
                                    igraph->label[q]     = igraph->label[p];
                                    igraph->pred[q]      = p;

                                    if (Q->color[t] == IFT_GRAY){
                                        iftGoUpDHeap(Q, Q->pos[t]);
                                    } else {
                                        iftInsertDHeap(Q,t);
                                    }
                                }	else {
                                    if (igraph->pred[q] == p){
                                        if (tmp > pvalue[t]) {
                                            iftIGraphSubTreeRemoval(igraph,t,pvalue,IFT_INFINITY_DBL,Q);
                                        } else { /* tmp == pvalue[t] */
                                            if ((igraph->label[q] != igraph->label[p])&&(igraph->label[q]!=0)){
                                                iftIGraphSubTreeRemoval(igraph,t,pvalue,IFT_INFINITY_DBL,Q);
                                            }
                                        }
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
                            tmp = pvalue[s] + pow(alpha*(double)iftFeatDistance(seed_features[index_seed],igraph->feat[q],igraph->nfeats),beta) + (double)iftVoxelDistance(u,v);


                            if (tmp < pvalue[t]){
                                pvalue[t]            = tmp;

                                /* /\* This is used for the old differential version *\/ */
                                /* if (igraph->label[p] != igraph->label[q]){ /\* voxels */
                                /* 						that */
                                /* 						have */
                                /* 						changed */
                                /* 						labels *\/ */
                                /*   iftInsertSet(&T, q); */
                                /* } */

                                igraph->root[q]      = igraph->root[p];
                                igraph->label[q]     = igraph->label[p];
                                igraph->pred[q]      = p;

                                if (Q->color[t] == IFT_GRAY){
                                    iftGoUpDHeap(Q, Q->pos[t]);
                                } else {
                                    iftInsertDHeap(Q,t);
                                }
                            }	else {
                                if (igraph->pred[q] == p){
                                    if (tmp > pvalue[t]) {
                                        iftIGraphSubTreeRemoval(igraph,t,pvalue,IFT_INFINITY_DBL,Q);
                                    } else { /* tmp == pvalue[t] */
                                        if ((igraph->label[q] != igraph->label[p])&&(igraph->label[q]!=0)){
                                            iftIGraphSubTreeRemoval(igraph,t,pvalue,IFT_INFINITY_DBL,Q);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                break;

            case COMPLETE:
                iftExit("Not implemented for complete graphs", "iftIGraphISF_Mean");
        }


        iftResetDHeap(Q);

        /* /\* This is part of the old version of the DIFT *\/ */

        /* if (it>0) */
        /*     iftIGraphFixLabelRootMap(igraph, &T); */
        /* iftDestroySet(&T); */


        /* End of comment */

        /* Remove small superpixels and their seeds */


        /* Recompute new seeds */

        iftIGraphISFRecomputeSeedsUsingSpatialInformation(igraph, seed, nseeds, &trees_for_removal, &new_seeds, &new_seeds_flag, seed_features);

        iftFilterSmallSuperpixels(igraph, seed, nseeds, &trees_for_removal, &new_seeds);

        t2 = iftToc();
        iftPrintCompTime(t1,t2,"Computational time for iteration %d",it+1);

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
    for (i = 0; i < nseeds_init; ++i)
        iftFree(seed_features[i]);
    iftFree(seed_features);

    iftDestroySet(&adj);
    iftDestroySet(&S);
    iftDestroySet(&new_seeds);
    iftDestroySet(&frontier_nodes);
    iftDestroySet(&trees_for_removal);
    iftDestroyDHeap(&Q);
    iftFree(pvalue);
    iftFree(seed);

    return it;
}

int iftIGraphISF_Root_filtered(iftIGraph *igraph, iftImage *seeds, double alpha, double beta, int niters)
{
    double      tmp;
    int        r, s, t, i, p, q, it, *seed, nseeds;
    float      new_seeds_flag;
    iftVoxel   u, v;
    iftDHeap  *Q;
    double    *pvalue = iftAllocDoubleArray(igraph->nnodes);
    iftSet    *adj=NULL, *S=NULL, *new_seeds=NULL, *frontier_nodes=NULL, *trees_for_removal=NULL;
    //  iftSet    *T =NULL; /* This is used for the old differential version */
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

    seed    = iftAllocIntArray(nseeds);
    S       = new_seeds; i = 0;
    while (S != NULL) {
        seed[i] = S->elem;
        p       = igraph->node[seed[i]].voxel;
        igraph->label[p] = i+1;
        i++; S = S->next;
    }


    /* differential optimum-path forest computation */

    //  for (I=0; (I < niters)&&(new_seeds_flag >= 0.1); I++) {  // If 90% of the seed do not change much (satisfy both the criteria in LAB and XY) we stop
    for (it=0; (it < niters); it++) {


        printf("iteration %d\n",it+1);
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
            if (Q->color[s] == IFT_WHITE){
                iftInsertDHeap(Q,s);
            }
        }


        switch(igraph->type) {

            case IMPLICIT:

                while (!iftEmptyDHeap(Q)) {
                    s = iftRemoveDHeap(Q);
                    p = igraph->node[s].voxel;
                    r = igraph->root[p];
                    igraph->pvalue[p] = pvalue[s];
                    u = iftGetVoxelCoord(igraph->index,p);
                    for (i=1; i < igraph->A->n; i++) {
                        v = iftGetAdjacentVoxel(igraph->A,u,i);
                        if (iftValidVoxel(igraph->index,v)){
                            q   = iftGetVoxelIndex(igraph->index,v);
                            t   = igraph->index->val[q];
                            if ((t != IFT_NIL) && (Q->color[t] != IFT_BLACK)){
                                tmp = pvalue[s] + pow(alpha*(double)iftFeatDistance(igraph->feat[r],igraph->feat[q],igraph->nfeats),beta) + (double)iftVoxelDistance(u,v);

                                if (tmp < pvalue[t]){
                                    pvalue[t]            = tmp;

                                    /* /\* This is used for the old differential version *\/ */
                                    /* if (igraph->label[p] != igraph->label[q]){ /\* voxels */
                                    /* 						that */
                                    /* 						have */
                                    /* 						changed */
                                    /* 						labels *\/ */
                                    /*   iftInsertSet(&T, q); */
                                    /* } */

                                    igraph->root[q]      = igraph->root[p];
                                    igraph->label[q]     = igraph->label[p];
                                    igraph->pred[q]      = p;

                                    if (Q->color[t] == IFT_GRAY){
                                        iftGoUpDHeap(Q, Q->pos[t]);
                                    } else {
                                        iftInsertDHeap(Q,t);
                                    }
                                }	else {
                                    if (igraph->pred[q] == p){
                                        if (tmp > pvalue[t]) {
                                            iftIGraphSubTreeRemoval(igraph,t,pvalue,IFT_INFINITY_DBL,Q);
                                        } else { /* tmp == pvalue[t] */
                                            if ((igraph->label[q] != igraph->label[p])&&(igraph->label[q]!=0)){
                                                iftIGraphSubTreeRemoval(igraph,t,pvalue,IFT_INFINITY_DBL,Q);
                                            }
                                        }
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
                    r = igraph->root[p];
                    u = iftGetVoxelCoord(igraph->index,p);
                    igraph->pvalue[p] = pvalue[s];

                    for (adj=igraph->node[s].adj; adj != NULL; adj=adj->next) {
                        t   = adj->elem;
                        if (Q->color[t] != IFT_BLACK){
                            q   = igraph->node[t].voxel;
                            v   = iftGetVoxelCoord(igraph->index,q);
                            tmp = pvalue[s] + pow(alpha*(double)iftFeatDistance(igraph->feat[r],igraph->feat[q],igraph->nfeats),beta) + (double)iftVoxelDistance(u,v);
                            if (tmp < pvalue[t]) {

                                if (tmp < pvalue[t]){
                                    pvalue[t]            = tmp;

                                    /* /\* This is used for the old differential version *\/ */
                                    /* if (igraph->label[p] != igraph->label[q]){ /\* voxels */
                                    /* 						that */
                                    /* 						have */
                                    /* 						changed */
                                    /* 						labels *\/ */
                                    /*   iftInsertSet(&T, q); */
                                    /* } */

                                    igraph->root[q]      = igraph->root[p];
                                    igraph->label[q]     = igraph->label[p];
                                    igraph->pred[q]      = p;

                                    if (Q->color[t] == IFT_GRAY){
                                        iftGoUpDHeap(Q, Q->pos[t]);
                                    } else {
                                        iftInsertDHeap(Q,t);
                                    }
                                }	else {
                                    if (igraph->pred[q] == p){
                                        if (tmp > pvalue[t]) {
                                            iftIGraphSubTreeRemoval(igraph,t,pvalue,IFT_INFINITY_DBL,Q);
                                        } else { /* tmp == pvalue[t] */
                                            if ((igraph->label[q] != igraph->label[p])&&(igraph->label[q]!=0)){
                                                iftIGraphSubTreeRemoval(igraph,t,pvalue,IFT_INFINITY_DBL,Q);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                break;

            case COMPLETE:
                iftExit("Not implemented for complete graphs", "iftIGraphISF_Root");
        }


        iftResetDHeap(Q);

        /* This is for the old differential version */
        /* if (it>0) */
        /*     iftIGraphFixLabelRootMap(igraph, &T); */
        /* iftDestroySet(&T); */


        /* End of comment */

        /* Remove small superpixels and their seeds */


        /* Recompute new seeds */

        iftIGraphISFRecomputeSeeds(igraph, seed, nseeds, &trees_for_removal, &new_seeds, &new_seeds_flag);

        iftFilterSmallSuperpixels(igraph, seed, nseeds, &trees_for_removal, &new_seeds);

        t2 = iftToc();
        iftPrintCompTime(t1,t2,"Computational time for iteration %d",it+1);

        /* Uncomment this block and comment the one above for
           non-differential IFT */

        /* iftResetDHeap(Q); */
        /* iftDestroySet(&trees_for_removal); */
        /* iftDestroySet(&new_seeds); */

        /* for (s=0; s < igraph->nnodes; s++) { */
        /*   p = igraph->node[s].voxel; */
        /*   pvalue[s]       = IFT_INFINITY_DBL; */
        /* } */
        /* for (int i = 0; i < nseeds; ++i) { */
        /*   s = seed[i]; */
        /*   iftInsertSet(&new_seeds,s); */
        /*   p = igraph->node[s].voxel; */
        /*   igraph->label[p] = i+1; */
        /* } */


        /* End of comment */

    }


    iftDestroySet(&adj);
    iftDestroySet(&S);
    iftDestroySet(&new_seeds);
    iftDestroySet(&frontier_nodes);
    iftDestroySet(&trees_for_removal);
    iftDestroyDHeap(&Q);
    iftFree(pvalue);
    iftFree(seed);

    return it;
}

void iftFilterSmallSuperpixels(iftIGraph *igraph, int *seed, int nseeds, iftSet **trees_for_removal, iftSet **new_seeds)
{
  int *spSize = iftAllocIntArray(nseeds);
  for (int s = 0; s < igraph->nnodes; ++s) {
    int p = igraph->node[s].voxel;
    int label = igraph->label[igraph->root[p]] - 1;
    spSize[label] += 1;
  }
  
  float sizeThreshold = ((float)igraph->nnodes / (float)(nseeds));
  sizeThreshold *= filteringThreshold;
  for (int i = 0; i < nseeds; ++i) {
    if (spSize[i] <= sizeThreshold) {
      iftInsertSet(trees_for_removal, seed[i]);
      iftRemoveSetElem(new_seeds, seed[i]);
    }
  }

  free(spSize);
}
