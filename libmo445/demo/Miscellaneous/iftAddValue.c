#include "ift.h"

int main(int argc, char **argv) {
    if (argc != 4)
        iftError("Please provide the following parameters:\n<input image> <output image> <value>\n\n", "main");

    char *filenameIn    = argv[1];
    char *filenameOut   = argv[2];
    int value           = atoi(argv[3]);

    iftImage* img = iftReadImageByExt(filenameIn);

    iftImage *imgout = iftAddValue(img, value);

    iftWriteImageByExt(imgout,filenameOut);

    iftDestroyImage(&img);
    iftDestroyImage(&imgout);

    return 0;
}
