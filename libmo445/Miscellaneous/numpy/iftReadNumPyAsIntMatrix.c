#include "ift.h"




int main(int argc, const char *argv[]) {
    if (argc != 2)
        iftError("%s <numpy_file.npy>", "main", argv[0]);

    puts("- Reading numpy array");
    iftIntMatrix *mat = iftReadIntMatrix(argv[1]);

    puts("[");
    for (int i = 0; i < mat->nrows; i++) {
        printf("  [");
        
        for (size_t j = 0; j < (mat->ncols-1); j++)
            printf("%d, ", iftMatrixElem(mat, j, i));
        printf("%d],\n", iftMatrixElem(mat, mat->ncols-1, i));
    }
    puts("]\n");

    puts("- Saving the numpy array: 2D_int32.npy");
    iftWriteIntMatrix(mat, "2D_int32.npy");

    iftDestroyIntMatrix(&mat);

    return 0;
}


