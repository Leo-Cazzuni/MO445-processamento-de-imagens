#include "ift.h"


int main(int argc, const char *argv[]) {
    if (argc != 3)
        iftError("iftExtractBorders <label_img> <output_border_image>", "main");

    iftImage *label_img = iftReadImageByExt(argv[1]);
    iftImage *border_img = iftRegionBorders(label_img);
    iftWriteImageByExt(border_img, argv[2]);

    return 0;
}


