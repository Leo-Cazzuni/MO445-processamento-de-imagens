/**
 * @file
 * @brief Adds Zero-Frames around a set of Images.
 * @note See the source code in @ref iftAddFramesToImages.c
 *
 * @example iftAddFramesToImages.c
 * @brief Adds Zero-Frames around a set of Images.
 * @author Samuel Martins
 * @date Apr 18, 2016
 */

#include "ift.h"


/************************** HEADERS **************************/
iftDict *iftGetArguments(int argc, const char *argv[]);
void iftValidateInputs(const char *img_entry, int n_frames, const char *out_dir);
/*************************************************************/

int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArguments(argc, argv);
    const char *img_entry = iftGetConstStrValFromDict("--img-entry", args);
    int n_frames          = iftGetLongValFromDict("--num-frames", args);
    const char *out_dir   = iftGetConstStrValFromDict("--out-dir", args);

    iftValidateInputs(img_entry, n_frames, out_dir);

    iftFileSet *fset = iftLoadFileSetFromDirOrCSV(img_entry, 0, true);

    #pragma omp parallel for
    for (size_t i = 0; i < fset->n; i++) {
        const char *img_path = fset->files[i]->path;
        char *filename       = iftFilename(img_path, NULL);
        char *out_path       = iftJoinPathnames(2, out_dir, filename);

        printf("[%lu/%lu]\n", i+1, fset->n);
        printf("In: \"%s\"\n", img_path);
        printf("Out: \"%s\"\n\n", out_path);

        iftImage *img = iftReadImageByExt(img_path);
        iftImage *out = iftAddFrame(img, n_frames, 0);
        iftWriteImage(out, out_path);

        // DESTROYERS
        iftFree(filename);
        iftFree(out_path);
        iftDestroyImage(&img);
        iftDestroyImage(&out);
    }


    // DESTROYERS
    iftDestroyDict(&args);
    iftDestroyFileSet(&fset);

    return 0;
}


/************************** SOURCES **************************/
iftDict *iftGetArguments(int argc, const char *argv[]) {
    char program_description[2048] = \
        "Adds Zero-Frames around a set of Images.";

    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--img-entry", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Dir with the Input Images or a CSV file with their pathnames."},
        {.short_name = "-n", .long_name = "--num-frames", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=true, .help="Number of Frames to be added."},
        {.short_name = "-o", .long_name = "--out-dir", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Output Directory where the images are saved."},
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);
        
    
    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}


void iftValidateInputs(const char *img_entry, int n_frames, const char *out_dir) {
    // IMAGE ENTRY
    if (iftFileExists(img_entry)) {
        if (!iftEndsWith(img_entry, ".csv")) {
            iftError("The Image Set Entry File \"%s\" is not a CSV file... Try *.csv", "iftValidateInputs", img_entry);
        }
    }
    else if (!iftDirExists(img_entry))
        iftError("Invalid Pathname for the Image Set Entry: \"%s\"...\nTry a valid Directory or a CSV file",
                 "iftValidateInputs", img_entry);

    // NUMBER OF FRAMES
    if (n_frames <= 0)
        iftError("Number of Frames %d <= 0", "iftValidateInputs", n_frames);

    // OUTPUT DIR
    if (!iftDirExists(out_dir))
        iftMakeDir(out_dir);

    puts("**************************");
    printf("- Image Entry: \"%s\"\n", img_entry);
    printf("- Number of Frames: %d\n", n_frames);
    printf("- Out Directory: \"%s\"\n", out_dir);
    puts("**************************\n");
}
/*************************************************************/






