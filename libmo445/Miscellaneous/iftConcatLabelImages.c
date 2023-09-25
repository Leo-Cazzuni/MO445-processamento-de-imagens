#include "ift.h"



int main(int argc, const char *argv[]) {
    if (argc != 4)
        iftError("iftConcatLabelImages <label_img1> <label_img2> <out_label_img>\n" \
                 "Concat the the label_img2 to label_img1", "main");

    iftImage *label_img1 = iftReadImageByExt(argv[1]);
    iftImage *label_img2 = iftReadImageByExt(argv[2]);

    iftImage *concat = iftCopyImage(label_img1);
    for (int p = 0; p < label_img2->n; p++)
        if (label_img2->val[p])
            concat->val[p] = label_img2->val[p];

    iftWriteImageByExt(concat, argv[3]);

    return 0;
}



