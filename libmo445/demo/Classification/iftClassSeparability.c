//
// Created by azael on 08/11/19.
//
// This program generates the separability/compactness matrix for
// neural network architecture optimization. It works as a guide to
// show how samples of distinct classes are separated in the high
// dimensional space and how the samples of the same class are
// clusterized (compactness).
//
// The separability/compactness matrix can be interpreted as follows:
//
//      +------+------+------+------+
//      |      |  c1  |  c2  |  c3  |
//      +------+------+------+------+
//      |  c1  | 1.12 | 20.8 | 12.1 |
//      +------+------+------+------+
//      |  c2  | 20.8 | 0.65 | 18.5 |
//      +------+------+------+------+
//      |  c3  | 12.1 | 18.5 | 1.98 |
//      +------+------+------+------+
//
// The main diagonal represents the compactness of each class, that is,
// the mean distance between samples of one single class (inter-class
// compactness), whereas the superior/inferior triangle shows how far the
// samples from distinct classes are (intra-class separability). Therefore
// we aim to minimize the main diagonal and maximize the elements in the
// superior/inferior matrix.
//

#include "ift.h"

double iftNeighborsOfDistinctClass(iftMatrix *it)
{
    int row,col;
    int acc = 0;
    int total = 0;

    for (row = 0; row < it->nrows; row++){
        for (col = 0; col < it->ncols; col++){
            if (row != col){
                acc += it->val[iftGetMatrixIndex(it,col,row)];
            }
            total += it->val[iftGetMatrixIndex(it,col,row)];
        }
    }

    return ((double)acc)/total;            
}

double iftComputeSeparability(iftMatrix *M)
{
    iftFloatArray *arr = iftCreateFloatArray(M->nrows-1);
    iftIntArray *idx = iftIntRange(0, arr->n-1, 1);
    float median;
    float acc;
    int pos;

    acc = 0;
    for (int row = 0; row < M->nrows; row++){
        pos = 0;
        for (int col = 0; col < M->ncols; col++)
            if (row != col)
                arr->val[pos++] = M->val[iftGetMatrixIndex(M,col,row)];
        iftFQuickSort(arr->val, idx->val, 0, arr->n-1, IFT_INCREASING);
        median = arr->val[arr->n / 2];
        acc += M->val[iftGetMatrixIndex(M,row,row)]/median;
    }

    return acc / M->nrows;
}


int main(int argc, char *argv[])
{
    timer *tstart = NULL;

    if (argc != 5)
        iftError("Usage: iftClassSeparability <...>\n"
                         "[1] input_dataset: Input zip file containing the mimage samples.\n"
                         "[2] k: Value of K used to build the K-NN graph.\n"
                         "[3] n: Dimension of the reduced vector space.\n"
                         "[4] output_csv_matrix: Output separability matrix in a csv file.\n",
                 "main");

    tstart = iftTic();

    iftDataSet *Z    = iftReadDataSet(argv[1]);
    if (Z->nclasses == 0)
      iftError("It requires annotated datasets","main");
    
    int k            = atoi(argv[2]);
    int n            = atoi(argv[3]);

    iftMatrix *dist  = iftHighDimensionalDistanceEstimationBasedOnPCA(Z,k,n);
    
    int nclasses     = Z->nclasses;
    iftMatrix *acc   = iftCreateMatrix(nclasses,nclasses);
    iftMatrix *it    = iftCreateMatrix(nclasses,nclasses);
    iftMatrix *mean  = NULL;
    int i,j,size;
    double score;
    double neighbor_class;

    for (i = 0; i < dist->nrows; i++)
        for (j = 0; j < dist->ncols; j++)
            if (dist->val[iftGetMatrixIndex(dist,j,i)] > 0.0){
                acc->val[iftGetMatrixIndex(acc,Z->sample[j].truelabel-1,Z->sample[i].truelabel-1)] += dist->val[iftGetMatrixIndex(dist,j,i)];
                it->val[iftGetMatrixIndex(it,Z->sample[j].truelabel-1,Z->sample[i].truelabel-1)]++;
            }
    iftDestroyMatrix(&dist);

    size = acc->ncols * acc->nrows;
    mean = iftCreateMatrix(nclasses,nclasses);
    for (i = 0; i < size; i++) {
        if (iftAlmostZero(it->val[i]))
            mean->val[i] = IFT_INFINITY_DBL;
        else
            mean->val[i] =  acc->val[i] / it->val[i];
    }
    iftDestroyMatrix(&acc);

    iftWriteMatrixCSV(mean,argv[4]);

    FILE *fp = fopen(argv[4],"a");
    neighbor_class = iftNeighborsOfDistinctClass(it);
    fprintf(fp,"%f\n",neighbor_class);
    score = iftComputeSeparability(mean);
    fprintf(fp,"%f\n",score);
    fclose(fp);
    iftDestroyMatrix(&mean);
    iftDestroyMatrix(&it);

    printf("Done ... %s\n", iftFormattedTime(iftCompTime(tstart, iftToc())));

    return 0;
}
