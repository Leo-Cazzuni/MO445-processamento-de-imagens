#include "ift.h"




int main(int argc, const char *argv[]) {
    if (argc != 4)
        iftError("iftMergeMarkers <ref_image.[png,ppm,pgm,scn]> <markers1.txt:markers2.txt:...> <out_markers.txt>", "main");

    iftImage *ref_img     = iftReadImageByExt(argv[1]);
    iftSList *marker_list = iftSplitString(argv[2], ":");

    iftLabeledSet *merged = NULL;

    iftSNode *snode = marker_list->head;
    while (snode != NULL) {
        iftLabeledSet *marker = iftReadSeeds(ref_img,snode->elem);
        iftConcatLabeledSet(&merged, &marker);
        snode = snode->next;
    }


    iftWriteSeeds(merged, ref_img, argv[3]);

    iftDestroySList(&marker_list);

    return 0;
}