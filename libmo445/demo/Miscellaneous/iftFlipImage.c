#include "ift.h"



int main(int argc, const char *argv[]) {
    if (argc != 4)
        iftError("iftFlipImage <input_image> <axis: [X, Y, Z] <output_image>", "main");

    iftImage *img = iftReadImageByExt(argv[1]);

    
    iftImage *flip_img = NULL;

    char *axis = iftCopyString(argv[2]);
    if (iftCompareStrings(axis, "X")) {
        puts("- Flipping on X");
        flip_img = iftFlipImage(img, IFT_AXIS_X);
    }
    else if (iftCompareStrings(axis, "Y")) {
        puts("- Flipping on Y");
        flip_img = iftFlipImage(img, IFT_AXIS_Y);
    }
    else if (iftCompareStrings(axis, "Z")) {
        puts("- Flipping on Z");
        flip_img = iftFlipImage(img, IFT_AXIS_Z);
    }
    else iftError("Invalid axis: %s\nTry X, Y or Z", axis, "main");

    iftWriteImageByExt(flip_img, argv[3]);

    return 0;
}









