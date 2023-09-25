#include "ift.h"


int main(int argc, const char* argv[]) {
    if (argc != 3)
        iftError("%s <label_image> <out_relabeled_image>", "main", argv[0]);
    
    iftImage* label_img = iftReadImageByExt(argv[1]);
    iftAdjRel *A = (iftIs3DImage(label_img)) ? iftSpheric(sqrt(3.0)) : iftCircular(sqrt(2.0));

    iftImage *bin_img = iftBinarize(label_img);
    iftWriteImageByExt(bin_img, "bin_img.nii.gz");
    iftImage *relabeled_img = iftFastLabelComp(bin_img, A);
    printf("Number of New objects: %d\n", iftMaximumValue(relabeled_img));

    iftWriteImageByExt(relabeled_img, argv[2]);

    
    return 0;
}

