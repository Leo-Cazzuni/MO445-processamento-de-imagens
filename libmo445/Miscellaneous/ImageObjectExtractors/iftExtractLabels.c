#include "ift.h"


iftIntArray *_iftGetLabelsFromStringInput(const char *labels_input) {
    iftSList *SL = iftSplitString(labels_input, ",");

    iftIntArray *labels = iftCreateIntArray(SL->n);

    for (int i = 0; i < labels->n; i++)
        labels->val[i] = atoi(iftRemoveSListHead(SL));

    iftDestroySList(&SL);

    return labels;
}



int main(int argc, const char *argv[]) {
    if (argc != 4)
        iftError("iftExtractLabels <label_image> <objects: (e.g: 1,5,10)> <out_img>\n\n" \
                 "Returns a label image with only a given objects from an input label image", "main");

    iftImage *label_img = iftReadImageByExt(argv[1]);
    iftIntArray *labels = _iftGetLabelsFromStringInput(argv[2]);

    iftPrintIntArray(labels->val, labels->n);
    iftImage *out_label_img = iftExtractLabels(label_img, labels);

    iftWriteImageByExt(out_label_img, argv[3]);

    iftDestroyImage(&label_img);
    iftDestroyImage(&out_label_img);
    iftDestroyIntArray(&labels);

    return 0;
}



