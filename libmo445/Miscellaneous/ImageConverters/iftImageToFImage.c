#include "ift.h"



int main(int argc, const char *argv[]) {
    if (argc != 4)
        iftError("iftImageToFImage <image.scn> <max_float_range (ex: 1.0, ...)> <out_fimg.[npy, fscn]>", "main");

    iftImage *img   = iftReadImage(argv[1]);
    float max_range = atof(argv[2]);

    iftFImage *fimg = iftImageToFImageMaxVal(img, max_range);
    iftWriteFImage(fimg, argv[3]);

    iftDestroyImage(&img);

    return 0;
}














