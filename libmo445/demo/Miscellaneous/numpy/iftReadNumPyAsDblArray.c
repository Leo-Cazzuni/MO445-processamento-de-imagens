#include "ift.h"




int main(int argc, const char *argv[]) {
    if (argc != 2)
        iftError("%s <numpy_file.npy>", "main", argv[0]);

    puts("- Reading numpy array");
    iftDblArray *arr = iftReadDblArray(argv[1]);

    printf("[");
    for (size_t i = 0; i < (arr->n-1); i++)
        printf("%.2lf, ", arr->val[i]);
    printf("%.2lf]\n", arr->val[arr->n-1]);

    puts("- Saving the numpy array: 1D_float64.npy");
    iftWriteDblArray(arr, "1D_float64.npy");

    iftDestroyDblArray(&arr);

    return 0;
}


