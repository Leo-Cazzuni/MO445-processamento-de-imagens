//
// Created by azaelmsousa on 06/04/20.
//

#include "ift.h"

int main(int argc, const char *argv[]) {
    if (argc != 6)
        iftError("iftMatchHistogram <input_img> <label_img> <reference_img> <reference_label> <out_img>", "main");

    timer *t1 = iftTic();

    iftImage *img = iftReadImageByExt(argv[1]);
    iftImage *mask = iftReadImageByExt(argv[2]);
    iftImage *ref = iftReadImageByExt(argv[3]);
    iftImage *ref_mask = iftReadImageByExt(argv[4]);

    printf("- Matching Histogram on mask:\n%s\nwith\n%s\non\n%s\n\n", argv[1], argv[3], argv[2]);

    iftImage *out = iftMatchHistogram(img, mask, ref, ref_mask);
    iftWriteImageByExt(out, argv[5]);

    puts("\nDone...");
    puts(iftFormattedTime(iftCompTime(t1, iftToc())));

    iftDestroyImage(&img);
    iftDestroyImage(&mask);
    iftDestroyImage(&ref);
    iftDestroyImage(&out);

    return 0;
}
