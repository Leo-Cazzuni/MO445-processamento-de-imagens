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
    int delay          = iftGetLongValFromDict("--delay-frames", args);
    const char *out_filneName   = iftGetConstStrValFromDict("--out-name", args);

    iftValidateInputs(img_entry, delay, out_filneName);

    iftFileSet *fset = iftLoadFileSetFromDirOrCSV(img_entry, 0, true);


    iftGifWriter* gifWriter = NULL;
    uint8_t* imageVec = NULL;
    int channelDepth = 8;
    int alpha = 255;
    bool useDither = false;
    gifWriter = (iftGifWriter*)calloc(1,sizeof(iftGifWriter));

    for (size_t i = 0; i < fset->n; i++) {
        const char *img_path = fset->files[i]->path;
        char *filename       = iftFilename(img_path, NULL);
        iftImage *img = iftReadImageByExt(img_path);

        if(i == 0){
            iftGifBegin(gifWriter,out_filneName, img->xsize,img->ysize,delay,channelDepth,useDither);
        }
        printf("Reading: \"%s\"\n", img_path);
        imageVec = convertImageYcbCr2IntergerArray8bits(img,alpha,0);
        iftGifWriteFrame(gifWriter,imageVec,img->xsize,img->ysize,delay,channelDepth,useDither);

        iftFree(imageVec);
        iftFree(filename);

        iftDestroyImage(&img);
    }

    iftGifEnd(gifWriter);

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
                    .required=true, .help="Dir with the Input Images."},
            {.short_name = "-d", .long_name = "--delay-frames", .has_arg=true, .arg_type=IFT_LONG_TYPE,
                    .required=true, .help="delay between frames."},
            {.short_name = "-o", .long_name = "--out-name", .has_arg=true, .arg_type=IFT_STR_TYPE,
                    .required=true, .help="[output_filename].gif ."},
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);


    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}


void iftValidateInputs(const char *img_entry, int delay, const char *out_filename) {
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
    if (delay <= 0)
        iftError("invalid value for delay", "iftValidateInputs", delay);

    if(out_filename == NULL){
        iftError("output filename is NULL", "iftValidateInputs", out_filename);
    }

    puts("**************************");
    printf("- Image Entry: \"%s\"\n", img_entry);
    printf("- delay: %d\n", delay);
    printf("- Out Filename: \"%s\"\n", out_filename);
    puts("**************************\n");
}
/*************************************************************/







