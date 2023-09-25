#include "ift.h"



int main(int argc, const char *argv[]) {
    if (argc != 4)
        iftError("iftGenerateColorTable <color_pallete> <n_colors> <out_img.[png,ppm]>\n\n" \
                 "Available Color Palettes:\n" \
                 "random\ngray\niron\nhot\nrainbow\nreverse_rainbow\nheatmap\nredhot\ngreenhot\nbluehot\nredyellowhot\n" \
                 "viridis\nplasma\nredtoblue\ncategory10\ncategory21\ncategorical", "main");
    
    char *color_pallete = iftCopyString(argv[1]);
    int n_colors        = atoi(argv[2]);
    const char *out_img = argv[3];

    iftRandomSeed(time(NULL));

    iftColorTable *ctb = NULL;

    if (iftCompareStrings(color_pallete, "random"))
        ctb = iftCreateColorTable(n_colors);
    else if (iftCompareStrings(color_pallete, "gray"))
        ctb = iftGrayColorTable(n_colors);
    else if (iftCompareStrings(color_pallete, "iron"))
        ctb = iftIronColorTable(n_colors);
    else if (iftCompareStrings(color_pallete, "hot"))
        ctb = iftHotColorTable(n_colors);
    else if (iftCompareStrings(color_pallete, "rainbow"))
        ctb = iftRainbowColorTable(n_colors);
    else if (iftCompareStrings(color_pallete, "reverse_rainbow"))
        ctb = iftReverseRainbowColorTable(n_colors);
    else if (iftCompareStrings(color_pallete, "heatmap"))
        ctb = iftHeatMapColorTable(n_colors);
    else if (iftCompareStrings(color_pallete, "redhot"))
        ctb = iftRedHotColorTable(n_colors);
    else if (iftCompareStrings(color_pallete, "greenhot"))
        ctb = iftGreenHotColorTable(n_colors);
    else if (iftCompareStrings(color_pallete, "bluehot"))
        ctb = iftBlueHotColorTable(n_colors);
    else if (iftCompareStrings(color_pallete, "redyellowhot"))
        ctb = iftRedYellowHotColorTable(n_colors);
    else if (iftCompareStrings(color_pallete, "redtoblue"))
        ctb = iftRedToBlueColorTable(n_colors);
    else if (iftCompareStrings(color_pallete, "viridis"))
        ctb = iftViridisColorTable(n_colors);
    else if (iftCompareStrings(color_pallete, "plasma"))
        ctb = iftPlasmaColorTable(n_colors);
    else if (iftCompareStrings(color_pallete, "category10"))
        ctb = iftCategory10ColorTable(n_colors);
    else if (iftCompareStrings(color_pallete, "category21"))
        ctb = iftCategory21ColorTable(n_colors);
    else if (iftCompareStrings(color_pallete, "categorical"))
        ctb = iftCategoricalColorTable(n_colors);
    else iftError("Invalid color pallete: %s", "main", color_pallete);

    // squares of 30x30
    iftImage *cimg = iftCreateColorImage(n_colors*30, 30, 1, 8);

    printf("- Generating the image \"%s\" with %d colored squares of 30x30\n", out_img, n_colors);
    iftVoxel u = {0, 0, 0};
    for (u.y = 0; u.y < cimg->ysize; u.y++) {
        for (u.x = 0; u.x < cimg->xsize; u.x++) {
            int c = u.x / 30;

            iftImgVoxelVal(cimg, u) = ctb->color[c].val[0];
            iftImgVoxelCb(cimg, u) = ctb->color[c].val[1];
            iftImgVoxelCr(cimg, u) = ctb->color[c].val[2];
        }
    }

    iftWriteImageByExt(cimg, out_img);

    iftDestroyImage(&cimg);
    iftDestroyColorTable(&ctb);

    return 0;
}