
#include "unitytest.h"

void testMinLabelsBoundingBox() {
    int val[35] = {0, 0, 0, 0, 0, 8, 0,
                   0, 5, 5, 5, 0, 0, 0,
                   0, 5, 5, 5, 8, 0, 8,
                   0, 5, 5, 5, 0, 8, 0,
                   0, 0, 0, 0, 0, 8, 0};
    iftImage *img = iftCreateImage(7, 5, 1);
    iftFree(img->val);
    img->val = val;

    iftIntArray *labels = iftCreateIntArray(2);
    labels->val[0] = 5;
    labels->val[1] = 8;

    iftBoundingBox true_mbbs[2] = { 
        {.begin={.x=1, .y=1, .z=0}, .end={.x=3, .y=3, .z=0}},
        {.begin={.x=4, .y=0, .z=0}, .end={.x=6, .y=4, .z=0}}
    };
    iftVoxel true_gcs[2] = {
        {.x=2, .y=2, .z=0},
        {.x=5, .y=2, .z=0}
    };

    iftVoxel *gcs;
    iftBoundingBox *mbbs = iftMinLabelsBoundingBox(img, labels, &gcs);

    for (int o = 0; o < labels->n; o++) {
        bool test1 = (iftGetVoxelIndex(img, mbbs[o].begin) == iftGetVoxelIndex(img, true_mbbs[o].begin)) &&
                     (iftGetVoxelIndex(img, mbbs[o].end) == iftGetVoxelIndex(img, true_mbbs[o].end));
        LETS_ASSERT("Not Match: Minimum Bounding Box", test1);

        bool test2 = iftGetVoxelIndex(img, gcs[o]) == iftGetVoxelIndex(img, true_gcs[o]);
        LETS_ASSERT("Not Match: Geometric Center", test2);
    }


    img->val = NULL;
    iftDestroyImage(&img);
    iftFree(gcs);
    iftFree(mbbs);
    iftDestroyIntArray(&labels);
}



int main(int argc, const char *argv[]) {
    letsInitTestSuit();

    LETS_TEST(testMinLabelsBoundingBox);

    letsPrintTestSuitStats(__FILE__);

    return 0;
}


