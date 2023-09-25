//
// Created by azael on 01/06/17.
//

#include "ift.h"

typedef struct energy{
    int p, q;
    struct energy *next;
} iftEnergySet;

void iftInsertEnergySet(iftEnergySet **E, int p, int q){

    iftEnergySet *s = NULL;

    s = (iftEnergySet *) iftAlloc(1,sizeof(iftEnergySet));
    s->p = p;
    s->q = q;
    if (*E == NULL)
        s->next = NULL;
    else
        s->next = *E;
    *E = s;
}

void iftRemoveEnergySet(iftEnergySet **E, int *p, int *q){

    iftEnergySet *s;

    if (*E != NULL){
        s = *E;
        *p = s->p;
        *q = s->q;
        *E = s->next;
        iftFree(s);
    }
}

void iftFreeFullEnergySet(iftEnergySet **E){

    iftEnergySet *s = NULL;

    while (*E != NULL){
        s = *E;
        *E = s->next;
        free(s);
    }

}

int iftOptimumThreshold(iftImage *img, int low, int top, float lambda){

    int p, q, r, K, dif, optimum;
    float energy, energyMin;
    float *h, *H, *G;
    iftAdjRel *A;

    iftEnergySet *E,*e;
    E = e = NULL;

    optimum = 1;

    puts("Initializing Energy Set");

    K = iftMaximumValue(img);
    h = iftAllocFloatArray(K+1);
    A = iftSpheric(1.0);

    for (p=0; p < img->n; p++){
        h[img->val[p]]++;
        q = p;
        dif = IFT_INFINITY_INT_NEG;
        iftVoxel u = iftGetVoxelCoord(img,p);
        for (r = 1; r < A->n; r++){
            iftVoxel v = iftGetAdjacentVoxel(A,u,r);
            if (iftValidVoxel(img,v)) {
                int s = iftGetVoxelIndex(img, v);
                if (dif < (img->val[p] - img->val[s])) {
                    dif = (img->val[p] - img->val[s]);
                    q = s;
                }
            }
        }
        if (img->val[p] > img->val[q])
            iftInsertEnergySet(&E, p, q);
    }
    iftDestroyAdjRel(&A);

    puts("Creating Histograms");;

    H = iftAllocFloatArray(K+1);
    G = iftAllocFloatArray(K+1);

    for (int k=0; k <= K; k++){
        H[k] = H[k-1] + k*h[k];
        G[k] = G[k-1] + (K-k-1)*h[k-1];
    }

    puts("Finding Miminum of Energy Function");
    FILE *fp = fopen("energy.txt","w");

    
    energyMin = IFT_INFINITY_FLT;
    for (int k=low; k < top-1; k++){
      energy = (H[K] - H[k] + G[k])*lambda;
      e = E;
      while (e != NULL) {
	p = e->p;
	q = e->q;
	if ((img->val[p] >= k) && (img->val[q] < k)){
	  energy += (K - img->val[p] + img->val[q]);
	}
	e = e->next;
      }
      fprintf(fp,"%f\n",energy);
        if (energy < energyMin) {
            energyMin = energy;
            optimum = k;
        }
    }
    fclose(fp);
    
    free(h);
    free(H);
    free(G);
    iftFreeFullEnergySet(&E);

    return optimum;
}

int main(int argc, char *argv[]){

    if ((argc < 4) || (argc > 6)){
        iftError("Usage: iftOptimumThreshold <image.[ppm,pgm,png,scn]> <output_image.[pgm,png,scn]> <Lambda> [0,1] <OPTIONAL starting intensity> <OPTIONAL ending intensity>","main");
    }

    char *img_path = argv[1];
    char *out_path = argv[2];
    float lambda = atof(argv[3]);
    int start,end, K, threshold;
    iftImage *img, *th;

    img = iftReadImageByExt(img_path);
    K = iftMaximumValue(img);
    start=1; end = K;
    
    if (argc == 5){
        start = atoi(argv[4]);
        if (start < 1){
            start = 1;
        }
    } else {
      if (argc == 6) {
	start = atoi(argv[4]);
        end = atoi(argv[5]);
        if (end > K){
	  end = K;
        }
      }
    }
      
    printf("start %d end %d\n",start,end);
    threshold = iftOptimumThreshold(img,start,end,lambda);

    printf("Optimum Threhsold: %d\n",threshold);

    puts("Performing Threshold");

    th = iftThreshold(img,threshold,K,255);

    iftWriteImageByExt(th,out_path);

    return 1;
}
