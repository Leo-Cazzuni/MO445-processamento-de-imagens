#include "ift.h"




int main(int argc, const char *argv[]) {
    if (argc != 4)
        iftError("iftSetLabels <input_label_img> <old to new label: ex: 1:2> <out_label_img>", "main");

    iftImage *img = iftReadImageByExt(argv[1]);
    
    int old_label = atoi(iftSplitStringAt(argv[2], ":", 0));
    int new_label = atoi(iftSplitStringAt(argv[2], ":", 1));

    iftImage *out = iftCopyImage(img);

    for (int p = 0; p < out->n; p++) {
        if (out->val[p] == old_label)
            out->val[p] = new_label;
    }

    iftWriteImageByExt(out, argv[3]);

    iftDestroyImage(&img);
    iftDestroyImage(&out);

    return 0;
}
