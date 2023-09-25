/**
 * @brief This program computes the Dice Similarity between two Multi Label Images.
 * If the number of objects is not passed, this one will be the maximum voxel value from the first image.
 * @author Samuel Martins
 * @date Feb 19, 2015
 */

#include "ift.h"


/************************** HEADERS **************************/
iftDict *iftGetArguments(int argc, const char *argv[]);
void iftValidateRequiredInputs(const char *label_img_pathname, const char *gt_pathname);
/*************************************************************/


int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArguments(argc, argv);

    const char *label_img_pathname = iftGetStrValFromDict("--label-img", args);
    const char *gt_pathname        = iftGetStrValFromDict("--ground-truth", args);
    //iftValidateRequiredInputs(label_img_pathname, gt_pathname);
    
    iftImage *label_img = iftReadImageByExt(label_img_pathname);
    iftImage *gt        = iftReadImageByExt(gt_pathname);

    // Optional arguments
    int n_objects = 0;
    if (iftDictContainKey("--num-objects", args, NULL))
        n_objects = iftGetLongValFromDict("--num-objects", args);
    if (n_objects <= 0)
        n_objects = iftMaximumValue(label_img);

    timer *t1 = iftTic();
    puts("----------------------------------------");
    printf("Label Image: \"%s\"\n", label_img_pathname);
    printf("Ground Truth: \"%s\"\n", gt_pathname);
    printf("Number of Objects: %d\n", n_objects);
    puts("----------------------------------------\n");
    
    iftDblArray *dices = iftDiceSimilarityMultiLabel(label_img, gt, n_objects);

    // Binary Images
    if (n_objects == 1)
        printf("Dice: %lf\n", dices->val[0]);
    // Label Images
    else {
        printf("Dices:\nAverage: %lf\n", dices->val[0]);
        for (int i = 1; i < dices->n; i++)
            printf("Label %d = %lf\n", i, dices->val[i]);    
    }

    puts("");
    timer *t2 = iftToc();
    iftPrintFormattedTime(iftCompTime(t1, t2));

    // DESTROYERS
    iftDestroyDblArray(&dices);
    iftDestroyImage(&label_img);
    iftDestroyImage(&gt);
    iftDestroyDict(&args);

    return 0;
}



/************************** SOURCES **************************/
iftDict *iftGetArguments(int argc, const char *argv[]) {
    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--label-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Label Image A"},
        {.short_name = "-g", .long_name = "--ground-truth", .has_arg=true,  .arg_type=IFT_STR_TYPE,
         .required=true, .help="Label Image B. Commonly the Ground Truth."},
        {.short_name = "-n", .long_name = "--num-objects", .has_arg=true,  .arg_type=IFT_LONG_TYPE,
         .required=false, .help="Number of Objects of the input labeled images (except the background)."}
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);
    

    char program_description[2048] = \
        "This program computes the Dice Similarity between two Multi Label Images (SCN or PGM) " \
        "with labels in the range [0..num_objects].\n" \
        "If the number of objects is not passed, it will be the maximum voxel value from the first image.\n\n" \
        "* PS: This program does not checks if the images are indeed label images with the same number of objects, " \
        "whose objects have label in the range [0..num_objects].\n" \
        "To ensure that, run the program ift/demo/Miscellaneous/ImageMetrics/iftCheckLabelImages.c";
    
    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}


void iftValidateRequiredInputs(const char *label_img_pathname, const char *gt_pathname) {
    if (!iftFileExists(label_img_pathname))
        iftError("Label Image \"%s\" does not exist", "iftValidateRequiredInputs", label_img_pathname);
    if (!iftEndsWith(label_img_pathname, ".pgm") && !iftEndsWith(label_img_pathname, ".scn"))
        iftError("Label Image \"%s\" is not pgm, scn", "iftValidateRequiredInputs", label_img_pathname);

    if (!iftFileExists(gt_pathname))
        iftError("Ground Truth \"%s\" does not exist", "iftValidateRequiredInputs", gt_pathname);
    if (!iftEndsWith(gt_pathname, ".pgm") && !iftEndsWith(gt_pathname, ".scn"))
        iftError("Ground Truth \"%s\" is not pgm, scn", "iftValidateRequiredInputs", gt_pathname);

    const char *ext_label = iftFileExt(label_img_pathname);
    const char *ext_gt    = iftFileExt(gt_pathname);
    if (!iftCompareStrings(ext_label, ext_gt))
        iftError("Label Image and Ground Truth do not have the same datatype", "iftValidateRequiredInputs");
}
/*************************************************************/
