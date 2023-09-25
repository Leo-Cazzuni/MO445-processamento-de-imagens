/**
 * @file
 * @brief Matches the Histograms from a Set of Images to a Reference Image.
 * 
 * @example iftMatchHistogramImageSet.c
 * @brief Matches the Histograms from a Set of Images to a Reference Image.
 * @author Samuel Martins
 * @date Jul 11, 2016
 */

#include "ift.h"

/************************** HEADERS **************************/
iftDict *iftGetArgs(int argc, const char *argv[]);
void iftGetRequiredArgs(const iftDict *args, char **img_entry, char **ref_img_path, char **out_dir);
void iftValidateRequiredArgs(const char *img_entry, const char *ref_img_path, const char *out_dir);
/*************************************************************/



int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArgs(argc, argv);

    // mandatory args
    char *img_entry    = NULL;
    char *ref_img_path = NULL;
    char *out_dir      = NULL;

    iftGetRequiredArgs(args, &img_entry, &ref_img_path, &out_dir);

    timer *t1 = iftTic();

    iftFileSet *img_set = iftLoadFileSetFromDirOrCSV(img_entry, 0, false);
    iftImage *ref_img   = iftReadImageByExt(ref_img_path);

//    int min_ref = iftMinimumValue(ref_img);
//    int max_ref = iftMaximumValue(ref_img);

    #pragma omp parallel for schedule(auto)
    for (size_t i = 0; i < img_set->n; i++) {
        const char *img_path = img_set->files[i]->path;
        char *filename       = iftFilename(img_path, NULL);
        char *out_img_path   = iftJoinPathnames(2, out_dir, filename);

        printf("[%lu] %s\n%s\n\n", i, img_path, out_img_path);
        iftImage *img     = iftReadImageByExt(img_path);
        // iftImage *out_img = iftMatchHistogram(img, ref_img);
        iftImage *out_img = iftMatchHistogram(img, NULL, ref_img, NULL);
        // iftImage *out_img = iftNormalizeWithNoOutliers(img, 0, 4095, 0.98);
//        iftImage *out_img = iftLinearStretch(img, iftMinimumValue(img), iftMaximumValue(img), 0, 4095);

        iftWriteImageByExt(out_img, out_img_path);

        iftDestroyImage(&img);
        iftDestroyImage(&out_img);
        iftFree(out_img_path);
        iftFree(filename);
    }

    puts("\nDone...");
    puts(iftFormattedTime(iftCompTime(t1, iftToc())));


    // DESTROYERS
    iftDestroyDict(&args);
    iftDestroyFileSet(&img_set);
    iftDestroyImage(&ref_img);
    iftFree(img_entry);
    iftFree(ref_img_path);
    iftFree(out_dir);

    return 0;
}


/************************** SOURCES **************************/
iftDict *iftGetArgs(int argc, const char *argv[]) {
    char program_description[2048] = \
        "Matches the Histograms from a Set of Images to a Reference Image.";

    iftCmdLineOpt cmd_line_opts[] = {
            {.short_name = "-i", .long_name = "--image-entry", .has_arg=true, .arg_type=IFT_STR_TYPE,
             .required=true, .help="Dir with the Input Images or a CSV file with their pathnames."},
            {.short_name = "-r", .long_name = "--ref-image", .has_arg=true, .arg_type=IFT_STR_TYPE,
             .required=true, .help="Reference Image."},
            {.short_name = "-o", .long_name = "--output-dir", .has_arg=true, .arg_type=IFT_STR_TYPE,
             .required=true, .help="Output Dir where the resulting Images will be saved."}
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}


void iftGetRequiredArgs(const iftDict *args, char **img_entry, char **ref_img_path, char **out_dir) {
    *img_entry    = iftGetStrValFromDict("--image-entry", args);
    *ref_img_path = iftGetStrValFromDict("--ref-image", args);
    *out_dir      = iftGetStrValFromDict("--output-dir", args);

    iftValidateRequiredArgs(*img_entry, *ref_img_path, *out_dir);


    puts("-----------------------");
    printf("- Image Entry: \"%s\"\n", *img_entry);
    printf("- Reference Image: \"%s\"\n", *ref_img_path);
    printf("- Output Dir: \"%s\"\n", *out_dir);
    puts("-----------------------\n");
}


void iftValidateRequiredArgs(const char *img_entry, const char *ref_img_path, const char *out_dir) {
    // IMAGE ENTRY
    if (iftFileExists(img_entry)) {
        if (!iftEndsWith(img_entry, ".csv")) {
            iftError("Image Set Entry File \"%s\" is not a CSV file... Try *.csv", "iftValidateRequiredArgs", img_entry);
        }
    }
    else if (!iftDirExists(img_entry))
        iftError("Invalid Pathname for the Label Image Set Entry: \"%s\"...\nTry a valid Directory or a CSV file",
                 "iftValidateRequiredArgs", img_entry);

    if (!iftIsImageFile(ref_img_path))
        iftError("Invalid Ref. Image: \"%s\"", "iftValidateRequiredArgs", ref_img_path);

    if (!iftDirExists(out_dir))
        iftMakeDir(out_dir);
}










