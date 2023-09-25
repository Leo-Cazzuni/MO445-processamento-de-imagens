#include "ift.h"




int main(int argc, const char *argv[]) {
    if (argc != 2)
        iftError("%s <numpy_file.npy>", "main", argv[0]);

    puts("- Reading numpy array****");
    iftFImage *fimg = iftReadFImage(argv[1]);

    for (int z = 0; z < fimg->zsize; z++) {
        for (int y = 0; y < fimg->ysize; y++) {
            printf("[");
            
            for (int x = 0; x < fimg->xsize; x++) {
                printf("%.2f ", iftImgVal(fimg, x, y, z));
            }

            puts("]");
        }
        puts("");
    }

    puts("- Saving the numpy array: fimg.npy");
    iftWriteFImage(fimg, "fimg.npy");

    iftDestroyFImage(&fimg);

    return 0;
}


