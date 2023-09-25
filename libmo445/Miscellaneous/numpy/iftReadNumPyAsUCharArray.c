#include "ift.h"




int main(int argc, const char *argv[]) {
    if (argc != 2)
        iftError("%s <numpy_file.npy>", "main", argv[0]);
    
    char *npy_path = iftCopyString(argv[1]);
    
    puts("- Reading numpy array");
    iftUCharArray *arr = iftReadUCharArray(npy_path);
    
    printf("[");
    for (size_t i = 0; i < (arr->n-1); i++)
        printf("%d, ", arr->val[i]);
    printf("%d]\n", arr->val[arr->n-1]);
    
    puts("\n- Saving the numpy array: 1D_uint8.npy");
    iftWriteUCharArray(arr, "1D_uint8.npy");
    
    iftDestroyUCharArray(&arr);
    iftFree(npy_path);
    
    return 0;
}


