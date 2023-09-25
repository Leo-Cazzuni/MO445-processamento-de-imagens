/**
 * @author Samuel Martins
 * @date Mar 1, 2016
 */

#include "ift.h"

/************************** HEADERS **************************/
iftDict *iftGetArgumentsFromCmdLine(int argc, const char *argv[]);
void iftValidateInputs(const char *vol_img_path, const char *plane_name, const char *out_path);
iftImagePlaneOrientation iftGetImagePlaneFromName(const char *plane_name);
void iftGetArguments(char **vol_img_path, iftImagePlaneOrientation *plane_orientation,
                     int *slice, char **out_path);
/*************************************************************/

int main(int argc, const char *argv[]) {
    iftDict *args            = iftGetArgumentsFromCmdLine(argc, argv);
    const char *vol_img_path = iftGetConstStrValFromDict("--image", args);
    const char *plane_name   = iftGetConstStrValFromDict("--plane-orientation", args);
    const char *out_path     = iftGetConstStrValFromDict("--output-image", args);
    iftValidateInputs(vol_img_path, plane_name, out_path);

    iftImagePlaneOrientation plane_orientation = iftGetImagePlaneFromName(plane_name);
    iftImage *vol_img                          = iftReadImageByExt(vol_img_path);
    
    int slice;
    if (iftDictContainKey("--slice", args, NULL))
        slice = iftGetLongValFromDict("--slice", args);
    else {
        if (plane_orientation == IFT_AXIAL)
            slice = (int) (vol_img->zsize + 1) / 2;
        else if (plane_orientation == IFT_CORONAL)
            slice = (int) (vol_img->ysize + 1) / 2;
        else // sagittal
            slice = (int) (vol_img->xsize + 1) / 2;
    }
    printf("- Slice: %d\n", slice);
    puts("-----------------------\n");
    

    iftImage *slice_img = iftExtractSlice(vol_img, plane_orientation, slice);
    iftImage *out       = NULL;
    if (iftDictContainKey("--normalize-image", args, NULL)) {
        int min_val = iftMinimumValue(slice_img);
        int max_val = iftMaximumValue(slice_img);
        printf("- Normalizing Slice Image (Linear Stretch) to 8 bits: [%d, %d] to [0, 255]\n", min_val, max_val);
        out = iftLinearStretch(slice_img, min_val, max_val, 0, 255);
        iftDestroyImage(&slice_img);
    }
    else out = slice_img;

    iftWriteImageByExt(out, out_path);
    puts("Done...");

    // DESTROYERS
    iftDestroyDict(&args);
    iftDestroyImage(&vol_img);
    iftDestroyImage(&out);

    return 0;
}



/************************** SOURCES **************************/
iftDict *iftGetArgumentsFromCmdLine(int argc, const char *argv[]) {
    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--image", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="3D Image pathname."},
        {.short_name = "-p", .long_name = "--plane-orientation", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Image Plane: [AXIAL, CORONAL, SAGITTAL]"},
        {.short_name = "-o", .long_name = "--output-image", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Output Image pathname."},
        {.short_name = "-s", .long_name = "--slice", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=false, .help="Number of the Desired Slice. Default: Central Slice."},
         {.short_name = "-n", .long_name = "--normalize-image", .has_arg=false,
         .required=false, .help="Normalize the Image (Linear Stretch) to 8 bits."}
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);
    

    char program_description[2048] = \
        "This program extracts a slice of a given Plane (AXIAL, CORONAL, SAGITTAL) from a 3D Image.\n"
        "The slice is saved as a 2D Image";
    
    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}


void iftValidateInputs(const char *vol_img_path, const char *plane_name, const char *out_path) {
    if (!iftIsImageFile(vol_img_path))
        iftError("Invalid Image Extension of \"%s\"\nTry *.scn, *.pgm, *.ppm, *.png", "iftValidateInputs", vol_img_path);

    char *aux = NULL;
    if (!iftRegexMatch(aux = iftUpperString(plane_name), "^(AXIAL|CORONAL|SAGITTAL)$"))
        iftError("Invalid Plane \"%s\"... Try AXIAL, CORONAL, SAGITTAL", "iftValidateInputs",
                 plane_name);
    free(aux);

    if (iftDirExists(out_path))
        iftError("Output Image \"%s\" is actually a directory", "iftValidateInputs",
                 out_path);
    if (!iftRegexMatch(out_path, "^.+\\.(pgm|ppm|scn|png)$"))
        iftError("Invalid Extension of the Output Image: \"%s\"... Try *.pgm, *.ppm, *.png, *.scn",
                 "iftValidateInputs", iftFileExt(out_path));

    puts("-----------------------");
    printf("- 3D Image: \"%s\"\n", vol_img_path);
    printf("- Image Plane Orientation: \"%s\"\n", plane_name);
    printf("- Output 2D (Slice) Image: \"%s\"\n", out_path);
}


iftImagePlaneOrientation iftGetImagePlaneFromName(const char *plane_name) {
    iftImagePlaneOrientation plane_orientation = IFT_SAGITTAL;

    char *plane_name_upper = iftUpperString(plane_name);
    if (iftRegexMatch(plane_name_upper, "^(AXIAL|CORONAL|SAGITTAL)$")) {
        if (iftCompareStrings(plane_name_upper, "AXIAL")) {
            plane_orientation = IFT_AXIAL;
        }
        else if (iftCompareStrings(plane_name_upper, "CORONAL"))
            plane_orientation = IFT_CORONAL;
        else // SAGITTAL
            plane_orientation = IFT_SAGITTAL;
    }
    else
        iftError("Invalid Plane \"%s\"... Try AXIAL, CORONAL, SAGITTAL", "iftValidateInputs",
                 plane_name);
    free(plane_name_upper);

    return plane_orientation;
}


void iftGetArguments(char **vol_img_path, iftImagePlaneOrientation *plane_orientation,
                     int *slice, char **out_path) {

}
/*************************************************************/
