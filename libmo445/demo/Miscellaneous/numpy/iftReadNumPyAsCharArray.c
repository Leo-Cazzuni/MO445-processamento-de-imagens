#include "ift.h"




int main(int argc, const char *argv[]) {
       if (argc != 2)
       iftError("%s <numpy_file.npy>", "main", argv[0]);

   char *npy_path = iftCopyString(argv[1]);

   puts("- Reading numpy array");
   iftCharArray *arr = iftReadCharArray(npy_path);

   printf("[");
   for (size_t i = 0; i < (arr->n-1); i++)
       printf("%d, ", arr->val[i]);
   printf("%d]\n", arr->val[arr->n-1]);

   puts("\n- Saving the numpy array: 1D_int8.npy");
   iftWriteCharArray(arr, "1D_int8.npy");

   iftDestroyCharArray(&arr);
   iftFree(npy_path);

    return 0;
}


