#include <ift.h>
#include <iftRegion.h>

iftRegionHierarchy* iftMergeBorderRegions2(iftRegionGraph *region_graph, float threshold,iftRegionHierarchyNode ***output_hierarchy_nodes, int *region_size){

  iftImage *label_image = (iftImage*)region_graph->dataset->ref_data;
  iftRegionHierarchy* rh = (iftRegionHierarchy*)iftAlloc(1, sizeof(iftRegionHierarchy));

  //Creates a region graph
  rh->nleaves = region_graph->nnodes;

  //Stores the pixels belonging to each region
  rh->pixels_by_node = (iftSet**)iftAlloc(region_graph->nnodes,sizeof(iftSet *));
  int p;

  for(p = 0; p < label_image->n; p++){
    iftInsertSet(&rh->pixels_by_node[label_image->val[p] - 1],p);
  }
  rh->label_image = iftCopyImage(label_image);

  iftRegionHierarchyNode **hierarchy_nodes = (iftRegionHierarchyNode**)iftAlloc(region_graph->nnodes,sizeof(iftRegionHierarchyNode*));
  int r;
  int node_id = 1;
  for(r = 0; r < region_graph->nnodes; r++){
    hierarchy_nodes[r] = iftCreateRegionHierarchyLeaf(label_image, rh->pixels_by_node[r], r, node_id++);
  }

  iftResetFHeap(region_graph->heap);

  //Finds the distance to the nearest neighbor, stores it in "pathval" and the nearest neighbor in "destination"
  int *dest = iftAllocIntArray(region_graph->nnodes);
  float *f1=iftAllocFloatArray(region_graph->dataset->nfeats);
  float *f2=iftAllocFloatArray(region_graph->dataset->nfeats);
  float dist;

  for(r = 0; r < region_graph->nnodes; r++) {

    region_graph->pathval[r] = IFT_INFINITY_FLT;
    dest[r] = -1;
    if (region_graph->node[r].node_type == 1) {
      iftSet *adj = region_graph->node[r].adjacent;

      while (adj) {
        int neighbor = adj->elem;

          if (region_graph->node[neighbor].node_type == 1) {
            for (int i=0;i<region_graph->dataset->nfeats;i++){
              f1[i]=region_graph->dataset->sample[r].feat[i]/region_size[r];
              f2[i]=region_graph->dataset->sample[neighbor].feat[i]/region_size[neighbor];
            }
            dist = 1 - region_graph->dataset->iftArcWeight(f1,f2,region_graph->dataset->alpha,
            region_graph->dataset->nfeats);
            if (region_graph->pathval[r] > dist) {
              region_graph->pathval[r] = dist;
              dest[r] = neighbor;
            }
          }

      adj = adj->next;
      }
    }
    iftInsertFHeap(region_graph->heap, r);
  }

  /* the threshold gotten as a parameter is a coefficient of similarity between [0..1]*/
  threshold=1-threshold;

  //Repeat until we achieve the desired number of iterations
  int i;
  int u;
  printf("\n");
  int count_nodes=0;
  for (int k=0;k<region_graph->nnodes;k++)
    if (region_graph->node[k].node_type ==1)
      count_nodes++;
  printf("count nodes in frontier -> %d\n ",count_nodes);
  for(i = 0 ;; i++){

    //The vertices u and v will be merged
    u = iftRemoveFHeap(region_graph->heap);
    int v = dest[u];

    if (region_size[u]==0)
      iftError("The node label is empty","iftMergeBorderRegions2");

    if (region_graph->pathval[u] > threshold)
      break;

    printf("Iter %d Giving value sup %d to sup %d with value %.3f\n",i+1,u,v,1-region_graph->pathval[u]);

    //Creates a new node in the hierarchy
    iftRegionHierarchyNode *new_node = iftCreateRegionHierarchyNode(i+1, hierarchy_nodes[u], hierarchy_nodes[v], node_id++);
    hierarchy_nodes[u] = new_node;

    //Remove the node v from consideration
    dest[v] = -1;
    iftRemoveFHeapElem(region_graph->heap, v);

    //Compute the features for the new node
    for (int f=0;f<region_graph->dataset->nfeats;f++)
      region_graph->dataset->sample[u].feat[f]+=region_graph->dataset->sample[v].feat[f];

    /*update region size*/
    region_size[u]+=region_size[v];
    region_size[v]=0;

    //Nobody should be adjacent to v
    iftSet *v_adj = region_graph->node[v].adjacent;
    while(v_adj){
      iftRemoveSetElem(&region_graph->node[v_adj->elem].adjacent,v);
      v_adj = v_adj->next;
    }

    //Removes u from v's adjacents
    iftRemoveSetElem(&region_graph->node[v].adjacent,u);

    //Joins u's and v's adjacents
    iftSet *old_adj_u = region_graph->node[u].adjacent;
    region_graph->node[u].adjacent = iftSetUnion(region_graph->node[u].adjacent,region_graph->node[v].adjacent);
    iftDestroySet(&old_adj_u);

    //Removes v's adjacents
    iftDestroySet(&region_graph->node[v].adjacent);

    //If u is adjacent to w, w is adjacent to u (it would be faster to consider only v's adjacents)
    iftSet* u_adj = region_graph->node[u].adjacent;
    while(u_adj){
      if(!iftSetHasElement(region_graph->node[u_adj->elem].adjacent, u))
        iftInsertSet(&region_graph->node[u_adj->elem].adjacent, u);

      u_adj = u_adj->next;
    }

    //The nearest node to u will be recomputed
    region_graph->pathval[u] = IFT_INFINITY_FLT;

    u_adj = region_graph->node[u].adjacent;

    while(u_adj){
      //w is adjacent to u
      int w = u_adj->elem;

      if (region_graph->node[w].node_type == 1 && region_size[w]>0) {
        //Finds the nearest adjacent to u
        for (int i=0;i<region_graph->dataset->nfeats;i++){
          f1[i]=region_graph->dataset->sample[u].feat[i]/region_size[u];
          f2[i]=region_graph->dataset->sample[w].feat[i]/region_size[w];
        }
        dist = 1-region_graph->dataset->iftArcWeight(f1,f2, region_graph->dataset->alpha,
          region_graph->dataset->nfeats);

        if (region_graph->pathval[u] > dist) {
          region_graph->pathval[u] = dist;
          dest[u] = w;
        }

        //Finds the nearest node to w
        iftSet *w_adj = region_graph->node[w].adjacent;
        iftRemoveFHeapElem(region_graph->heap, w);
        region_graph->pathval[w] = IFT_INFINITY_FLT;
        while (w_adj) {
            if (region_graph->node[w_adj->elem].node_type == 1 && region_size[w_adj->elem]>0) {
              for (int i=0;i<region_graph->dataset->nfeats;i++){
                f1[i]=region_graph->dataset->sample[w].feat[i]/region_size[w];
                f2[i]=region_graph->dataset->sample[w_adj->elem].feat[i]/region_size[w_adj->elem];
              }
              dist = 1-region_graph->dataset->iftArcWeight(f1,f2, region_graph->dataset->alpha,
                                           region_graph->dataset->nfeats);
              if (region_graph->pathval[w] > dist) {
                region_graph->pathval[w] = dist;
                dest[w] = w_adj->elem;
              }
            }
          w_adj = w_adj->next;
        }
        iftInsertFHeap(region_graph->heap, w);
      }

      u_adj = u_adj->next;
    }

    //Tests whether there is only one region left
    if(iftEmptyFHeap(region_graph->heap)){
      break;
    }

    //Re-inserts u in the queue
    iftInsertFHeap(region_graph->heap, u);
  }

  iftFree(dest);
  iftFree(f1);
  iftFree(f2);

  //Sets the root of the hierarchy
  rh->root = hierarchy_nodes[u];
  // set output array of hierarchy nodes
  *output_hierarchy_nodes = hierarchy_nodes;

  return rh;
}


int main(int argc, char *argv[])
{
  iftImage        *img=NULL,*label=NULL,*aux=NULL;
  iftDataSet      *Z=NULL;
  iftColor         RGB,YCbCr;
  iftAdjRel       *A=NULL,*B=NULL;
  char             ext[10],*pos;
  timer           *t1=NULL,*t2=NULL;

  if (argc<7 || argc >8)
    iftError("Usage: iftClusterImageByDividingInBlocksUsingOPFAndMergingAdjacentRegions2 <image.ppm[pgm,png,scn]> <num_blocks> <train_samples_nb> <kmax_perc [0..1]> <area> <similarity threshold to merge sup in differents blocks[0..1] (recomended a value near 0.85)> <gt_image(OPTIONAL)>","main");

  iftRandomSeed(IFT_RANDOM_SEED);

  img=iftReadImageByExt(argv[1]);

  /* convert the image to multi-image*/
  iftMImage *mimg;
  iftMImage *eimg;

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
    mimg = iftImageToMImage(img, LABNorm2_CSPACE);
    eimg = iftExtendMImageByVoxelCoord(mimg,1);
    iftDestroyMImage(&mimg);
    mimg = eimg;
  }

  A=NULL;
  Z= iftMImageToDataSet(mimg);
  fprintf(stdout,"%s %d %.3f %.3f ",argv[1],Z->nsamples,atof(argv[3]),atof(argv[4]));

  /* read the gt image if it was given as parameter*/
//  iftImage *gt=NULL;
//  if (argc==9){
//    gt=iftReadImageByExt(argv[8]);
//    iftImageGTToDataSet(gt,Z);
//  }

  t1 = iftTic();
  iftClusterImageByDividingInBlocksAndUsingOPF(Z, atoi(argv[2]), atof(argv[3]), iftNormalizedCut, atof(argv[4]));
  t2 = iftToc();
  fprintf(stdout,"%d %.2f ",Z->nlabels,iftCompTime(t1,t2));

  if (Z->nclasses > 0){
    iftSetStatus(Z,TEST);
    printf("%.2f ",iftTruePositives(Z));
  }

  label = iftDataSetToLabelImage(Z, false);

  if (!iftIs3DImage(img)){
    iftImage *labels_orig=iftColorizeComp(label);
    iftWriteImageP6(labels_orig,"labels_orig.ppm");
    iftDestroyImage(&labels_orig);
  }
  else{
    iftWriteImage(label,"labels_orig.scn");
  }

  /* do smoothing*/
  aux=label;
  label = iftSmoothRegionsByDiffusion(aux,img,0.5,2);
  iftDestroyImage(&aux);

  /* eliminate noise inside blocks*/
  aux   = iftSelectAndPropagateRegionsAboveAreaByColor(img,label,10);
  iftDestroyImage(&label);
  label=aux;

  if (!iftIs3DImage(img)){
    iftImage *aux=iftColorizeComp(label);
    iftWriteImageP6(aux,"after_smooth.ppm");
    iftDestroyImage(&aux);
  }

  if (!iftIs3DImage(img)){
    iftWriteImageP2(label,"label_before_merge.pgm");
  }

  /* print regions before merging*/
  if (!iftIs3DImage(img)){
    iftDestroyAdjRel(&A);
    A  = iftCircular(sqrtf(2.0));
    B          = iftCircular(0.0);
    RGB.val[0] = 0;
    RGB.val[1] = 255;
    RGB.val[2] = 255;
    YCbCr      = iftRGBtoYCbCr(RGB,255);
    aux        = iftCopyImage(img);
    iftDrawBorders(aux,label,A,YCbCr,B);

    pos = strrchr(argv[1],'.') + 1;
    sscanf(pos,"%s",ext);
    if (strcmp(ext,"png")==0)
      iftWriteImagePNG(aux,"regions_before.png");
    else
      iftWriteImageP6(aux,"regions_before.ppm");

    iftDestroyImage(&aux);
    iftDestroyAdjRel(&B);
  }

  t1=iftTic();
  iftImage *block_img=iftMImageTilesToLabelImage(mimg,atoi(argv[2]));

  printf("initially has %d superpixels\n",iftMaximumValue(label));

  /*create a dataset for the histogram*/
  int *regions=iftAllocIntArray(iftMaximumValue(label));
  int nbins;
  if (iftIsColorImage(img))
    nbins=512;
  else
    nbins=16;
  iftDataSet *sup_dataset=iftSupervoxelsToLabOrGrayHistogramDataSet(img, label, nbins, regions);

  if (!iftIs3DImage(img))
    A = iftCircular(1.0);
  else
    A = iftSpheric(1.0);
  iftRegionGraph *region_graph = iftRegionGraphFromLabelImage(label, sup_dataset, A);

  iftSetRegionGraphNodeTypeForBorderSuperpixelsInTiles(region_graph, label, block_img, A);

//  for (int i=0;i<region_graph->nnodes;i++){
//    printf("%d Adjacents are -> ",i);
//    if (region_graph->node[i].node_type){
//      iftSet *adj=region_graph->node[i].adjacent;
//      while (adj){
//        if (region_graph->node[adj->elem].node_type)
//          printf("%d ",adj->elem);
//        adj=adj->next;
//      }
//    }
//    printf("\n");
//  }

  float join_threshold =atof(argv[6]);
  iftRegionHierarchyNode **hierarchy_nodes;

  iftRegionHierarchy *rh = iftMergeBorderRegions2(region_graph, join_threshold, &hierarchy_nodes,regions);

  // Relabel merged regions
  int current_label = sup_dataset->nsamples + 1;
  for (int r = 0; r < region_graph->nnodes; ++r) {
    if (region_graph->node[r].node_type==1)
      current_label = iftRHFlatten(rh, hierarchy_nodes[r], label, IFT_INFINITY_INT, current_label);
  }

  iftFree(hierarchy_nodes);
  iftDestroyRegionHierarchy(&rh);
  iftDestroyImage(&block_img);
  iftDestroyRegionGraph(&region_graph);
  iftDestroyAdjRel(&A);
  iftDestroyDataSet(&sup_dataset);

  A=iftCircular(sqrtf(2.0));

  iftImage *relabel = iftRelabelRegions(label,A);
  iftDestroyImage(&label);
  label = relabel;

  printf("remaining %d superpixels\n",iftMaximumValue(label));

  t2=iftToc();
  printf("\nTime to remove block lines is %.3f\n",iftCompTime(t1,t2));

  /* eliminate short superpixels*/
  aux   = iftSelectAndPropagateRegionsAboveAreaByColor(img,label,atoi(argv[5]));
  iftDestroyImage(&label);
  label=aux;

  printf("%d\n",iftMaximumValue(label));

  /* write labels image */
  if (iftIs3DImage(img)){
    iftWriteImage(label,"labels.scn");
  }
  else{
    iftWriteImageP2(label,"labels.pgm");
    aux   = iftColorizeComp(label);
    iftWriteImageP6(aux,"clusters.ppm");
    iftDestroyImage(&aux);
  }

  /*compute br and ue*/
  iftImage *border = iftBorderImage(label,0);
  if (argc ==8){
    iftImage *gt_img=iftReadImageByExt(argv[7]);
    aux=iftRelabelGrayScaleImage(gt_img);
    iftDestroyImage(&gt_img);
    gt_img=aux;
    iftImage *gt_borders=iftBorderImage(gt_img,0);
    printf("br -> %.4f\n",iftBoundaryRecall(gt_borders, border, 2.0));
    printf("ue -> %.4f\n",iftUnderSegmentation(gt_img, label));

    iftDestroyImage(&gt_img);
    iftDestroyImage(&gt_borders);
  }

  if (iftIs3DImage(img)){
    iftWriteImage(border, "border.scn");
  }
  else
    iftWriteImageP2(border, "border.pgm");
  iftDestroyImage(&border);

  if (!iftIs3DImage(img)){
    iftDestroyAdjRel(&A);
    A  = iftCircular(sqrtf(2.0));
    B          = iftCircular(0.0);
    RGB.val[0] = 0;
    RGB.val[1] = 255;
    RGB.val[2] = 255;
    YCbCr      = iftRGBtoYCbCr(RGB,255);
    aux        = iftCopyImage(img);
    iftDrawBorders(aux,label,A,YCbCr,B);

    pos = strrchr(argv[1],'.') + 1;
    sscanf(pos,"%s",ext);
    if (strcmp(ext,"png")==0)
      iftWriteImagePNG(aux,"regions.png");
    else
      iftWriteImageP6(aux,"regions.ppm");

    iftDestroyImage(&aux);
    iftDestroyAdjRel(&B);

    int norm_value;
    iftImage *dens=NULL;
    int s,p,q;
    iftVoxel u;

    dens = iftDataSetWeight(Z);
    norm_value = iftNormalizationValue(iftMaximumValue(img));
    int maxdens = iftMaximumValue(dens);

    /* generate an image with the outliers*/
    RGB.val[0] = norm_value;
    RGB.val[1] = 0;
    RGB.val[2] = 0;
    YCbCr      = iftRGBtoYCbCr(RGB,norm_value);
    B          = iftCircular(1.5);
    aux        = iftCopyImage(dens);
    for (p=0; p < dens->n; p++)
      aux->val[p] = (int) (((float)dens->val[p]/maxdens)*norm_value);
    int outlier_nb=0;
    for (s=0; s < Z->nsamples; s++) {
      if (Z->sample[s].status == OUTLIER){
        q = Z->sample[s].id;
        u = iftGetVoxelCoord(img,q);
        iftDrawPoint(aux,u,YCbCr,B);
        outlier_nb++;
      }
    }
    if (outlier_nb >0)
      iftWriteImageP6(aux,"outlier.ppm");
    else
      iftWriteImageP2(aux,"outlier.ppm");

    iftDestroyImage(&aux);
    iftDestroyAdjRel(&B);
    iftDestroyImage(&dens);
  }

  iftDestroyAdjRel(&A);
  iftDestroyImage(&img);
  iftDestroyMImage(&mimg);
  iftDestroyImage(&label);
  iftDestroyDataSet(&Z);

   return(0);
}




