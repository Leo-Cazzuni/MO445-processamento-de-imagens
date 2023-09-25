/**
 * @file
 * @brief Inserts/Copies an Image (Region Of Interest (ROI)) inside a Target Image.
 * @note See the source code in @ref iftInsertROI.c
 * 
 * @example iftInsertROI.c
 * @brief Inserts/Copies an Image (Region Of Interest (ROI)) inside a Target Image.
 * @author Samuel Martins
 * @date Mar 3, 2016
 */

#include "ift.h"


/************************** HEADERS **************************/
iftDict *iftGetArgumentsFromCmdLine(int argc, const char *argv[]);
void iftValidateInputs(const char *roi_path, const char *target_path, const char *out_img_path);
/*************************************************************/



int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArgumentsFromCmdLine(argc, argv);

    const char *roi_path = iftGetConstStrValFromDict("-r", args);
    const char *target_path     = iftGetConstStrValFromDict("-i", args);
    const char *out_img_path = iftGetConstStrValFromDict("-o", args);

    iftValidateInputs(roi_path, target_path, out_img_path);

    iftVoxel uo;
    uo.x = iftGetLongValFromDict("--xo", args);
    uo.y = iftGetLongValFromDict("--yo", args);
    uo.z = iftGetLongValFromDict("--zo", args);

    puts("-----------------------");
    printf("- ROI Image: \"%s\"\n", roi_path);
    printf("- Target Image: \"%s\"\n", target_path);
    printf("- Initial Position (xo, yo, zo): (%d, %d, %d)\n", uo.x, uo.y, uo.z);
    printf("- Output Image: \"%s\"\n", out_img_path);
    puts("-----------------------\n");

    iftImage *roi    = iftReadImageByExt(roi_path);
    iftImage *target = iftReadImageByExt(target_path);

    iftInsertROI(roi, target, uo);

    if (iftDictContainKey("--normalize", args, NULL)) {
        iftImage *norm_img = iftLinearStretch(target, iftMinimumValue(target), iftMaximumValue(target), 0, 255);
        iftDestroyImage(&target);
        target = norm_img;
    }
    iftWriteImageByExt(target, out_img_path);

    // DESTROYERS
    iftDestroyDict(&args);
    iftDestroyImage(&roi);
    iftDestroyImage(&target);

    return 0;
}



/************************** SOURCES **************************/
iftDict *iftGetArgumentsFromCmdLine(int argc, const char *argv[]) {
    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-r", .long_name = "--roi-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="ROI Image to be inserted."},
        {.short_name = "-i", .long_name = "--input-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Image where the ROI will be inserted."},
        {.short_name = "", .long_name = "--xo", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=true, .help="Initial x coordinate in the Target Image."},
        {.short_name = "", .long_name = "--yo", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=true, .help="Initial y coordinate in the Target Image."},
        {.short_name = "", .long_name = "--zo", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=true, .help="Initial z coordinate. in the Target Image"},
        {.short_name = "-o", .long_name = "--output-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Output pathname from the resulting Image"},
        {.short_name = "-n", .long_name = "--normalize", .has_arg=false,
         .required=false, .help="Normalize the Output Image to 8 bits [0,255]"}
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);
    

    char program_description[2048] = \
        "Inserts an Image (Region Of Interest ROI) inside a Target Image from an initial position (xo,yo,zo).\n" \
        "If the Image DOESN'T fit entirely inside the Target Image, it will be copied/inserted what it is possible.";
    
    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}


void iftValidateInputs(const char *roi_path, const char *target_path, const char *out_img_path) {
    if (!iftIsImageFile(roi_path))
        iftError("Invalid Roi Image: \"%s\"", "iftValidateInputs", target_path);

    if (!iftIsImageFile(target_path))
        iftError("Invalid Target Image: \"%s\"", "iftValidateInputs", target_path);

    if (iftDirExists(out_img_path))
        iftError("Output Image is actually a Directory", "iftValidateInputs", out_img_path);
    else if (!iftIsImagePathnameValid(out_img_path))
            iftError("Invalid Output Image Pathname: \"%s\"\nTry the supported extensions: scn, ppm, pgm, png",
                     "iftValidateInputs", out_img_path);
}




