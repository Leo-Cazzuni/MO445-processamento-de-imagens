#include "ift.h"




int main(int argc, const char *argv[]) {
    if (argc != 5)
        iftError("iftExtractObject <label_image> <object> <binarize_output: 0 (no) | 1 (yes)> <out_img>", "main");


    iftImage *label_img = iftReadImageByExt(argv[1]);
    int obj_label = atoi(argv[2]);

    iftImage *obj_img = iftExtractObject(label_img, obj_label);

    int binarize = atoi(argv[3]);

    if (binarize)
        iftWriteImageByExt(iftBinarize(obj_img), argv[4]);
    else iftWriteImageByExt(obj_img, argv[4]);

    iftDestroyImage(&label_img);
    iftDestroyImage(&obj_img);

    return 0;
}



