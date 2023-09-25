#include "ift.h"

int main(int argc, char *argv[])
{
    if (argc != 10) {
        iftError("iftDrawSeeds <image> <seeds> <adjrel> <Red Obj[0, 1]> <Green Obj[0, 1]> <Blue Obj[0, 1]> "
                "<Red Bkg[0, 1]> <Green Blkg[0, 1]> <Blue Bkg[0, 1]>", "iftDrawSeeds");
    }
    const char *img_path = argv[1];
    const char *seeds_path = argv[2];
    float radius = atof(argv[3]);

    iftImage *img = iftReadImageByExt(img_path);
    iftLabeledSet *seeds = iftReadSeeds(img, seeds_path);

    int norm = iftNormalizationValue(iftMaximumValue(img));
    iftColor obj, bkg;

    obj.val[0] = norm * atof(argv[4]);
    obj.val[1] = norm * atof(argv[5]);
    obj.val[2] = norm * atof(argv[6]);
    obj = iftRGBtoYCbCr(obj, norm);

    bkg.val[0] = norm * atof(argv[7]);
    bkg.val[1] = norm * atof(argv[8]);
    bkg.val[2] = norm * atof(argv[9]);
    bkg = iftRGBtoYCbCr(bkg, norm);

    iftAdjRel *A = iftCircular(radius);

    if (!iftIsColorImage(img))
    {
        img->Cb = iftAlloc(img->n, sizeof *img->Cb);
        img->Cr = iftAlloc(img->n, sizeof *img->Cr);
        for (int i = 0; i < img->n; i++) {
            img->Cb[i] = 127;
            img->Cr[i] = 127;
        }
    }

    for (iftLabeledSet *S = seeds; S != NULL; S = S->next) {
        iftVoxel u = iftGetVoxelCoord(img, S->elem);
        for (int i = 0; i < A->n; i++) {
            iftVoxel v = iftGetAdjacentVoxel(A, u, i);
            int p = iftGetVoxelIndex(img, v);
            if (iftValidVoxel(img, v)) {
                if (S->label == 0) {
                    img->val[p] = bkg.val[0];
                    img->Cb[p]  = bkg.val[1];
                    img->Cr[p]  = bkg.val[2];
                } else {
                    img->val[p] = obj.val[0];
                    img->Cb[p]  = obj.val[1];
                    img->Cr[p]  = obj.val[2];
                }
            }
        }
    }

    iftWriteImageByExt(img, img_path);

    iftDestroyAdjRel(&A);
    iftDestroyImage(&img);
    iftDestroyLabeledSet(&seeds);

}
