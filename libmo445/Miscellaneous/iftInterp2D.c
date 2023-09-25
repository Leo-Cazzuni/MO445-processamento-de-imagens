#include "ift.h"


int main(int argc, const char *argv[]) {
    if (argc != 6)
        iftError("iftInterp <input_img> <VOXEL | DOMAIN> <out_dx | out_xsize> <out_dy | out_ysize> "
                 "<output_img>\n\n"
                 "Interpolate an image passing either the output voxel sizes (VOXEL) or the image domain (DOMAIN).\n"
                 "Ex: iftInterp2D imagepng VOXEL 1.25 1.25 output.png\n"
                 "Ex: iftInterp2D image.png DOMAIN 600 400 output.png",
                 "main");

    char *img_path = iftCopyString(argv[1]);
    char *option = iftCopyString(argv[2]);
    if (!iftCompareStrings(option, "VOXEL") && !iftCompareStrings(option, "DOMAIN"))
        iftError("Invalid option for Interpolation: %s\nTry VOXEL or DOMAIN", "main");

    char *out_img_path = iftCopyString(argv[5]);
    char *parent_dir = iftParentDir(out_img_path);
    if (!iftDirExists(parent_dir))
        iftMakeDir(parent_dir);
    iftFree(parent_dir);


    printf("- Reading Image: %s\n", img_path);
    iftImage *img = iftReadImageByExt(img_path);

    float sx, sy;
    if (iftCompareStrings(option, "VOXEL")) {
        float out_dx = atof(argv[3]);
        float out_dy = atof(argv[4]);

        sx = img->dx / out_dx;
        sy = img->dy / out_dy;
    }
    else {
        float out_xsize = atoi(argv[3]);
        float out_ysize = atoi(argv[4]);

        sx = out_xsize / img->xsize;
        sy = out_ysize / img->ysize;
    }

    printf("- Interpolating: factors: (%f, %f)\n", sx, sy);
    iftImage *interp_img = iftInterp2D(img, sx, sy);

    printf("- Writing Interpolated Image: %s\n", out_img_path);
    iftWriteImageByExt(interp_img, out_img_path);

    iftFree(img_path);
    iftFree(option);
    iftFree(out_img_path);
    iftDestroyImage(&img);
    iftDestroyImage(&interp_img);

    return 0;
}


