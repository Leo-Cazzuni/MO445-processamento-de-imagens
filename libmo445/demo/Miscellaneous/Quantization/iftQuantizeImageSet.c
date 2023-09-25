#include "ift.h"


/************************** HEADERS **************************/
iftDict *iftGetArgs(int argc, const char *argv[]);
void iftGetRequiredArgs(const iftDict *args, iftFileSet **img_set, int *n_bins_per_band, char **out_dir);
/*************************************************************/





int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArgs(argc, argv);

    iftFileSet *img_set = NULL;
    int n_bins_per_band;
    char *out_dir = NULL;

    iftGetRequiredArgs(args, &img_set, &n_bins_per_band, &out_dir);

    timer *t1 = iftTic();

    #pragma omp parallel for
    for (int f = 0; f < img_set->n; f++) {
        char *out_path = iftJoinPathnames(2, out_dir, iftFilename(img_set->files[f]->path, NULL));

        printf("[%d/%ld] %s\n%s\n\n", f, img_set->n - 1,
                                     img_set->files[f]->path, out_path);

        iftImage *img = iftReadImageByExt(img_set->files[f]->path);
        int norm_val = iftNormalizationValue(iftMaximumValue(img));

        iftImage *quant_img = iftQuantize(img, n_bins_per_band, norm_val);
        iftWriteImageByExt(quant_img, out_path);
        iftDestroyImage(&quant_img);
    }


    puts("\nDone...");
    puts(iftFormattedTime(iftCompTime(t1, iftToc())));

    // DESTROYERS
    iftDestroyDict(&args);

    return 0;
}


/************************** SOURCES **************************/
iftDict *iftGetArgs(int argc, const char *argv[]) {
    char program_description[2048] = \
        "- Quantize an image set.\n";

    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--image-set", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="CSV or Directory with the image set."},
        {.short_name = "-n", .long_name = "--num-bins-per-band", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=true, .help="Number of bins per Band. E.g: 8"},
        {.short_name = "-o", .long_name = "--out-dir", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Output directory."},
    };

    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}

void iftGetRequiredArgs(const iftDict *args, iftFileSet **img_set, int *n_bins_per_band, char **out_dir) {
    const char *img_path = iftGetConstStrValFromDict("--image-set", args);
    *img_set = iftLoadFileSetFromDirOrCSV(img_path, 0, true);

    *n_bins_per_band = iftGetLongValFromDict("--num-bins-per-band", args);
    *out_dir = iftGetStrValFromDict("--out-dir", args);
    if (!iftDirExists(*out_dir)) {
        iftMakeDir(*out_dir);
    }

    puts("-----------------------------");
    printf("- Image Set: %s\n", img_path);
    printf("- Num. Bins per Band: %d\n", *n_bins_per_band);
    printf("- Out Dir: %s\n", *out_dir);
    puts("-----------------------------\n");
}
