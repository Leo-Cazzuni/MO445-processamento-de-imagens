//
// Created by azaelmsousa on 09/05/20.
//

#include "ift.h"

iftIntArray *iftSListToIntArray(iftSList *SL)
{
    iftIntArray *arr = iftCreateIntArray(SL->n);

    iftSNode *node = SL->head;
    int i = 0;
    while (node != NULL) {
        arr->val[i++] = atoi(node->elem);
        node = node->next;
    }

    return arr;
}

int main(int argc, const char *argv[])
{
    if (argc != 5)
        iftError("Usage: iftCropObject <input image> <label image> <objects, e.g. \"1,2,3\"> <output image>","main");

    iftImage *img = iftReadImageByExt(argv[1]);
    iftImage *label = iftReadImageByExt(argv[2]);

    iftSList *sl = iftSplitString(argv[3], ",");
    iftIntArray *arr = iftSListToIntArray(sl);
    iftDestroySList(&sl);
    iftImage *aux = iftCreateImageFromImage(label);
    for (int i = 0; i < arr->n; i++) {
        iftImage *obj_label = iftExtractObject(label, arr->val[i]);
        iftAddInPlace(obj_label, aux);
        iftDestroyImage(&obj_label);
    }
    iftDestroyIntArray(&arr);
    iftDestroyImage(&label);
    label = aux;
    aux = NULL;

    iftVoxel v;
    iftBoundingBox bb = iftMinBoundingBox(label,&v);
    iftDestroyImage(&label);

    iftImage *out = iftExtractROI(img,bb);
    iftDestroyImage(&img);

    iftWriteImageByExt(out,argv[4]);
    iftDestroyImage(&out);

    return 0;
}