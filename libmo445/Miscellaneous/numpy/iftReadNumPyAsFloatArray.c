#include "ift.h"




int main(int argc, const char *argv[]) {
    if (argc != 2)
        iftError("%s <numpy_file.npy>", "main", argv[0]);

    puts("- Reading numpy array");
    iftFloatArray *arr = iftReadFloatArray(argv[1]);

    printf("[");
    for (size_t i = 0; i < (arr->n-1); i++)
        printf("%.2f, ", arr->val[i]);
    printf("%.2f]\n", arr->val[arr->n-1]);

    puts("- Saving the numpy array: 1D_float32.npy");
    iftWriteFloatArray(arr, "1D_float32.npy");

    iftDestroyFloatArray(&arr);

    return 0;
}


