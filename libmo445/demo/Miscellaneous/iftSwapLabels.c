#include "ift.h"




int main(int argc, const char *argv[]) {
    if (argc != 4)
        iftError("iftSwapLabels <input_label_img> <labels to swap: ex: 1:2> <out_img>", "main");

    iftImage *img       = iftReadImageByExt(argv[1]);
    iftIntArray *labels = iftCreateIntArray(iftMaximumValue(img)+1);
    for (int l = 0; l < labels->n; l++)
        labels->val[l] = l;

    int old_label = atoi(iftSplitStringAt(argv[2], ":", 0));
    int new_label = atoi(iftSplitStringAt(argv[2], ":", 1));

    printf("old_label: %d\nnew_label: %d\n\n", old_label, new_label);
    labels->val[old_label] = new_label;
    labels->val[new_label] = old_label;

    iftImage *out = iftCopyImage(img);

    for (int p = 0; p < out->n; p++) {
        out->val[p] = labels->val[out->val[p]];
    }

    iftWriteImageByExt(out, argv[3]);

    iftDestroyImage(&img);
    iftDestroyImage(&out);
    iftDestroyIntArray(&labels);

    return 0;
}