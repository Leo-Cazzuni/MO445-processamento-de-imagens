/**
 * @author Samuel Martins
 * @date Jul 11, 2016
 */

#include "ift.h"

int main(int argc, const char *argv[]) {
    if (argc != 4)
        iftError("iftIntersectImages <img1.[png,ppm,pgm,scn]> <img2.[png,ppm,pgm,scn]> <out_img.[png,ppm,pgm,scn]>", "main");

    const char *img1_path    = argv[1];
    const char *img2_path    = argv[2];
    const char *out_img_path = argv[3];

    timer *t1 = iftTic();

    puts("- Reading Image 1");
    iftImage *img1 = iftReadImageByExt(img1_path);

    puts("- Reading Image 2");
    iftImage *img2 = iftReadImageByExt(img2_path);

    if (!iftIsDomainEqual(img1, img2))
        iftError("Input Images with Different Domains", "main");

    puts("- Intersecting Images");
    iftImage *out_img = iftCreateImage(img1->xsize, img1->ysize, img1->zsize);
    iftCopyVoxelSize(img1, out_img);

    for (int p = 0; p < out_img->n; p++) {
        if ((img1->val[p] != 0) && (img2->val[p] != 0)) {
            out_img->val[p] = img1->val[p];
        }
    }

    puts("- Writing Resulting Image");
    iftWriteImageByExt(out_img, out_img_path);

    // DESTROYERS
    iftDestroyImage(&img1);
    iftDestroyImage(&img2);
    iftDestroyImage(&out_img);


    puts("\nDone...");
    puts(iftFormattedTime(iftCompTime(t1, iftToc())));

    return 0;
}







