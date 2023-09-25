//
// Created by azaelmsousa on 13/07/20.
//

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

    if (argc != 5)

        iftError("iftCleanImageBackgroundOnMask <orig_image> <label_image> <objects: (e.g: 1,5,10)> <out_img>\n\n" \
                 "Returns an image with the original intensity only on the spels that belong to the label image", "main");

    iftImage *orig_img = iftReadImageByExt(argv[1]);
    iftImage *label_img = iftReadImageByExt(argv[2]);
    iftIntArray *labels = _iftGetLabelsFromStringInput(argv[3]);
    iftImage *extracted_labels = iftExtractLabels(label_img, labels);
    iftDestroyImage(&label_img);

    iftImage *output = iftCreateImageFromImage(orig_img);
    for (int i = 0; i < output->n; i++){
        if (extracted_labels->val[i] > 0)
            output->val[i] = orig_img->val[i];
    }
    iftDestroyImage(&orig_img);
    iftDestroyImage(&extracted_labels);

    iftWriteImageByExt(output,argv[4]);
    iftDestroyImage(&output);

    return 0;
}
