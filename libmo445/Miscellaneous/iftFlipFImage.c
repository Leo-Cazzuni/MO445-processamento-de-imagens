#include "ift.h"



int main(int argc, const char *argv[]) {
    if (argc != 4)
        iftError("iftFlipFImage <input_fimage.[npy, fscn]> <axis: [X, Y, Z] <output_fimage.[npy, fscn]>", "main");


    iftFImage *img = iftReadFImage(argv[1]);

    iftFImage *flip_img = NULL;

    char *axis = iftCopyString(argv[2]);
    if (iftCompareStrings(axis, "X")) {
        puts("- Flipping on X");
        flip_img = iftFlipFImage(img, IFT_AXIS_X);
    }
    else if (iftCompareStrings(axis, "Y")) {
        puts("- Flipping on Y");
        flip_img = iftFlipFImage(img, IFT_AXIS_Y);
    }
    else if (iftCompareStrings(axis, "Z")) {
        puts("- Flipping on Z");
        flip_img = iftFlipFImage(img, IFT_AXIS_Z);
    }
    else iftError("Invalid axis: %s\nTry X, Y or Z", axis, "main");

    iftWriteFImageByExt(flip_img, argv[3]);

    return 0;
}









