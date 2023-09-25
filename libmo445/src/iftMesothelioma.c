
#include "iftMesothelioma.h"
//
// Created by taylla on 16/01/23.
//

iftImage *iftMesoRegionContainingPleura(iftImage *img, iftImage *mask, float max_dist) {
    iftAdjRel *A         = iftSpheric(1.75);
    iftImage  *dist      = iftCreateImageFromImage(img);
    iftImage  *root      = iftCreateImageFromImage(img);
    iftImage  *roi       = iftCreateImageFromImage(img);
    iftGQueue *Q         = iftCreateGQueue(IFT_QSIZE, img->n, dist->val);

    max_dist             = max_dist*max_dist;

    for (int p = 0; p < img->n; p++) {
        dist->val[p] = IFT_INFINITY_INT;

        if (mask->val[p] != 0) {
            dist->val[p] = 0;
            root->val[p] = p;
            iftInsertGQueue(&Q, p);
        }
    }

    while(!iftEmptyGQueue(Q)) {
        int p = iftRemoveGQueue(Q);

        if (dist->val[p] < max_dist) {

            iftVoxel u = iftGetVoxelCoord(img, p);
            iftVoxel r = iftGetVoxelCoord(img, root->val[p]);

            for (int i = 1; i < A->n; i++) {
                iftVoxel v = iftGetAdjacentVoxel(A, u, i);

                if (iftValidVoxel(img, v)) {
                    int q = iftGetVoxelIndex(img, v);

                    if (dist->val[q] > dist->val[p]) {

                        float tmp = (v.x-r.x)*(v.x-r.x) + (v.y-r.y)*(v.y-r.y) + (v.z-r.z)*(v.z-r.z);

                        if (tmp < dist->val[q]) {
                            if (dist->val[q] != IFT_INFINITY_INT)
                                iftRemoveGQueueElem(Q, q);
                            dist->val[q]      = tmp;
                            root->val[q]      = root->val[p];
                            //if (img->val[q] > 1000)
                            roi->val[q]     = 1;
                            iftInsertGQueue(&Q, q);
                        }
                    }
                }
            }
        }
    }

    for (int p=0; p < roi->n; p++){
        if (mask->val[p]!=0)
            roi->val[p] = 0;
    }

    iftDestroyGQueue(&Q);

    iftDestroyImage(&root);
    iftDestroyImage(&dist);
    iftDestroyAdjRel(&A);

    return(roi);
}

float* iftMesoIntensitiesFileInput(int n_labels, int n_img){
    char name[20];
    float *intensities = iftAllocFloatArray(n_labels);
    sprintf(name, "f%d.txt",n_img);
    FILE *fp = fopen(name,"r");
    int i=0;
    while (fscanf(fp, "%f", &intensities[i++]) != EOF);
    fclose(fp);
    return intensities;
}

float** iftMesoTransposeMatrix(float** matrix, int line, int col){
    float** matrix_t = (float**) iftAlloc(line, sizeof(float*));

    for (int i = 0; i < line; i++) {
        matrix_t[i] = iftAllocFloatArray(col);
        for (int j = 0; j < col; j++) {
            matrix_t[i][j] = matrix[j][i];
        }
    }

    return matrix_t;
}
int iftMesoGetIndexofOrdenatedIntArrayfromValue(int* array, int length, int value){
    /* Return the index of value igual in the ordenated array or if there is no value, the previous index
     */
    for(int i=0; i<length;i++){
        if(array[i]==value){
            return i;
        }
        else if(array[i]>value){
            return i-1;
        }
    }
    return length-1;
}

iftSet* iftMesoECECurveLabelSelection(float** mean_intensity_superspel, int n_label, int n_images, int* time_sec){
    int     peak_index, flag_passed = 0;
    iftSet *possible_curves = NULL;

    mean_intensity_superspel     = iftMesoTransposeMatrix(mean_intensity_superspel, n_label, n_images);
    int time_ECE_index = iftMesoGetIndexofOrdenatedIntArrayfromValue(time_sec, n_images, 270);

    for(int n = 0; n < n_label; n++) {
        flag_passed = 0;
        peak_index  = iftMaxIndexFloatArray(mean_intensity_superspel[n],n_images );

        if (peak_index <= time_ECE_index) {

            for (int i = 0; i < n_images - 1; i++) {
                if(i<peak_index){
                    if(mean_intensity_superspel[n][i]< mean_intensity_superspel[n][i + 1]){
                        flag_passed++;
                    }
                }
                else {
                    if (mean_intensity_superspel[n][i] > mean_intensity_superspel[n][i + 1]) {
                        flag_passed++;
                    }
                }
            }
            for (int k = 0; k < n_images; k++) {
                if (mean_intensity_superspel[n][0] < mean_intensity_superspel[n][k]) {
                    flag_passed++;
                }
            }

            if (flag_passed == (2*n_images)-2){
                iftInsertSet(&possible_curves, n+1);
                printf("Label: %d\n",n+1);
                for (int i = 0; i < n_images; i++){
                    printf("%f\t",mean_intensity_superspel[n][i]);
                }
                printf("\n");

            }
        }
    }
    if(possible_curves == NULL){
        printf("No possible curves in this image set.\n");

    }

    return possible_curves;
}
float** iftMesoFinalMeanIntensitySuperspel(iftSet* possible_curves, int n_images, float** mean_intensity_superspel) {
    int set_size = iftSetSize(possible_curves);
    float **final_mean_intensity_superspels = (float **) iftAlloc(set_size, sizeof(float *));

    for(int i=0; i< set_size; i++){
        final_mean_intensity_superspels[i] = iftAllocFloatArray(n_images);
    }


    for (int k = set_size-1; k>= 0 && possible_curves!=0 ;k--) {
        for (int i = 0; i < n_images; i++) {
            final_mean_intensity_superspels[k][i] = mean_intensity_superspel[i][possible_curves->elem-1];
        }
        possible_curves= possible_curves->next;
    }
    return final_mean_intensity_superspels;
}

iftImage* iftMesoECECurveImageLabels(iftImage* label, iftIntArray* possible_curves_v){
    iftImage* final_label = iftCreateImageFromImage(label);

    for(int i = 0; i < label->n; i++){
        for(int j = 0; j < possible_curves_v->n; j++) {
            if (label->val[i] == possible_curves_v->val[j]) {
                final_label->val[i] = j+1;
            }
        }
    }
    return final_label;
}

float iftMesoMeanBackgroundSeeds(iftImage *img, iftLabeledSet* background_seeds){
    int   elem, sum = 0, count = 0;
    float mean=0;

    while (background_seeds != 0){
        if(background_seeds->label != 0){
            elem = background_seeds->elem;
            sum += img->val[elem];
            count++;
        }
        background_seeds = background_seeds->next;
    }
    if(count!=0) {
        mean = sum * (1.0) / count;
    }
    return mean;
}


void ifMesoCheckIfSeedsAreOutsideLabel(iftImage *label, iftLabeledSet *seeds) {
    iftLabeledSet *S = seeds;
    int        count = 0;
    while (S) {
        if (label->val[S->elem] == 0) {
            count++;
        }
        S = S->next;
    }
    printf("seeds outside: %d\n", count);
}

float* iftMesoSphericPatch(iftImage *Img,  iftVoxel u, iftAdjRel *A_patch){
    float *patch = iftAllocFloatArray(A_patch->n);

    for (int k = 0; k < A_patch->n; k++) {
        iftVoxel v = iftGetAdjacentVoxel(A_patch, u, k);
        if (iftValidVoxel(Img, v)) {
            int q = iftGetVoxelIndex(Img, v);
            patch[k] = Img->val[q];
        }
    }
    return patch;
}

int * iftMesoCreateIndexArray(int n) {
    int *indexes = iftAllocIntArray(n);

    for (int i = 0; i < n; i++) {
        indexes[i] = i;
    }
    return indexes;
}

void iftMesoSortCandidatesSeeds(iftLabeledSet **set) {
    int n        = iftLabeledSetSize(*set);
    int *indexes = iftMesoCreateIndexArray(n);

    int *differences = iftAllocIntArray(n);
    int *labels      = iftAllocIntArray(n);
    int *elems       = iftAllocIntArray(n);

    iftLabeledSet *S = *set;
    for (int i = 0; i < n && S != NULL; i++) {
        elems[i]  = S->elem;
        labels[i] = S->label;
        differences[i]   = S->handicap;

        S = S->next;
    }

    iftQuickSort(differences, indexes, 0, n - 1, IFT_DECREASING);

    iftLabeledSet *sorted_candidates = NULL;

    for (int i = 0; i < n; i++) {
        int index = indexes[i];

        int elem  = elems[index];
        int label = labels[index];
        int diff  = differences[i];

        iftInsertLabeledSetMarkerAndHandicap(&sorted_candidates, elem, label, 0, diff);
    }
    iftFree(indexes);
    iftFree(differences);
    iftFree(labels);
    iftFree(elems);
    iftDestroyLabeledSet(set);
    *set = sorted_candidates;
}

#define PROJECTION_RADIUS sqrtf(5.0)
#define PATCH_RADIUS      sqrtf(3.0)

iftLabeledSet *iftMesoGetCandidatesOfVoxelP(int p, int label, iftImage *img, iftImage *prev_img, iftImage *mask) {
    iftLabeledSet *candidates = NULL;

    iftAdjRel *A_projection = iftSpheric(PROJECTION_RADIUS);
    iftAdjRel *A_patch      = iftSpheric(PATCH_RADIUS);

    iftVoxel v_p      = iftGetVoxelCoord(img, p); // coordenada do centroide PREVIOUS
    float *patch_prev = iftMesoSphericPatch(prev_img, v_p, A_patch);

    for (int k = 0; k < A_projection->n; k++) {
        iftVoxel v_c = iftGetAdjacentVoxel(A_projection, v_p, k);
        if (iftValidVoxel(img, v_c)) {
            int q = iftGetVoxelIndex(img, v_c);
            if (mask->val[q]) {
                float *patch_curr = iftMesoSphericPatch(img, v_c, A_patch);
                int diff = iftComputeL1Norm(patch_curr, patch_prev, A_patch->n, NULL);
                iftInsertLabeledSetMarkerAndHandicap(&candidates, q,  label, 0, diff);
                iftFree(patch_curr);
            }
        }
    }


    iftDestroyAdjRel(&A_patch);
    iftDestroyAdjRel(&A_projection);
    iftFree(patch_prev);

    iftMesoSortCandidatesSeeds(&candidates);

    return candidates;
}

iftImage* iftMesoProjectSeedsOnNextImage(iftImage *prev_img, iftImage *img, iftImage *mask, iftMImage *mimg, iftLabeledSet *ith_centroids, char* SEGMENTATION_METHOD) {
    int number_of_centroids    = iftLabeledSetSize(ith_centroids);
    iftLabeledSet **candidates = (iftLabeledSet**) iftAlloc(number_of_centroids, sizeof (iftLabeledSet *));

    iftLabeledSet *centroids_with_adj_voxels_outside_mask = NULL;

    iftLabeledSet *centroid = ith_centroids;
    int i = 0;
    while (centroid != NULL) {  //3D

        int p     = centroid->elem;
        int label = centroid->label;

        candidates[i] = iftMesoGetCandidatesOfVoxelP(p, label, img, prev_img, mask);

        // if there is no candidate, then it means that all the adjacent voxels of the centroid are outside the mask
        if (iftLabeledSetSize(candidates[i]) == 0) {
            iftInsertLabeledSetMarkerAndHandicap(&centroids_with_adj_voxels_outside_mask, p, centroid->label, 0, i);
        } else {
            // printf("-- Processed the %d-th seed...\n", ++count_processed_seeds);
        }

        i++;
        centroid = centroid->next;
    }

    iftAdjRel *A = iftSpheric(1.0);
    iftLabeledSet *current_centroid_outside = centroids_with_adj_voxels_outside_mask;
    iftImage *border_mask = iftBorderImage(mask, false);

    while (current_centroid_outside != NULL) {

        iftVoxel v = iftGetVoxelCoord(img, current_centroid_outside->elem);

        if (iftValidVoxel(img, v)) {
            iftImage *label_centroids = iftCreateImageFromImage(img);
            int p = current_centroid_outside->elem;
            int label = current_centroid_outside->label;
            label_centroids->val[p] = label;

            // obter o menor valor dentro na borda da mascara

            int p_tmp = -1;
            int minimum_distance = IFT_INFINITY_INT;
            for (int p = 0; p < border_mask->n; p++) {
                if (border_mask->val[p]) {
                    iftVoxel u = iftGetVoxelCoord(img, p);
                    int dist = iftSquaredVoxelDistance(u,v);
                    if (minimum_distance > dist ) {
                        p_tmp = p;
                        minimum_distance = dist;
                    }
                }
            }

            int j = current_centroid_outside->handicap;
            p = p_tmp;
            label = current_centroid_outside->label;
            candidates[j] = iftMesoGetCandidatesOfVoxelP(p, label, img, prev_img, mask);
            iftDestroyImage(&label_centroids);
        }
        current_centroid_outside = current_centroid_outside->next;

    }
    iftDestroyImage(&border_mask);

    // FAZER MATCHING
    iftLabeledSet *resulting_seeds = NULL;

    for (int i = 0; i < number_of_centroids; i++) {
        // adicionar em resulting seeds a semente selecionada
        if (candidates[i]) {
            int elem  = candidates[i]->elem;
            int label = candidates[i]->label;
            iftInsertLabeledSet(&resulting_seeds, elem, label);

            for (int j = i + 1; j < number_of_centroids; j++) {
                if (iftLabeledSetHasElement(candidates[j], elem)) {
                    iftRemoveLabeledSetElem(&candidates[j], elem);
                }
            }
        }
    }


    iftImage *label=NULL;

    if(!strcmp (SEGMENTATION_METHOD, "Watershed")){

        iftImage * grad = iftImageBasins(img,A);
        label = iftWatershed(grad, A, resulting_seeds, NULL);
    }
    else if(!strcmp (SEGMENTATION_METHOD, "DynamicTrees")){label = iftDynamicSetRootPolicyInMask(mimg, A, resulting_seeds, mask);
    }
    
    iftDestroyAdjRel(&A);
    iftDestroyLabeledSet(&resulting_seeds);
    iftDestroyLabeledSet(&centroids_with_adj_voxels_outside_mask);


    return label;
}

float* iftMesoMeanIntensityCurves(iftImage *img, iftImage * label, int n_label){
    int* array_n_label             = iftAllocIntArray(n_label+1);
    float* intensities_labels      = iftAllocFloatArray(n_label+1);
    float* intensities_mean_labels = iftAllocFloatArray(n_label);

    //ideia , iterar pelos labels, acrescentar o tamanho no v[label], pegar as coordenadas e olhar a intensidade do voxel, salvar em intensidade[label]
    //intensidade media[label]/tamanho[label]

    for(int i=0;i<label->n;i++){

        if(label->val[i]!=0) {

            int current_label = label->val[i];
            array_n_label[current_label]++;

            intensities_labels[current_label] += img->val[i];

        }
    }
    for(int i = 1; i <= n_label; i++){

        intensities_mean_labels[i-1] = intensities_labels[i]/array_n_label[i];
    }
    iftFree(intensities_labels);
    iftFree(array_n_label);

    return intensities_mean_labels;
}
void iftMesoIntensitiesFileOutput(float* intensities, int n_label, int n_img){

    char name[20];
    sprintf(name, "f%d.txt",n_img);
    FILE *fp = fopen(name,"w");
    for(int i = 0; i < n_label; i++){
        fprintf(fp, "%f ", intensities[i]);
    }
    fclose(fp);
}
float** iftMeanIntensitySuperspelsFromImageAndLabel(iftImage** imgs, iftImage** labels_ECE, int n_imgs){
    int n_labels = iftMaximumValue(labels_ECE[0]);
    float** mean_intensity_superspels = (float**) iftAlloc(n_imgs, sizeof(float*));

    for(int i = 0; i < n_imgs; i++) {
        mean_intensity_superspels[i]= iftMesoMeanIntensityCurves(imgs[i], labels_ECE[i], n_labels);
    }
    return mean_intensity_superspels;
}
iftImage** iftMesoMaskDilation(iftImage** imgs, iftImage** fluid_masks, float dilation_ratio, int n_images){

    iftImage **masks_meso   = (iftImage**) iftAlloc(n_images, sizeof(iftImage*));
    for (int i = 0; i < n_images; i++) {
        masks_meso[i]    = iftMesoRegionContainingPleura(imgs[i], fluid_masks[i], dilation_ratio);
    }
    return masks_meso;
}

iftImage** iftMesoSuperspels(iftImage** imgs, iftImage** masks_meso, iftLabeledSet** background_seeds,
                             int num_init_seeds, int num_superpixels,
                             char* SUPERVOXEL_METHOD, char* SEGMENTATION_METHOD, int n_images,
                             float*** mean_intensity_superspels){

    iftImage **orig_imgs = (iftImage **) iftAlloc(n_images, sizeof(iftImage *));
    iftMImage **mimgs       = (iftMImage**) iftAlloc(n_images, sizeof(iftMImage*));
    iftImage **labels       = (iftImage**) iftAlloc(n_images, sizeof(iftImage*));

    for (int i = 0; i < n_images; i++) {

        iftImage *masked = iftMask(imgs[i], masks_meso[i]);
        orig_imgs[i] = imgs[i];//iftDestroyImage(&imgs[i]);
        imgs[i] = masked;
        mimgs[i] = iftImageToMImage(imgs[i], GRAY_CSPACE);
    }

    if(!strcmp (SUPERVOXEL_METHOD, "DISF")){
        iftAdjRel *A_DISF;
        A_DISF = iftSpheric(sqrtf(1.0));
        labels[0] = iftDISF(mimgs[0], A_DISF, num_init_seeds, num_superpixels, masks_meso[0]); // DISF result

    }
    else if(!strcmp (SUPERVOXEL_METHOD, "SICLE")){

        iftSICLEArgs *sargs;
        iftSICLE *sicle;

        sargs = iftCreateSICLEArgs();
        sargs->n0 = num_init_seeds;
        sargs->nf = num_superpixels;
        sicle = iftCreateSICLE(imgs[0], NULL, masks_meso[0]);
        labels[0] = iftRunSICLE(sicle, sargs);

    }
    // calcular o centroide dos superpixels da imagem 1

    iftLabeledSet **centroids = (iftLabeledSet**) iftAlloc(n_images, sizeof(iftLabeledSet*));
    centroids[0] = iftGeodesicCenters(labels[0]);

    *(mean_intensity_superspels) = (float**) iftAlloc(n_images, sizeof(float*));
    float*  mean_background_imgs      = iftAllocFloatArray(n_images);

    *(mean_intensity_superspels)[0] = iftMesoMeanIntensityCurves(imgs[0], labels[0], num_superpixels);
    mean_background_imgs[0]      = iftMesoMeanBackgroundSeeds(orig_imgs[0],background_seeds[0]);

    for (int i = 1; i < n_images; i++) {
        printf("Processing %d-th image...\n", i+1);
        labels[i]                   = iftMesoProjectSeedsOnNextImage(imgs[i-1], imgs[i], masks_meso[i], mimgs[i], centroids[i-1], SEGMENTATION_METHOD);
        centroids[i]                = iftGeodesicCenters(labels[i]);

        *(mean_intensity_superspels)[i]= iftMesoMeanIntensityCurves(imgs[i],labels[i],num_superpixels);
        mean_background_imgs[i]     = iftMesoMeanBackgroundSeeds(orig_imgs[i],background_seeds[i]);

        for(int j=0;j<num_superpixels;j++){
            *(mean_intensity_superspels)[i][j] -= mean_background_imgs[i];
        }
    }

return labels;
}
iftImage** iftMesoSelectLabels(iftImage** imgs, iftImage** labels, float** mean_intensity_superspels, int n_images,
                               int num_superpixels, int* time_sec, float*** final_mean_intensity_superspels){

    iftSet* possible_curves  = NULL;

    possible_curves = iftMesoECECurveLabelSelection(mean_intensity_superspels, num_superpixels, n_images,time_sec);
    *final_mean_intensity_superspels = iftMesoFinalMeanIntensitySuperspel(possible_curves, n_images, mean_intensity_superspels);
    iftImage **labels_ECE   = (iftImage**) iftAlloc(n_images, sizeof(iftImage*));

    if (possible_curves != NULL) {

        iftIntArray *possible_curves_array = iftSetToArray(possible_curves);


        for (int i = 0; i < n_images; i++) {
            labels_ECE[i] = iftMesoECECurveImageLabels(labels[i], possible_curves_array);
        }
    }
    return labels_ECE;
}

iftImage ** iftMesoAutoECE(iftImage** imgs, iftImage** masks_fluid,  float  max_dist, int num_init_seeds,
                           int num_superpixels, int n_images, iftLabeledSet** background_seeds, int* time_sec, char* SUPERVOXEL_METHOD, char* SEGMENTATION_METHOD,
                           float*** final_mean_intensity_superspels){

    iftImage **masks_meso   = (iftImage**) iftAlloc(n_images, sizeof(iftImage*));
    iftMImage **mimgs       = (iftMImage**) iftAlloc(n_images, sizeof(iftMImage*));
    iftImage **labels       = (iftImage**) iftAlloc(n_images, sizeof(iftImage*));
    iftImage **labels_ECE   = (iftImage**) iftAlloc(n_images, sizeof(iftImage*));
    iftImage **orig_imgs    = (iftImage**) iftAlloc(n_images, sizeof(iftImage*));

    for (int i = 0; i < n_images; i++) {
        masks_meso[i]    = iftMesoRegionContainingPleura(imgs[i], masks_fluid[i], max_dist);

        iftImage *masked = iftMask(imgs[i], masks_meso[i]);
        orig_imgs[i] = imgs[i];//iftDestroyImage(&imgs[i]);
        imgs[i]  = masked;
        mimgs[i] = iftImageToMImage(imgs[i],GRAY_CSPACE);
    }

    // Validate inputs
    if(num_init_seeds < 0) iftError("Non-positive N_0 = %d", "main", num_init_seeds);
    if(num_superpixels < 0) iftError("Non-positive N_f = %d", "main", num_superpixels);
    if(num_superpixels >= num_init_seeds) iftError("N_f >= N_0", "main");

    for (int i = 0; i < n_images; i++)
        iftVerifyImageDomains(imgs[i], masks_meso[i], "main");

    if(!strcmp (SUPERVOXEL_METHOD, "DISF")){
        iftAdjRel *A_DISF;
        A_DISF = iftSpheric(sqrtf(1.0));
        labels[0] = iftDISF(mimgs[0], A_DISF, num_init_seeds, num_superpixels, masks_meso[0]); // DISF result
        iftDestroyAdjRel(&A_DISF);

    }
    else if(!strcmp (SUPERVOXEL_METHOD, "SICLE")){

        iftSICLEArgs *sargs;
        iftSICLE *sicle;

        sargs = iftCreateSICLEArgs();
        sargs->n0 = num_init_seeds;
        sargs->nf = num_superpixels;
        sicle = iftCreateSICLE(imgs[0], NULL, masks_meso[0]);
        labels[0] = iftRunSICLE(sicle, sargs);
        iftDestroySICLE(&sicle);
	iftDestroySICLEArgs(&sargs);

    }

    // calcular o centroide dos superpixels da imagem 1

    iftLabeledSet **centroids = (iftLabeledSet**) iftAlloc(n_images, sizeof(iftLabeledSet*));

    centroids[0] = iftGeodesicCenters(labels[0]);

    float** mean_intensity_superspels = (float**) iftAlloc(n_images, sizeof(float*));
    float*  mean_background_imgs      = iftAllocFloatArray(n_images);

    mean_intensity_superspels[0] = iftMesoMeanIntensityCurves(imgs[0], labels[0], num_superpixels);
    mean_background_imgs[0]      = iftMesoMeanBackgroundSeeds(orig_imgs[0],background_seeds[0]);

    //intensities_file_output(intensities_mean_per_img[0],iftLabeledSetSize(centroids[0]),0);
    for (int i = 1; i < n_images; i++) {
        printf("Processing %d-th image...\n", i+1);

        labels[i]                   = iftMesoProjectSeedsOnNextImage(imgs[i-1], imgs[i], masks_meso[i], mimgs[i], centroids[i-1], SEGMENTATION_METHOD);
        centroids[i]                = iftGeodesicCenters(labels[i]);
        mean_intensity_superspels[i]= iftMesoMeanIntensityCurves(imgs[i],labels[i],num_superpixels);
        mean_background_imgs[i]     = iftMesoMeanBackgroundSeeds(orig_imgs[i],background_seeds[i]);

        for(int j=0;j<num_superpixels;j++){
            mean_intensity_superspels[i][j] -= mean_background_imgs[i];
        }
        //intensities_file_output(intensities_mean_per_img[i], iftLabeledSetSize(centroids[i]), i);
    }

    iftSet* possible_curves  = NULL;


    possible_curves = iftMesoECECurveLabelSelection(mean_intensity_superspels, num_superpixels, n_images,time_sec);
    *final_mean_intensity_superspels = iftMesoFinalMeanIntensitySuperspel(possible_curves, n_images, mean_intensity_superspels);

    if (possible_curves != NULL) {

        iftIntArray* possible_curves_array = iftSetToArray(possible_curves);


        for(int i = 0; i < n_images; i++) {
            labels_ECE[i] = iftMesoECECurveImageLabels(labels[i], possible_curves_array);
        }

        if (iftDirExists("tmp")) iftRemoveDir("tmp");
        iftMakeDir("tmp");
        for (int i = 0; i < n_images; i++) {
            char filename[255];
            sprintf(filename, "%03d", i + 1);

            iftWriteSeeds(centroids[i],labels[i], "tmp/%s-centers.txt", filename);
            iftWriteImageByExt(labels[i], "tmp/label/%s.nii.gz", filename);
            iftWriteImageByExt(masks_fluid[i], "tmp/masks_fluid/%s.nii.gz", filename);
            iftWriteImageByExt(masks_meso[i], "tmp/masks_meso/%s.nii.gz", filename);
            iftWriteImageByExt(labels_ECE[i], "tmp/selected_labels/%s.nii.gz", filename);
        }

        for (int i = 0; i < n_images; i++) {
            char filename[255];
            sprintf(filename, "%03d", i + 1);

            iftWriteSeeds(centroids[i],labels[i], "tmp/%s-centers.txt", filename);
            iftWriteImageByExt(labels[i], "tmp/label/%s.nii.gz", filename);
            iftWriteImageByExt(masks_fluid[i], "tmp/masks_fluid/%s.nii.gz", filename);
            iftWriteImageByExt(masks_meso[i], "tmp/masks_meso/%s.nii.gz", filename);
            iftWriteImageByExt(labels_ECE[i], "tmp/selected_labels/%s.nii.gz", filename);
        }

        iftDestroyIntArray(&possible_curves_array);
    }


    // Clear
    for (int i = 0; i < n_images; i++) {
        iftDestroyImage(&masks_meso[i]);
        iftDestroyImage(&labels[i]);
        iftDestroyImage(&orig_imgs[i]);
        iftDestroyMImage(&mimgs[i]);
        iftDestroyLabeledSet(&centroids[i]);
        iftFree(mean_intensity_superspels[i]);

    }
    iftDestroySet(&possible_curves);
  
    iftFree(mean_intensity_superspels);
    iftFree(mean_background_imgs);
    iftFree(masks_meso);
    iftFree(orig_imgs);
    iftFree(labels);
    iftFree(mimgs);
    iftFree(centroids);

    return labels_ECE;
}

iftImage ** iftMesoAutoECE2(iftImage** imgs, iftImage** masks_fluid,  float  max_dist, int num_init_seeds,
                           int num_superpixels, int n_images, iftLabeledSet** background_seeds, int* time_sec,
                           char* SUPERVOXEL_METHOD, char* SEGMENTATION_METHOD, float*** final_mean_intensity_superspels){

    iftImage **masks_meso   = (iftImage**) iftAlloc(n_images, sizeof(iftImage*));
    iftMImage **mimgs       = (iftMImage**) iftAlloc(n_images, sizeof(iftMImage*));
    iftImage **labels       = (iftImage**) iftAlloc(n_images, sizeof(iftImage*));
    iftImage **labels_ECE   = (iftImage**) iftAlloc(n_images, sizeof(iftImage*));
    iftImage **orig_imgs    = (iftImage**) iftAlloc(n_images, sizeof(iftImage*));

    for (int i = 0; i < n_images; i++) {
        masks_meso[i]    = iftMesoRegionContainingPleura(imgs[i], masks_fluid[i], max_dist);

        iftImage *masked = iftMask(imgs[i], masks_meso[i]);
        orig_imgs[i] = imgs[i];//iftDestroyImage(&imgs[i]);
        imgs[i]  = masked;
        mimgs[i] = iftImageToMImage(imgs[i],GRAY_CSPACE);
    }

    // Validate inputs
    if(num_init_seeds < 0) iftError("Non-positive N_0 = %d", "main", num_init_seeds);
    if(num_superpixels < 0) iftError("Non-positive N_f = %d", "main", num_superpixels);
    if(num_superpixels >= num_init_seeds) iftError("N_f >= N_0", "main");

    for (int i = 0; i < n_images; i++)
        iftVerifyImageDomains(imgs[i], masks_meso[i], "main");

    int i_time_270 = iftMesoGetIndexofOrdenatedIntArrayfromValue(time_sec,n_images, 270);

    if(!strcmp (SUPERVOXEL_METHOD, "DISF")){
        iftAdjRel *A_DISF;
        A_DISF = iftSpheric(sqrtf(1.0));
        labels[i_time_270] = iftDISF(mimgs[i_time_270], A_DISF, num_init_seeds,
                                     num_superpixels, masks_meso[i_time_270]); // DISF result

    }
    else if(!strcmp (SUPERVOXEL_METHOD, "SICLE")){

        iftSICLEArgs *sargs;
        iftSICLE *sicle;

        sargs = iftCreateSICLEArgs();
        sargs->n0 = num_init_seeds;
        sargs->nf = num_superpixels;

        sicle = iftCreateSICLE(imgs[i_time_270], NULL, masks_meso[i_time_270]);
        labels[i_time_270] = iftRunSICLE(sicle, sargs);
    }

    // calcular o centroide dos superpixels da imagem 1

    iftLabeledSet **centroids = (iftLabeledSet**) iftAlloc(n_images, sizeof(iftLabeledSet*));

    centroids[i_time_270] = iftGeodesicCenters(labels[i_time_270]);


    for(int i=0;i<n_images;i++){

        if(i!= i_time_270){
            labels[i] = iftMesoProjectSeedsOnNextImage(imgs[i_time_270],
                                                       imgs[i], masks_meso[i],
                                                       mimgs[i], centroids[i_time_270],
                                                       SEGMENTATION_METHOD);
        }
    }

    float** mean_intensity_superspels = (float**) iftAlloc(n_images, sizeof(float*));
    float*  mean_background_imgs      = iftAllocFloatArray(n_images);

    mean_intensity_superspels[0] = iftMesoMeanIntensityCurves(imgs[0], labels[0], num_superpixels);
    mean_background_imgs[0]      = iftMesoMeanBackgroundSeeds(orig_imgs[0],background_seeds[0]);

    //intensities_file_output(intensities_mean_per_img[0],iftLabeledSetSize(centroids[0]),0);

    for (int i = 1; i < n_images; i++) {
        mean_intensity_superspels[i]= iftMesoMeanIntensityCurves(imgs[i],labels[i],num_superpixels);
        mean_background_imgs[i]     = iftMesoMeanBackgroundSeeds(orig_imgs[i],background_seeds[i]);

        for(int j=0;j<num_superpixels;j++){
            mean_intensity_superspels[i][j] -= mean_background_imgs[i];
        }
        //intensities_file_output(intensities_mean_per_img[i], iftLabeledSetSize(centroids[i]), i);
    }

    iftSet* possible_curves  = NULL;


    possible_curves = iftMesoECECurveLabelSelection(mean_intensity_superspels, num_superpixels, n_images,time_sec);
    *final_mean_intensity_superspels = iftMesoFinalMeanIntensitySuperspel(possible_curves, n_images, mean_intensity_superspels);

    if (possible_curves != NULL) {

        iftIntArray* possible_curves_array = iftSetToArray(possible_curves);


        for(int i = 0; i < n_images; i++) {
            labels_ECE[i] = iftMesoECECurveImageLabels(labels[i], possible_curves_array);
        }


        if (iftDirExists("tmp")) iftRemoveDir("tmp");
        iftMakeDir("tmp");
        for (int i = 0; i < n_images; i++) {
            char filename[255];
            sprintf(filename, "%03d", i + 1);

            iftWriteSeeds(centroids[i],labels[i], "tmp/%s-centers.txt", filename);
            iftWriteImageByExt(labels[i], "tmp/label/%s.nii.gz", filename);
            iftWriteImageByExt(masks_fluid[i], "tmp/masks_fluid/%s.nii.gz", filename);
            iftWriteImageByExt(masks_meso[i], "tmp/masks_meso/%s.nii.gz", filename);
            iftWriteImageByExt(labels_ECE[i], "tmp/selected_labels/%s.nii.gz", filename);
        }

        for (int i = 0; i < n_images; i++) {
            char filename[255];
            sprintf(filename, "%03d", i + 1);

            iftWriteSeeds(centroids[i],labels[i], "tmp/%s-centers.txt", filename);
            iftWriteImageByExt(labels[i], "tmp/label/%s.nii.gz", filename);
            iftWriteImageByExt(masks_fluid[i], "tmp/masks_fluid/%s.nii.gz", filename);
            iftWriteImageByExt(masks_meso[i], "tmp/masks_meso/%s.nii.gz", filename);
            iftWriteImageByExt(labels_ECE[i], "tmp/selected_labels/%s.nii.gz", filename);
        }

        iftDestroyIntArray(&possible_curves_array);
    }


    // Clear
    for (int i = 0; i < n_images; i++) {
        iftDestroyImage(&masks_meso[i]);
        iftDestroyImage(&labels[i]);
        iftDestroyImage(&orig_imgs[i]);
        iftDestroyMImage(&mimgs[i]);
        iftDestroyLabeledSet(&centroids[i]);
        iftFree(mean_intensity_superspels[i]);

    }
    iftDestroySet(&possible_curves);
    //iftDestroyAdjRel(&A_DISF);

    iftFree(mean_intensity_superspels);
    iftFree(mean_background_imgs);
    iftFree(masks_meso);
    iftFree(orig_imgs);
    iftFree(labels);
    iftFree(mimgs);
    iftFree(centroids);

    return labels_ECE;
}



