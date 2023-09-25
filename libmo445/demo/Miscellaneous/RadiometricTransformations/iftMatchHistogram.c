#include "ift.h"




int main(int argc, const char *argv[]) {
    if (argc != 4)
        iftError("iftMatchHistogram <input_img> <reference_img> <out_img>", "main");


    timer *t1 = iftTic();

    iftImage *img = iftReadImageByExt(argv[1]);
    iftImage *ref = iftReadImageByExt(argv[2]);
    printf("- Matching Histogram:\n%s\nwith\n%s\n\n", argv[1], argv[2]);

    iftImage *out = iftMatchHistogram(img, NULL, ref, NULL);
    iftWriteImageByExt(out, argv[3]);

    puts("\nDone...");
    puts(iftFormattedTime(iftCompTime(t1, iftToc())));

    iftDestroyImage(&img);
    iftDestroyImage(&ref);
    iftDestroyImage(&out);

    return 0;
}
