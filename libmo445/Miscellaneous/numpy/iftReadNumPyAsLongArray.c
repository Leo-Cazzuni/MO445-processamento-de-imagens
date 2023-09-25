#include "ift.h"




int main(int argc, const char *argv[]) {
    if (argc != 2)
        iftError("%s <numpy_file.npy>", "main", argv[0]);;

    char *npy_path = iftCopyString(argv[1]);

    puts("- Reading numpy array");
    iftLongArray *arr = iftReadLongArray(npy_path);

    printf("[");
    for (size_t i = 0; i < (arr->n-1); i++)
        printf("%ld, ", arr->val[i]);
    printf("%ld]\n", arr->val[arr->n-1]);

    puts("- Saving the numpy array: 1D_uint64.npy");
    iftWriteLongArray(arr, "1D_uint64.npy");

    iftDestroyLongArray(&arr);
    iftFree(npy_path);

    return 0;
}


