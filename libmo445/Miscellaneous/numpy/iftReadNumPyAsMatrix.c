#include "ift.h"




int main(int argc, const char *argv[]) {
    if (argc != 2)
        iftError("%s <numpy_file.npy>", "main", argv[0]);

    puts("- Reading numpy array");
    iftMatrix *mat = iftReadMatrix(argv[1]);

    puts("[");
    for (int i = 0; i < mat->nrows; i++) {
        printf("  [");
        for (size_t j = 0; j < (mat->ncols-1); j++)
            printf("%.2f, ", iftMatrixElem(mat, j, i));
        printf("%.2f],\n", iftMatrixElem(mat, mat->ncols-1, i));
    }
    puts("]\n");

    puts("- Saving the numpy array: 2D_float32.npy");
    iftWriteMatrix(mat, "2D_float32.npy");

    iftDestroyMatrix(&mat);

    return 0;
}


