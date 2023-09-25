#include "ift.h"




int main(int argc, const char *argv[]) {
    if (argc != 3)
        iftError("iftBinarize Image <input> <output>", "main");

    puts("Binarizing");
    iftImage *img = iftReadImageByExt(argv[1]);
    iftImage *out = iftBinarize(img);
    iftWriteImageByExt(out, argv[2]);
    puts("Done...");


    iftDestroyImage(&img);
    iftDestroyImage(&out);


    return 0;
}















