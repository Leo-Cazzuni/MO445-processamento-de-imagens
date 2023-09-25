//
// Created by azaelmsousa on 01/11/20.
//

#include "ift.h"

int main(int argc, char *argv[]) {

    if ((argc < 4) || (argc > 5)){
        iftError("iftFindContrastReferenceImage <...>\n"
                 "[1] Directory with normalized images\n"
                 "[2] Directory with labeled images (optional)\n"
                 "[3] Dome neighborhood\n"
                 "[4] Output CSV file","main");
    }

    char *img_set_path = argv[1];
    int neighbor = atoi(argv[2]);
    char *output_path  = argv[3];
    char *lbl_set_path = NULL;

    if (argc == 5){
        lbl_set_path = argv[2];
        neighbor = atoi(argv[3]);
        output_path = argv[4];
    }

    iftFileSet *img_set = iftLoadFileSetFromDirOrCSV(img_set_path, 0, true);
    iftFileSet *lbl_set = NULL;
    if (lbl_set_path != NULL)
        lbl_set = iftLoadFileSetFromDirOrCSV(lbl_set_path, 0, true);

    iftIntArray *dist = iftCreateIntArray(img_set->n);

    for (int i = 0; i < img_set->n; i++){
        iftImage *img = iftReadImageByExt(img_set->files[i]->path);
        iftHist *H = NULL;

        if (lbl_set != NULL){
            iftImage *lbl = iftReadImageByExt(lbl_set->files[i]->path);
            H = iftCalcGrayImageHist(img,lbl,iftMaximumValue(img)+1,4095,true);
            iftDestroyImage(&lbl);
        } else {
            H = iftCalcGrayImageHist(img,NULL,iftMaximumValue(img)+1,4095,true);
        }
        iftDestroyImage(&img);

        iftSet *S = NULL;
        for (int j = 0; j < H->nbins; j++){
            float ref = H->val[j];
            bool is_dome = true;
            for (int n = -neighbor; n <= neighbor; n++){
                if (j+n < 0)
                    continue;
                if (j+n >= H->nbins)
                    break;
                if (H->val[j+n] > ref) {

                    is_dome = false;
                    break;
                }
            }
            if (is_dome)
                iftInsertSet(&S,j);
        }

        float max[2] = {0,0};
        int max_ind[2] = {0,0};

        while(S != NULL){
            int elem = iftRemoveSet(&S);
            if (H->val[elem] > max[0]){

                max[1] = max[0];
                max_ind[1] = max_ind[0];

                max[0] = H->val[elem];
                max_ind[0] = elem;
            }
        }

        dist->val[i] = abs(max_ind[0] - max_ind[1]);

        puts(img_set->files[i]->path);
        printf("max1: %f\n",max[0]);
        printf("max_ind1: %d\n",max_ind[0]);
        printf("H->val[max_ind1]: %f\n",H->val[max_ind[0]]);
        printf("max2: %f\n",max[1]);
        printf("max_ind2: %d\n",max_ind[1]);
        printf("H->val[max_ind2]: %f\n",H->val[max_ind[1]]);
        printf("dist: %d\n",dist->val[i]);
        puts("---------------");
        iftDestroyHist(&H);
    }

    int max = -1;
    int max_ind = 0;
    for (int i = 0; i < dist->n; i++){
        if (max < dist->val[i]){
            max = dist->val[i];
            max_ind = i;
        }
    }

    printf("Chosen file: %s\n",img_set->files[max_ind]->path);

    // Writing output file
    FILE *f;
    f = fopen(output_path, "w");
    if (f == NULL)
        iftError("Cannot open csv file", "main");
    fprintf(f,"%s\n",img_set->files[max_ind]->path);
    fclose(f);

    iftDestroyFileSet(&img_set);
    iftDestroyFileSet(&lbl_set);
    iftDestroyIntArray(&dist);

    return 0;

}