//
// Created by azael on 25/04/19.
//

#include "ift.h"

int main(int argc, char *argv[])
{

    if (argc != 7)
        iftError("iftInterpMask <input_mask> <VOXEL | DOMAIN> <out_dx | out_xsize> <out_dy | out_ysize> " \
                 "<out_dz | out_zsize> <output_img>\n\n" \
                 "Interpolate an image passing either the output voxel sizes (VOXEL) or the image domain (DOMAIN).\n" \
                 "Ex: iftInterpMask segm.scn VOXEL 1.25 1.25 1.25 output.scn\n" \
                 "Ex: iftInterpMask segm.scn DOMAIN 600 400 200 output.scn", "main");

    char *img_path = iftCopyString(argv[1]);
    char *option = iftCopyString(argv[2]);
    if (!iftCompareStrings(option, "VOXEL") && !iftCompareStrings(option, "DOMAIN"))
        iftError("Invalid option for Interpolation: %s\nTry VOXEL or DOMAIN", "main");

    char *out_img_path = iftCopyString(argv[6]);
    char *parent_dir = iftParentDir(out_img_path);
    if (!iftDirExists(parent_dir))
        iftMakeDir(parent_dir);
    iftFree(parent_dir);


    printf("- Reading Image: %s\n", img_path);
    iftImage *img = iftReadImageByExt(img_path);

    float sx, sy, sz;
    if (iftCompareStrings(option, "VOXEL")) {
        float out_dx = atof(argv[3]);
        float out_dy = atof(argv[4]);
        float out_dz = atof(argv[5]);

        sx = img->dx / out_dx;
        sy = img->dy / out_dy;
        sz = img->dz / out_dz;
    }
    else {
        float out_xsize = atoi(argv[3]);
        float out_ysize = atoi(argv[4]);
        float out_zsize = atoi(argv[5]);

        sx = out_xsize / img->xsize;
        sy = out_ysize / img->ysize;
        sz = out_zsize / img->zsize;
    }

    printf("- Interpolating: factors: (%f, %f, %f)\n", sx, sy, sz);
    iftImage *interp_img = iftInterpByNearestNeighbor(img, sx, sy, sz);

    printf("- Writing Interpolated Image: %s\n", out_img_path);
    iftWriteImageByExt(interp_img, out_img_path);

    iftFree(img_path);
    iftFree(option);
    iftFree(out_img_path);
    iftDestroyImage(&img);
    iftDestroyImage(&interp_img);

    return 0;
}
