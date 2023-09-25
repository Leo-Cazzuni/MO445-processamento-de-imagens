#include "ift.h"


/************************** HEADERS **************************/
iftDict *iftGetArgs(int argc, const char *argv[]);
void iftGetRequiredArgs(const iftDict *args, char **img_path, int *n_bins_per_band, char **out_path);
/*************************************************************/





int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArgs(argc, argv);

    char *img_path = NULL;
    int n_bins_per_band;
    char *out_path = NULL;

    iftGetRequiredArgs(args, &img_path, &n_bins_per_band, &out_path);

    timer *t1 = iftTic();

    iftImage *img = iftReadImageByExt(img_path);
    int norm_val = iftNormalizationValue(iftMaximumValue(img));

    iftImage *quant_img = iftQuantize(img, n_bins_per_band, norm_val);

    iftWriteImageByExt(quant_img, out_path);

    puts("\nDone...");
    puts(iftFormattedTime(iftCompTime(t1, iftToc())));

    // DESTROYERS
    iftDestroyDict(&args);

    return 0;
}


/************************** SOURCES **************************/
iftDict *iftGetArgs(int argc, const char *argv[]) {
    char program_description[2048] = \
        "- Quantize an image.\n";

    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--image", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Image pathname."},
        {.short_name = "-n", .long_name = "--num-bins-per-band", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=true, .help="Number of bins per Band. E.g: 8"},
        {.short_name = "-o", .long_name = "--out-image", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Output pathname."},
    };

    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}

void iftGetRequiredArgs(const iftDict *args, char **img_path, int *n_bins_per_band, char **out_path) {
    *img_path = iftGetStrValFromDict("--image", args);
    *n_bins_per_band = iftGetLongValFromDict("--num-bins-per-band", args);
    *out_path = iftGetStrValFromDict("--out-image", args);

    puts("-----------------------------");
    printf("- Image: %s\n", *img_path);
    printf("- Num. Bins per Band: %d\n", *n_bins_per_band);
    printf("- Out Quantized Image: %s\n", *out_path);
    puts("-----------------------------\n");
}
