/**
 * @file
 * @brief Computes the Linear Combination of two Input Images according to the formula: \n
 * grad = (alpha * input_grad_img1) + ((1-alpha) * input_grad_img2).
 * @note See the source code in @ref iftLinearCombination.c
 *
 * @example iftLinearCombination.c
 * @brief Computes the Linear Combination of two Input Images according to the formula: \n
 * grad = (alpha * input_grad_img1) + ((1-alpha) * input_grad_img2).
 * @author Samuel Martins
 * @date Jul 14, 2016
 */


#include "ift.h"

/************************** HEADERS **************************/
iftDict *iftGetArgs(int argc, const char *argv[]);
void iftGetRequiredArgs(const iftDict *args, char **grad1_path, char **grad2_path, float *alpha, char **out_grad_path);
void iftValidateRequiredArgs(const char *grad1_path, const char *grad2_path, float alpha, const char *out_grad_path);
void iftValidateInputImages(const iftImage *img, const iftImage *img2);
int iftGetImageMaxRange(const iftDict *args, const iftImage *img);
/*************************************************************/



int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArgs(argc, argv);

    // mandatory args
    char *grad1_path    = NULL;
    char *grad2_path    = NULL;
    float alpha;
    char *out_grad_path = NULL;
    // optional args

    iftGetRequiredArgs(args, &grad1_path, &grad2_path, &alpha, &out_grad_path);

    timer *t1 = iftTic();

    iftImage *grad_img1    = iftReadImageByExt(grad1_path);
    iftImage *grad_img2    = iftReadImageByExt(grad2_path);
    iftValidateInputImages(grad_img1, grad_img2);

    int img_range = iftGetImageMaxRange(args, grad_img1);

    puts("- Normalizing Gradients");
    iftImage *norm_grad_img1 = iftLinearStretch(grad_img1, iftMinimumValue(grad_img1), iftMaximumValue(grad_img1),
                                                0, img_range);
    iftImage *norm_grad_img2 = iftLinearStretch(grad_img2, iftMinimumValue(grad_img2), iftMaximumValue(grad_img2),
                                                0, img_range);
    iftDestroyImage(&grad_img1);
    iftDestroyImage(&grad_img2);

    puts("- Linear Combination of Gradients");
    iftImage *out_grad_img = iftLinearCombination(norm_grad_img1, norm_grad_img2, alpha);
    iftDestroyImage(&norm_grad_img1);
    iftDestroyImage(&norm_grad_img2);

    puts("- Writing Gradient Image");
    iftWriteImageByExt(out_grad_img, out_grad_path);

    puts("\nDone...");
    puts(iftFormattedTime(iftCompTime(t1, iftToc())));


    // DESTROYERS
    iftDestroyDict(&args);
    iftDestroyImage(&out_grad_img);
    iftFree(grad1_path);
    iftFree(grad2_path);
    iftFree(out_grad_path);

    return 0;
}



/************************** SOURCES **************************/
iftDict *iftGetArgs(int argc, const char *argv[]) {
    char program_description[2048] = \
        "Computes the Linear Combination of two Input Images according to the formula:\n" \
        "grad = (alpha * input_grad_img1) + ((1-alpha) * input_grad_img2)";

    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--first-grad-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="First Gradient Image."},
        {.short_name = "-j", .long_name = "--second-grad-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Second Gradient Image."},
        {.short_name = "-a", .long_name = "--alpha", .has_arg=true, .arg_type=IFT_DBL_TYPE,
         .required=true, .help="Factor used to combine linearly the Gradients [0, 1]"},
        {.short_name = "-o", .long_name = "--output-grad-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Pathname from the Output Gradient Image."},
        {.short_name = "-b", .long_name = "--img-depth", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=false, .help="Depth from the Input Image and Object Map in bits (8, 12, 16, ...)\n" \
                                "Default: it tries to find the image depth from the First Input Image automatically"}
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}


void iftGetRequiredArgs(const iftDict *args, char **grad1_path, char **grad2_path, float *alpha, char **out_grad_path) {
    *grad1_path    = iftGetStrValFromDict("--first-grad-img", args);
    *grad2_path    = iftGetStrValFromDict("--second-grad-img", args);
    *alpha         = iftGetDblValFromDict("--alpha", args);
    *out_grad_path = iftGetStrValFromDict("--output-grad-img", args);

    iftValidateRequiredArgs(*grad1_path, *grad2_path, *alpha, *out_grad_path);

    puts("-----------------------");
    printf("- First Input Gradient: \"%s\"\n", *grad1_path);
    printf("- Second Input Gradient: \"%s\"\n", *grad2_path);
    printf("- Linear Factor (alpha): %f\n", *alpha);    
    printf("- Output Gradient: \"%s\"\n", *out_grad_path);
    puts("-----------------------");

}


void iftValidateRequiredArgs(const char *grad1_path, const char *grad2_path, float alpha, const char *out_grad_path) {
    if (!iftIsImageFile(grad1_path))
        iftError("Invalid First Input Gradient: \"%s\"", "iftValidateRequiredArgs", grad1_path);

    if (!iftIsImageFile(grad2_path))
        iftError("Invalid Second Input Gradient: \"%s\"", "iftValidateRequiredArgs", grad2_path);

    if ((alpha < 0.0) || (alpha > 1.0))
        iftError("Invalid alpha: %f... Try [0,1]\n", "iftValidateRequiredArgs", alpha);

    char *parent_dir = iftParentDir(out_grad_path);
    if (!iftDirExists(parent_dir)) {
        iftMakeDir(parent_dir);
    }
    iftFree(parent_dir);
}


void iftValidateInputImages(const iftImage *img1, const iftImage *img2) {
    iftVerifyImageDomains(img1, img2, "iftValidateInputImages");
    if (!iftIsVoxelSizeEqual(img1, img2))
        iftError("Input Images have different Voxel Sizes\n" \
                 "Image 1 (%lf, %lf, %lf)\n" \
                 "Image 2 (%lf, %lf, %lf)", "iftValidateInputImages", 
                 img1->dx, img1->dy, img1->dz, img2->dx, img2->dy, img2->dz);
}


int iftGetImageMaxRange(const iftDict *args, const iftImage *img) {
    int img_range;

    if (iftDictContainKey("--img-depth", args, NULL)) {
        int img_depth = iftGetLongValFromDict("--img-depth", args);
        if (img_depth <= 0)
            iftError("Invalid Image Depth: %d... Try > 0", "iftGetOptionalArgs", img_depth);

        img_range = (1 << img_depth) - 1; // (2^img_depth) - 1
    }
    else img_range = iftNormalizationValue(iftMaximumValue(img));

    printf("  - Image Range: [0, %d]\n", img_range);

    return img_range;
}



