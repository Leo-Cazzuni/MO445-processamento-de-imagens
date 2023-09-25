#include "ift.h"


/************************** HEADERS **************************/
iftDict *iftGetArgs(int argc, const char *argv[]);
void iftGetRequiredArgs(const iftDict *args, char **img_path, char **label_img_path, char **out_label_img_path);
void iftGetOptionalArgs(const iftDict *args, float *smooth_factor, int *n_iters);
/*************************************************************/





int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArgs(argc, argv);

    char *img_path           = NULL;
    char *label_img_path     = NULL;
    char *out_label_img_path = NULL;
    float smooth_factor;
    int n_iters;

    iftGetRequiredArgs(args, &img_path, &label_img_path, &out_label_img_path);
    iftGetOptionalArgs(args, &smooth_factor, &n_iters);


    timer *t1 = iftTic();

    iftImage *img       = iftReadImageByExt(img_path);
    iftImage *label_img = iftReadImageByExt(label_img_path);

    puts("- Smoothing Label Image (Regions) by Diffusion");
    iftImage *smooth_img = iftSmoothRegionsByDiffusion(label_img, img, smooth_factor, n_iters);
    iftWriteImageByExt(smooth_img, out_label_img_path);


    puts("\nDone...");
    puts(iftFormattedTime(iftCompTime(t1, iftToc())));
    

    // DESTROYERS
    iftDestroyDict(&args);
    iftFree(img_path);
    iftFree(label_img_path);
    iftFree(out_label_img_path);
    iftDestroyImage(&img);
    iftDestroyImage(&label_img);
    iftDestroyImage(&smooth_img);

    return 0;
}


/************************** SOURCES **************************/
iftDict *iftGetArgs(int argc, const char *argv[]) {
    char program_description[2048] = \
        "- Smooth a Label Image (Mask) by Diffusion.\n";

    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--input-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Input Original Image."},
        {.short_name = "-l", .long_name = "--label-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Label Image (from the input one) to be smoothed."},
        {.short_name = "-o", .long_name = "--output-smoothed-label-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Output Smoothed Labeled Image."},
        {.short_name = "-f", .long_name = "--smooth-factor", .has_arg=true, .arg_type=IFT_DBL_TYPE,
         .required=false, .help="Smooth Factor [0, 1]. Defaul: 0.5"},
        {.short_name = "-n", .long_name = "--num-iterations", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=false, .help="Number of Iteration. Defaul: 5"}
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}


void iftGetRequiredArgs(const iftDict *args, char **img_path, char **label_img_path, char **out_label_img_path) {
    *img_path           = iftGetStrValFromDict("--input-img", args);
    *label_img_path     = iftGetStrValFromDict("--label-img", args);
    *out_label_img_path = iftGetStrValFromDict("--output-smoothed-label-img", args);

    puts("-----------------------");
    printf("- Image: %s\n", *img_path);
    printf("- Label Image: %s\n", *label_img_path);
    puts("-----------------------");
}



void iftGetOptionalArgs(const iftDict *args, float *smooth_factor, int *n_iters) {
    if (iftDictContainKey("--smooth-factor", args, NULL))
        *smooth_factor = iftGetDblValFromDict("--smooth-factor", args);
    else *smooth_factor = 0.5;

    if (iftDictContainKey("--num-iterations", args, NULL))
        *n_iters = iftGetLongValFromDict("--num-iterations", args);
    else *n_iters = 5;

    printf("- Smooth Factor: %f\n", *smooth_factor);
    printf("- Number of Iterations: %d\n", *n_iters);
    puts("-----------------------\n");
}
/*************************************************************/










