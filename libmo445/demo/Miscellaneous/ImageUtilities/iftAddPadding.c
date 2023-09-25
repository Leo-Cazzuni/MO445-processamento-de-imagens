/**
 * @file
 * @brief Adds padding around an Image with a given optional voxel value.
 * @note See the source code in @ref iftAddPadding.c
 *
 * @example iftAddPadding.c
 * @brief Adds padding around an Image with a given optional voxel value.
 * @author Thiago V. Spina
 * @date Dec 1, 2016
 */

#include "ift.h"


/************************** HEADERS **************************/
iftDict *iftGetArguments(int argc, const char *argv[]);
void iftValidateInputs(const char *img_entry, int x_padding, int y_padding, int z_padding, int value, const char *out_file);
/*************************************************************/

int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArguments(argc, argv);
    const char *img_file    = iftGetConstStrValFromDict("--img-file", args);
    int x_padding           = 0;
    int y_padding           = 0;
    int z_padding           = 0;
    int value               = 0;
    const char *out_file    = iftGetConstStrValFromDict("--out-file", args);

    if(iftDictContainKey("--x-padding", args, NULL))
        x_padding           = iftGetLongValFromDict("--x-padding", args);
    if(iftDictContainKey("--y-padding", args, NULL))
        y_padding           = iftGetLongValFromDict("--y-padding", args);
    if(iftDictContainKey("--z-padding", args, NULL))
        z_padding           = iftGetLongValFromDict("--z-padding", args);
    if(iftDictContainKey("--value", args, NULL))
        value           = iftGetLongValFromDict("--value", args);

    iftValidateInputs(img_file, x_padding, y_padding, z_padding, value, out_file);

    iftImage *img    = iftReadImageByExt(img_file);
    iftImage *padded = iftAddPadding(img, x_padding, y_padding, z_padding, value);

    iftWriteImageByExt(padded, out_file);

    iftDestroyImage(&img);
    iftDestroyImage(&padded);
    // DESTROYERS
    iftDestroyDict(&args);

    return 0;
}


/************************** SOURCES **************************/
iftDict *iftGetArguments(int argc, const char *argv[]) {
    char program_description[2048] = \
        "Adds Zero-Frames around a set of Images.";

    iftCmdLineOpt cmd_line_opts[] = {
            {.short_name = "-i", .long_name = "--img-file", .has_arg=true, .arg_type=IFT_STR_TYPE,
                    .required=true, .help="Input Image."},
            {.short_name = "-x", .long_name = "--x-padding", .has_arg=true, .arg_type=IFT_LONG_TYPE,
                    .required=false, .help="Padding in the X direction."},
            {.short_name = "-y", .long_name = "--y-padding", .has_arg=true, .arg_type=IFT_LONG_TYPE,
                    .required=false, .help="Padding in the Y direction."},
            {.short_name = "-z", .long_name = "--z-padding", .has_arg=true, .arg_type=IFT_LONG_TYPE,
                    .required=false, .help="Padding in the Z direction."},
            {.short_name = "-v", .long_name = "--value", .has_arg=true, .arg_type=IFT_LONG_TYPE,
                    .required=false, .help="Value used for padding."},
            {.short_name = "-o", .long_name = "--out-file", .has_arg=true, .arg_type=IFT_STR_TYPE,
                    .required=true, .help="Output Image."},
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);


    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}


void iftValidateInputs(const char *img_file, int x_padding, int y_padding, int z_padding, int value, const char *out_file) {
    // IMAGE ENTRY
    if (!iftFileExists(img_file)) {
        iftError("The Image File \"%s\" does not exist", "iftValidateInputs", img_file);
    }

    puts("**************************");
    printf("- Image: \"%s\"\n", img_file);
    printf("- X padding: %d\n", x_padding);
    printf("- Y padding: %d\n", y_padding);
    printf("- Z padding: %d\n", z_padding);
    printf("- Value: %d\n", value);
    printf("- Output image: \"%s\"\n", out_file);
    puts("**************************\n");
}
/*************************************************************/






