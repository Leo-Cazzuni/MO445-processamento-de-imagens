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
    const char *vol_img_path = iftGetConstStrValFromDict("--image", args);
    int delay          = iftGetLongValFromDict("--delay-frames", args);
    const char *out_filneName   = iftGetConstStrValFromDict("--out-name", args);

    iftValidateInputs(vol_img_path, delay, out_filneName);
    iftImage *vol_img = iftReadImageByExt(vol_img_path);
    int alpha = 255;

    iftWriteGif(vol_img,alpha,delay,out_filneName);

    // DESTROYERS
    iftDestroyDict(&args);

    return 0;
}


/************************** SOURCES **************************/
iftDict *iftGetArguments(int argc, const char *argv[]) {
    char program_description[2048] = \
        "Adds Zero-Frames around a set of Images.";

    iftCmdLineOpt cmd_line_opts[] = {
            {.short_name = "-i", .long_name = "--image", .has_arg=true, .arg_type=IFT_STR_TYPE,
                    .required=true, .help="filename path to 3D image (*.scn)."},
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


void iftValidateInputs(const char *vol_img_path, int delay, const char *out_filename) {
    if (!iftIsImageFile(vol_img_path))
        iftError("Invalid Image Extension of \"%s\"\nTry *.scn", "iftValidateInputs", vol_img_path);

    // NUMBER OF FRAMES
    if (delay <= 0)
        iftError("invalid value for delay", "iftValidateInputs", delay);

    if(out_filename == NULL){
        iftError("output filename is NULL", "iftValidateInputs", out_filename);
    }

    puts("**************************");
    printf("- Image Entry: \"%s\"\n", vol_img_path);
    printf("- delay: %d\n", delay);
    printf("- Out Filename: \"%s\"\n", out_filename);
    puts("**************************\n");
}
/*************************************************************/







