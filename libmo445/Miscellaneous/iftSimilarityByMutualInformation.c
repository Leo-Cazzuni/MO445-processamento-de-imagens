#include "ift.h"


int main(int argc, const char* argv[]) {
    if (argc != 3)
        iftError("%s <image_1.*> <image_2.*>", "main", argv[0]);
    
    iftImage *img1 = iftReadImageByExt(argv[1]);
    iftImage *img2 = iftReadImageByExt(argv[2]);

    printf("similarity %lf\n",iftNormalizedMutualInformation(img1,img2));

    return 0;
}

