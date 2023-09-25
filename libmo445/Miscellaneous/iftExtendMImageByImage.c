//
// Created by azaelmsousa on 26/09/20.
//

#include "ift.h"

int main(int argc, char *argv[])
{
    if (argc != 4)
        iftError("Usage: iftExtendMImageByImage <input mimg> <img to concatenate> <output mimg>","main");

    iftMImage *mimg = iftReadMImage(argv[1]);
    iftImage *img = iftReadImageByExt(argv[2]);

    iftMImage *out = iftExtendMImageByImage(mimg,img);
    iftDestroyImage(&img);
    iftDestroyMImage(&mimg);

    iftWriteMImage(out,argv[3]);
    iftDestroyMImage(&out);

    return 0;
}