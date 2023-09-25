//
// Created by jookuma on 24/01/18.
//

#include "iftDynamicImageForesting.h"

#define ift3MImageDist(mimg, p, q) sqrtf((mimg->val[q][0] - mimg->val[p][0]) * (mimg->val[q][0] - mimg->val[p][0]) +\
                                         (mimg->val[q][1] - mimg->val[p][1]) * (mimg->val[q][1] - mimg->val[p][1]) +\
                                         (mimg->band[2].val[q] - mimg->val[p][2]) * (mimg->band[2].val[q] - mimg->val[p][2]))

iftDynamicTree *iftCreateDynamicTree(int n_nodes, int m_bands, int tree_label, iftFHeap *heap_queue,
                                     iftFImage* dyn_path_val, int *aux_array){
    iftDynamicTree* T = NULL;
    T = iftAlloc(1, sizeof(iftDynamicTree));
    if (T == NULL) iftError("Cannot allocate memory space", "iftCreateDynamicTree");

    T->n = n_nodes;
    T->size = 0;
    T->m = m_bands;
    T->tree = iftAllocIntArray(n_nodes);
    T->mean = iftAllocIntArray(m_bands);
    T->tree_label = tree_label;
    T->heap = heap_queue;
    T->dyn_path_val = dyn_path_val->val;
    T->aux_array = aux_array;

    if (T->tree == NULL || T->mean == NULL)
        iftError("Cannot allocate memory space", "iftCreateDynamicTree");

    return T;
}

void iftDestroyDynamicTree(iftDynamicTree** T){
    iftDynamicTree* aux = *T;
    if(aux != NULL){
        iftFree(aux->tree);
        iftFree(aux->mean);
        iftFree(aux);
        aux = NULL;
    }
}

bool iftInsertNodeDynamicTree(iftMImage* mimg, int node, iftDynamicTree* T){
    if(T->size >= T->n){
        printf("Dynamic Tree is full, node not inserted.");
        return false;
    }

    T->tree[ T->size ] = node;
    T->size++;

    /* Was after iftSortedUpdateDynamicTree, changed on 22/05/18 */
    for(int i = 0; i < T->m; i++){
        T->mean[i] = iftRound( (T->mean[i] * (T->size - 1) + mimg->band[i].val[node]) / T->size);
    }

    _iftSortedUpdateDynamicTree(mimg, T);

    return true;
}

void _iftSortedUpdateDynamicTree(iftMImage *mimg, iftDynamicTree* T) {

    for(int i = 0; i < T->size; i++){
        iftInsertFHeap(T->heap, T->tree[i]);
    }

    // first iteration with tree root (mean) it is not on the heap since
    // the index of this artificial node is -1
    for( int i = 0; i < T->size; i++){
        int t = T->tree[i];
        float dist = 0;

        for (int b = 0; b < mimg->m; b++)
            dist += (T->mean[b] - mimg->band[b].val[t]) * (T->mean[b] - mimg->band[b].val[t]);

        dist = sqrtf(dist);
        // no need for fmax because it is the root and its path-value is 0
        if(dist < T->dyn_path_val[t]){
            T->dyn_path_val[t] = dist;
            iftGoUpFHeap(T->heap, T->heap->pos[t]);
        }
    }

    int k = 0;
    while (!iftEmptyFHeap(T->heap)) {
        int s = iftRemoveFHeap(T->heap);
        T->aux_array[k] = s;
        k++;

        for (int i = 0; i < T->size; i++) {
            int t = T->tree[i];

            if (T->heap->color[t] != IFT_BLACK && T->dyn_path_val[s] < T->dyn_path_val[t]){

                float dist = ift3MImageDist(mimg, s, t);
                float tmp = iftMax(T->dyn_path_val[s], dist);

                if (tmp < T->dyn_path_val[t]) {
                    T->dyn_path_val[t] = tmp;
                    iftGoUpFHeap(T->heap, T->heap->pos[t]);
                }
            }
        }
    }

    for (int i = 0; i < T->size; i++) {
        int t = T->tree[i];
        T->heap->color[t] = IFT_WHITE;
        T->heap->pos[t]   = -1;
        T->heap->node[t] = -1;
    }
    T->heap->last = -1;

    memcpy(T->tree, T->aux_array, sizeof(int) * T->size);
}

float _iftArcWeightByDynamicTree(iftMImage *mimg, iftDynamicTree* T, int q) {
    // first iteration for root node (mean);
    float dist = 0;

    for(int i = 0; i < T->size; i++){
        int p = T->tree[i];
        dist = ift3MImageDist(mimg, p, q);
        float tmp = iftMax(T->dyn_path_val[p], dist);
        if(tmp < T->dyn_path_val[q]){
            return tmp;
        }
    }

    return (((float) IFT_INFINITY_INT / (DEC_CORRECTION * 2)) - 1.0f);
}


iftImage *iftDynamicObjectDelineation(iftMImage *mimg, iftLabeledSet *seeds, const iftAdjRel *A, int max_pred_length){

    iftImage *label    = iftCreateImage(mimg->xsize, mimg->ysize, mimg->zsize);
    iftImage *pathval     = iftCreateImage(mimg->xsize, mimg->ysize, mimg->zsize);
    iftFImage *pathval_dyn  = iftCreateFImage(mimg->xsize, mimg->ysize, mimg->zsize);
    iftImage *root     = iftCreateImage(mimg->xsize, mimg->ysize, mimg->zsize);

    iftDynamicTree **forest = iftAlloc(mimg->n, sizeof(iftDynamicTree*));
    int *aux_array = iftAllocIntArray(max_pred_length);

    int mimg_max = (int) (2 * iftMMaximumValue(mimg, -1) * DEC_CORRECTION);
    iftGQueue *Q      = iftCreateGQueue(mimg_max + 1, mimg->n, pathval->val);
    iftFHeap *H_dyn   = iftCreateFHeap(mimg->n, pathval_dyn->val);

    for (int p = 0; p < mimg->n; p++){
        pathval->val[p]    = IFT_INFINITY_INT;
        pathval_dyn->val[p] = IFT_INFINITY_FLT;
        label->val[p] = IFT_NIL;
    }

    for(iftLabeledSet *L = seeds; L != NULL; L = L->next){
        int p = L->elem;

        pathval->val[p]    = 0;
        pathval_dyn->val[p] = 0;
        label->val[p]   = L->label;
        root->val[p] = p;

        forest[p] = iftCreateDynamicTree(max_pred_length, mimg->m, L->label, H_dyn,
                                            pathval_dyn, aux_array);

        iftInsertGQueue(&Q, p);
    }

    while (!iftEmptyGQueue(Q)) {
        int p      = iftRemoveGQueue(Q);
        iftVoxel u = iftMGetVoxelCoord(mimg, p);

        int r = root->val[p];

        if(forest[r]->size < max_pred_length) {
            iftInsertNodeDynamicTree(mimg, p, forest[r]);
        }

        for (int i = 1; i < A->n; i++) {
            iftVoxel v = iftGetAdjacentVoxel(A, u, i);

            if (iftValidVoxel(mimg, v)) {
                int q = iftMGetVoxelIndex(mimg, v);

                if (Q->L.elem[q].color != IFT_BLACK && pathval->val[p] < pathval->val[q]) {
                    // p conquers q

//                    float arc_w = _iftArcWeightByDynamicTree(mimg, forest[r], q);
                    float arc_w = _iftArcWeightByDynamicTree(mimg, forest[r], q) + ift3MImageDist(mimg, p, q);

                    int tmp = iftMax(pathval->val[p], (int) (arc_w * (DEC_CORRECTION)));

                    if (tmp < pathval->val[q]) {

                        if(Q->L.elem[q].color == IFT_GRAY) {
                            iftRemoveGQueueElem(Q, q);
                        }

                        pathval->val[q]  = tmp;
                        pathval_dyn->val[q] = arc_w;
                        label->val[q] = label->val[p];
                        root->val[q]  = root->val[p];

                        iftInsertGQueue(&Q, q);
                    }
                }
            }
        }
    }

    for(iftLabeledSet *L = seeds; L != NULL; L = L->next){
        int p = L->elem;
        iftDestroyDynamicTree(&forest[p]);
    }
    iftFree(forest);

    iftDestroyImage(&pathval);
    iftDestroyFImage(&pathval_dyn);
    iftDestroyGQueue(&Q);
    iftDestroyFHeap(&H_dyn);
    iftDestroyImage(&root);
    iftFree(aux_array);

    return label;
}

iftImage *iftDynamicObjectDelinForest(iftMImage *mimg, iftLabeledSet *seeds, const iftAdjRel *A,
                                      int max_pred_length, float k_perc){

    iftImage *label    = iftCreateImage(mimg->xsize, mimg->ysize, mimg->zsize);
    iftImage *pathval     = iftCreateImage(mimg->xsize, mimg->ysize, mimg->zsize);
    iftFImage *pathval_dyn  = iftCreateFImage(mimg->xsize, mimg->ysize, mimg->zsize);
    iftImage *root     = iftCreateImage(mimg->xsize, mimg->ysize, mimg->zsize);
    iftImage *control = iftCreateImage(mimg->xsize, mimg->ysize, mimg->zsize);

    iftDynamicTree **forest = iftAlloc(mimg->n, sizeof(iftDynamicTree*));
    int *aux_array = iftAllocIntArray(max_pred_length);

    int mimg_max = (int) (2 * iftMMaximumValue(mimg, -1) * DEC_CORRECTION);
    iftGQueue *Q      = iftCreateGQueue(mimg_max + 1, mimg->n, pathval->val);
    iftFHeap *H_dyn   = iftCreateFHeap(mimg->n, pathval_dyn->val);

    for (int p = 0; p < mimg->n; p++){
        pathval->val[p]    = IFT_INFINITY_INT;
        pathval_dyn->val[p] = IFT_INFINITY_FLT;
        label->val[p] = IFT_NIL;
        control->val[p] = 0;
    }

    for(iftLabeledSet *L = seeds; L != NULL; L = L->next){
        int p = L->elem;

        pathval->val[p]    = 0;
        pathval_dyn->val[p] = 0;
        label->val[p]   = L->label;
        root->val[p] = p;

        forest[p] = iftCreateDynamicTree(max_pred_length, mimg->m, L->label, H_dyn,
                                         pathval_dyn, aux_array);

        iftInsertGQueue(&Q, p);
    }

    iftDataSet *Z = _iftCreateForestRootsDataSet(seeds, forest);

    int count = 0;
    while (!iftEmptyGQueue(Q)) {
        int p      = iftRemoveGQueue(Q);
        iftVoxel u = iftMGetVoxelCoord(mimg, p);

        int r = root->val[p];

        if(forest[r]->size < max_pred_length) {
            iftInsertNodeDynamicTree(mimg, p, forest[r]);
            if (count++ % 100  == 0) {
                _iftUpdateDataSetFromForest(Z, forest, seeds);
                iftControlClusterPurity(Z, forest, control, k_perc);
            }
        }

        for (int i = 1; i < A->n; i++) {
            iftVoxel v = iftGetAdjacentVoxel(A, u, i);

            if (iftValidVoxel(mimg, v)) {
                int q = iftMGetVoxelIndex(mimg, v);

                if (Q->L.elem[q].color != IFT_BLACK && pathval->val[p] < pathval->val[q]) {
                    // p conquers q

                    float arc_w = _iftArcWeightByDynamicTree(mimg, forest[r], q);
//                    float arc_w = _iftArcWeightByDynamicTree(mimg, forest[r], q) + ift3MImageDist(mimg, p, q);

                    for(iftLabeledSet *S = seeds; S != NULL; S = S->next) {
                        if(S->label == label->val[p] && control->val[S->elem] == 1){
                            float maybe_arc_w = _iftArcWeightByDynamicTree(mimg, forest[S->elem], q);
                            if (maybe_arc_w < arc_w) arc_w = maybe_arc_w;
                        }
                    }
                    arc_w += ift3MImageDist(mimg, p, q);

                    int tmp = iftMax(pathval->val[p], (int) (arc_w * (DEC_CORRECTION)));

                    if (tmp < pathval->val[q]) {

                        if(Q->L.elem[q].color == IFT_GRAY) {
                            iftRemoveGQueueElem(Q, q);
                        }

                        pathval->val[q]  = tmp;
                        pathval_dyn->val[q] = arc_w;
                        label->val[q] = label->val[p];
                        root->val[q]  = root->val[p];

                        iftInsertGQueue(&Q, q);
                    }
                }
            }
        }
    }

    for(iftLabeledSet *L = seeds; L != NULL; L = L->next){
        int p = L->elem;
        iftDestroyDynamicTree(&forest[p]);
    }
    iftFree(forest);

    iftDestroyDataSet(&Z);
    iftDestroyImage(&control);
    iftDestroyImage(&pathval);
    iftDestroyFImage(&pathval_dyn);
    iftDestroyGQueue(&Q);
    iftDestroyFHeap(&H_dyn);
    iftDestroyImage(&root);
    iftFree(aux_array);

    return label;
}


iftImage *iftDynamicArcWeightsForest(iftMImage *mimg, iftLabeledSet *seeds, const iftAdjRel *A,
                                     int max_pred_length, float k_perc){

    iftImage *label    = iftCreateImage(mimg->xsize, mimg->ysize, mimg->zsize);
    iftImage *pathval     = iftCreateImage(mimg->xsize, mimg->ysize, mimg->zsize);
    iftFImage *pathval_dyn  = iftCreateFImage(mimg->xsize, mimg->ysize, mimg->zsize);
    iftImage *root     = iftCreateImage(mimg->xsize, mimg->ysize, mimg->zsize);
    iftImage *control = iftCreateImage(mimg->xsize, mimg->ysize, mimg->zsize);

    iftDynamicTree **forest = iftAlloc(mimg->n, sizeof(iftDynamicTree*));
    int *aux_array = iftAllocIntArray(max_pred_length);

    int mimg_max = (int) (2 * iftMMaximumValue(mimg, -1) * DEC_CORRECTION);
    iftGQueue *Q      = iftCreateGQueue(mimg_max + 1, mimg->n, pathval->val);
    iftFHeap *H_dyn   = iftCreateFHeap(mimg->n, pathval_dyn->val);

    for (int p = 0; p < mimg->n; p++){
        pathval->val[p]    = IFT_INFINITY_INT;
        pathval_dyn->val[p] = IFT_INFINITY_FLT;
        label->val[p] = IFT_NIL;
        control->val[p] = 0;
    }

    for(iftLabeledSet *L = seeds; L != NULL; L = L->next){
        int p = L->elem;

        pathval->val[p]    = 0;
        pathval_dyn->val[p] = 0;
        label->val[p]   = L->label;
        root->val[p] = p;

        forest[p] = iftCreateDynamicTree(max_pred_length, mimg->m, L->label, H_dyn,
                                         pathval_dyn, aux_array);

        iftInsertGQueue(&Q, p);
    }

    iftDataSet *Z = _iftCreateForestRootsDataSet(seeds, forest);

    int count = 0;
    while (!iftEmptyGQueue(Q)) {
        int p      = iftRemoveGQueue(Q);
        iftVoxel u = iftMGetVoxelCoord(mimg, p);

        int r = root->val[p];

        if (forest[r]->size < max_pred_length) {
            iftInsertNodeDynamicTree(mimg, p, forest[r]);
            if (count++ % 100  == 0) {
                _iftUpdateDataSetFromForest(Z, forest, seeds);
                iftControlClusterPurity(Z, forest, control, k_perc);
            }
        }

        for (int i = 1; i < A->n; i++) {
            iftVoxel v = iftGetAdjacentVoxel(A, u, i);

            if (iftValidVoxel(mimg, v)) {
                int q = iftMGetVoxelIndex(mimg, v);

                if (Q->L.elem[q].color != IFT_BLACK && pathval->val[p] < pathval->val[q]) {
                    // p conquers q

                    float arc_w = _iftArcWeightByDynamicTree(mimg, forest[r], q);
//                    float arc_w = _iftArcWeightByDynamicTree(mimg, forest[r], q) + ift3MImageDist(mimg, p, q);

                    for(iftLabeledSet *S = seeds; S != NULL; S = S->next) {
                        if(S->label == label->val[p] && control->val[S->elem] == 1){
                            float maybe_arc_w = _iftArcWeightByDynamicTree(mimg, forest[S->elem], q);
                            if (maybe_arc_w < arc_w) arc_w = maybe_arc_w;
                        }
                    }
                    arc_w += ift3MImageDist(mimg, p, q);

                    int tmp = iftMax(pathval->val[p], (int) (arc_w * (DEC_CORRECTION)));

                    if (tmp < pathval->val[q]) {

                        if(Q->L.elem[q].color == IFT_GRAY) {
                            iftRemoveGQueueElem(Q, q);
                        }

                        pathval->val[q]  = tmp;
                        pathval_dyn->val[q] = arc_w;
                        label->val[q] = label->val[p];
                        root->val[q]  = root->val[p];

                        iftInsertGQueue(&Q, q);
                    }
                }
            }
        }
    }

    for(iftLabeledSet *L = seeds; L != NULL; L = L->next){
        int p = L->elem;
        iftDestroyDynamicTree(&forest[p]);
    }
    iftFree(forest);

    iftDestroyDataSet(&Z);
    iftDestroyImage(&control);
    iftDestroyImage(&label);
    iftDestroyFImage(&pathval_dyn);
    iftDestroyGQueue(&Q);
    iftDestroyFHeap(&H_dyn);
    iftDestroyImage(&root);
    iftFree(aux_array);

    return pathval;
}

void iftControlClusterPurity(iftDataSet* Z, iftDynamicTree** forest, iftImage* control, float k_perc) {
    
    int k = (int) (Z->ntrainsamples * k_perc);
    if(k < 2) k = 2;

    iftSetImage(control, 1);

    iftClusterDataSetByKMeans(Z, k, 50, 0.0001f, 1, 0, 0);

//    iftKnnGraph* graph = iftCreateKnnGraph(Z, Z->ntrainsamples - 1);
//    iftKnnGraph* graph = iftCreateKnnGraph(Z, k);
//    graph->n = Z->ntrainsamples / 20;
//    iftPDFByRange(graph);
//    ngroups=iftUnsupOPF(graph);

//    iftUnsupTrain(graph, iftNormalizedCut);

    #pragma omp parallel for
    for (int i = 0; i < Z->nsamples - 1; i++) {
        int p = Z->sample[i].id;
        for (int j = i + 1; j < Z->nsamples; j++) {
            int q = Z->sample[j].id;
            if (Z->sample[i].group == Z->sample[j].group) {
                if (forest[p]->tree_label != forest[q]->tree_label) {
                    control->val[p] = 0;
                    control->val[q] = 0;
                }
            }
        }
    }
//    iftDestroyKnnGraph(&graph);
}

iftDataSet *_iftCreateForestRootsDataSet(iftLabeledSet* seeds, iftDynamicTree** forest) {

    int inc = 0, n_samples = 0, min_class = IFT_INFINITY_INT, max_class = IFT_INFINITY_INT_NEG;

    for (iftLabeledSet *S = seeds; S != NULL; S = S->next) {
        n_samples++;
        if (S->label < min_class) min_class = S->label;
        if (S->label > max_class) max_class = S->label;
    }

   if (min_class == 0) {
        inc = 1;
    }

    int n_classes = max_class + inc;
 
    iftDataSet *Z = iftCreateDataSet(n_samples, forest[seeds->elem]->m);

    int s = 0;
    for (iftLabeledSet *S = seeds; S != NULL; S = S->next, s++) {
        int p = S->elem;
        for (int b = 0; b < forest[p]->m; b++) {
            Z->sample[s].feat[b] = forest[p]->mean[b];
        }
        Z->sample[s].truelabel = S->label + inc;
        Z->sample[s].id = p;
        iftSetSampleStatus(&Z->sample[s], IFT_TRAIN);
    }

    Z->ref_data = forest;
    Z->ref_data_type = IFT_REF_DATA_NULL;
    Z->nclasses = n_classes;
    Z->ntrainsamples = n_samples;
    
    return Z;
}

void _iftUpdateDataSetFromForest(iftDataSet* Z, iftDynamicTree** forest, iftLabeledSet* seeds) {
    int s = 0;
    for (iftLabeledSet *S = seeds; S != NULL; S = S->next, s++) {
        int p = S->elem;
        for (int b = 0; b < forest[p]->m; b++) {
            Z->sample[s].feat[b] = forest[p]->mean[b];
        }
    }
}
