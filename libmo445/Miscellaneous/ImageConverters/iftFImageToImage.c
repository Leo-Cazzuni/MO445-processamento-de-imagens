#include "ift.h"



int main(int argc, const char *argv[]) {
    if (argc != 4)
        iftError("iftFImageToImage <fimage.[npy, fscn]> <max_img_range (ex: 255, 4095, ...)> <out_img>", "main");

    iftFImage *fimg = iftReadFImage(argv[1]);
    int max_range   = atoi(argv[2]);

    iftImage *img = iftFImageToImage(fimg, max_range);
    iftWriteImageByExt(img, argv[3]);

    iftDestroyFImage(&fimg);

    return 0;
}














