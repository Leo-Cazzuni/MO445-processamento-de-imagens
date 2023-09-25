#include "ift.h"

int main(int argc, char *argv[])
{
    timer *tstart = iftTic();

    if (argc != 6)
    {
        iftError("\nUsage: iftFilesetDivisiveNormalization <...>\n"
                 "[1] input_images: Input image set (CSV/Folder)\n"
                 "[2] radius: Radius for the adjacency\n"
                 "[3] min_val_norm: Minimum value for normalization\n"
                 "[4] max_val_norm: Maximum value for normalization\n"
                 "[5] output_folder: Folder to save the normalized images\n",
                 "iftFilesetDivisiveNormalization.c");
    }

    iftFileSet *fileset = iftLoadFileSetFromDirOrCSV(argv[1], 1, true);
    iftAdjRel *A = iftCircular(atof(argv[2]));
    int minValNorm = atoi(argv[3]);
    int maxValNorm = atoi(argv[4]);
    char *outputFolder = iftCopyString(argv[5]);

    if(!iftDirExists(outputFolder))
        iftMakeDir(outputFolder);

    for(int f = 0; f < fileset->n; f++)
    {
        printf("Processing file: %s ... ", fileset->files[f]->path); fflush(stdout);
        iftImage *img = iftReadImageByExt(fileset->files[f]->path);

        iftImage *normImg = iftImageDivisiveNormalization(img, A, minValNorm, maxValNorm);
        char *basename = iftFilename(fileset->files[f]->path, NULL);
        char *filename = iftJoinPathnames(2, outputFolder, basename);
        iftWriteImageByExt(normImg, filename);
        printf("OK\n"); fflush(stdout);

        iftDestroyImage(&img);
        iftDestroyImage(&normImg);
        iftFree(basename);
        iftFree(filename);
    }
    printf("\n"); fflush(stdout);
    puts(iftFormattedTime(iftCompTime(tstart, iftToc())));

    iftDestroyFileSet(&fileset);
    iftDestroyAdjRel(&A);

    return (0);
}
