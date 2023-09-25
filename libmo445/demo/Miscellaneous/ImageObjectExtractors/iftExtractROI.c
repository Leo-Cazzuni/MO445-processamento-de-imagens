/**
 * @file
 * @brief Extracts a Region Of Interest (ROI) from an Image.
 * @note See the source code in @ref iftExtractROI.c
 * 
 * @example iftExtractROI.c
 * @brief Extracts a Region Of Interest (ROI) from an Image.
 * @author Samuel Martins
 * @date Mar 3, 2016
 */

#include "ift.h"


/************************** HEADERS **************************/
iftDict *iftGetArgumentsFromCmdLine(int argc, const char *argv[]);
void iftValidateInputs(const char *img_path, const char *out_img_path);
/*************************************************************/



int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArgumentsFromCmdLine(argc, argv);

    const char *img_path     = iftGetConstStrValFromDict("-i", args);
    const char *out_img_path = iftGetConstStrValFromDict("-o", args);

    iftValidateInputs(img_path, out_img_path);

    iftVoxel uo, uf;
    uo.x = iftGetLongValFromDict("--xo", args);
    uo.y = iftGetLongValFromDict("--yo", args);
    uo.z = iftGetLongValFromDict("--zo", args);
    uf.x   = iftGetLongValFromDict("--xf", args);
    uf.y   = iftGetLongValFromDict("--yf", args);
    uf.z   = iftGetLongValFromDict("--zf", args);

    puts("-----------------------");
    printf("- Input Image: \"%s\"\n", img_path);
    printf("- Output Image: \"%s\"\n", out_img_path);
    printf("- ROI: from (%d, %d, %d) to (%d, %d, %d)\n", uo.x, uo.y, uo.z, uf.x, uf.y, uf.z);
    puts("-----------------------\n");

    iftImage *img = iftReadImageByExt(img_path);
    iftImage *out_img   = NULL;

    iftBoundingBox bb = {.begin = uo, .end = uf};
    out_img            = iftExtractROI(img, bb);

    printf("- Input Image Size (x, y, z): (%d, %d, %d)\n", img->xsize, img->ysize, img->zsize);
    printf("- Output Image Size (x, y, z): (%d, %d, %d)\n", out_img->xsize, out_img->ysize, out_img->zsize);

    if (iftDictContainKey("--normalize", args, NULL)) {
        iftImage *norm_img = iftLinearStretch(out_img, iftMinimumValue(out_img), iftMaximumValue(out_img), 0, 255);
        iftDestroyImage(&out_img);
        out_img = norm_img;
    }
    iftWriteImageByExt(out_img, out_img_path);

    // DESTROYERS
    iftDestroyDict(&args);
    iftDestroyImage(&img);
    iftDestroyImage(&out_img);

    return 0;
}



/************************** SOURCES **************************/
iftDict *iftGetArgumentsFromCmdLine(int argc, const char *argv[]) {
    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--input-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Image where the ROI will be extracted."},
        {.short_name = "", .long_name = "--xo", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=true, .help="Initial x coordinate."},
        {.short_name = "", .long_name = "--yo", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=true, .help="Initial y coordinate."},
        {.short_name = "", .long_name = "--zo", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=true, .help="Initial z coordinate."},
        {.short_name = "", .long_name = "--xf", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=true, .help="Final x coordinate."},
        {.short_name = "", .long_name = "--yf", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=true, .help="Final y coordinate."},
        {.short_name = "", .long_name = "--zf", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=true, .help="Final z coordinate."},
        {.short_name = "-o", .long_name = "--output-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Output pathname from the resulting Image"},
        {.short_name = "-n", .long_name = "--normalize", .has_arg=false,
         .required=false, .help="Normalize the Output Image to 8 bits [0,255]"}
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);
    
    
    char program_description[2048] = \
        "This program extracts a given ROI with coordinates from (xo,yo,zo) to (xf,yf,zf) from an Image.";
    
    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}


void iftValidateInputs(const char *img_path, const char *out_img_path) {
    if (!iftIsImageFile(img_path))
        iftError("Invalid Image: \"%s\"", "iftValidateInputs", img_path);

    if (iftDirExists(out_img_path))
        iftError("Output Image is actually a Directory", "iftValidateInputs", out_img_path);
    else if (!iftIsImagePathnameValid(out_img_path))
            iftError("Invalid Output Image Pathname: \"%s\"\nTry the supported extensions: scn, ppm, pgm, png",
                     "iftValidateInputs", out_img_path);
}




