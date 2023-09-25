//
// Created by azael on 10/11/17.
//

#include "ift.h"

int main(int argc, char *argv[]){

    if (argc != 5){
        iftError("iftWindowsAndLevel <input image> <width> <level> <output image>","iftWindowsAndLevel");
    }

    iftImage *img = iftReadImageByExt(argv[1]);
    int width = atoi(argv[2]);
    int level = atoi(argv[3]);

    iftImage *out = iftWindowAndLevel(img,width,level,iftImageDepth(img)-1);
    iftDestroyImage(&img);

    iftWriteImageByExt(out,argv[4]);
    iftDestroyImage(&out);

    return 0;
}