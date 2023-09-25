/**
 * @file
 * @brief Extract the Min. Bounding Box (ROI) from an Image with all objects/voxels (non-zero) inside it.
 * @note See the source code in @ref iftExtractImageROI.c
 * 
 * @example iftExtractImageROI.c
 * @brief Extract the Min. Bounding Box (ROI) from an Image with all objects/voxels (non-zero) inside it.
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

    const char *img_path = iftGetConstStrValFromDict("-i", args);
    const char *out_img_path   = iftGetConstStrValFromDict("-o", args);

    iftValidateInputs(img_path, out_img_path);

    puts("-----------------------");
    printf("- Input Image: \"%s\"\n", img_path);
    printf("- Output Image: \"%s\"\n", out_img_path);
    puts("-----------------------\n");

    iftImage *img = iftReadImageByExt(img_path);
    iftImage *out_img   = NULL;

    iftPoint gc;
    iftBoundingBox mbb = iftMinBoundingBox(img, &gc);
    out_img            = iftExtractROI(img, mbb);

    printf("- Input Image Size (x, y, z): (%d, %d, %d)\n", img->xsize, img->ysize, img->zsize);
    printf("- Output Image Size (x, y, z): (%d, %d, %d)\n", out_img->xsize, out_img->ysize, out_img->zsize);
    printf("- Min. Bound. Box: begin: (%d, %d, %d), end (%d, %d, %d)\n",
            mbb.begin.x, mbb.begin.y, mbb.begin.z, mbb.end.x, mbb.end.y, mbb.end.z);
    printf("- Out. Image Geometric Center in the Input Image (x, y, z): (%f, %f, %f)\n", gc.x, gc.y, gc.z);

    for (int i = 0; i < out_img->n; i++)
        printf("[%d] = %d\n", i, out_img->val[i]);

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
         .required=true, .help="Input Image with labels (values) from [0, n_labels], where the object "\
                               "will be extracted."},
        {.short_name = "-o", .long_name = "--output-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Output pathname from the resulting Image"},
        {.short_name = "-n", .long_name = "--normalize", .has_arg=false,
         .required=false, .help="Normalize the Output Image to 8 bits [0,255]"}
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);
    
    
    char program_description[2048] = \
        "This program extracts the Minimum Bounding Box (ROI) from an Image.\n" \
        "All objects/voxels (non-zero) will be contained inside it.";
    
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




