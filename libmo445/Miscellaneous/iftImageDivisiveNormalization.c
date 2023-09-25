#include "ift.h"

int main(int argc, char *argv[])
{
    timer *tstart = iftTic();

    if (argc != 4)
    {
        iftError("\nUsage: iftImageDivisiveNormalization <...>\n"
                 "[1] input_image: Input image\n"
                 "[2] radius: Radius for the adjacency\n",
                 "[3] min_val_norm: Minimum value for normalization\n"
                 "[4] max_val_norm: Maximum value for normalization\n"
                 "[5] output_image: Output image\n"
                 "iftImageDivisiveNormalization.c");
    }

    iftImage *img = iftReadImageByExt(argv[1]);
    iftAdjRel *A = iftCircular(atof(argv[2]));
    int minValNorm = atoi(argv[3]);
    int maxValNorm = atoi(argv[4]);

    printf("-> Performing divisive normalization ... "); fflush(stdout);
    iftImage *normImg = iftImageDivisiveNormalization(img, A, minValNorm, maxValNorm);
    iftWriteImageByExt(normImg, argv[5]);

    printf("Done\n"); fflush(stdout);
    puts(iftFormattedTime(iftCompTime(tstart, iftToc())));

    iftDestroyImage(&img);
    iftDestroyImage(&normImg);
    iftDestroyAdjRel(&A);

    return (0);
}
