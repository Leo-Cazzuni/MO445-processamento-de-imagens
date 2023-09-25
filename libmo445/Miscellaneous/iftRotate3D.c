#include "ift.h"


int main(int argc, const char *argv[]) {
    if (argc != 5)
        iftError("%s <input_image> <tilt [0-180]> <spin [0-180] <output_rotated_image>", "main", argv[0]);
    
    iftImage *img = iftReadImageByExt(argv[1]);
    iftImage *res = iftRotateImage(img, atof(argv[2]), atof(argv[3]));
    iftWriteImageByExt(res, argv[4]);
    
    iftDestroyImage(&img);
    iftDestroyImage(&res);
    
    return 0;
}




