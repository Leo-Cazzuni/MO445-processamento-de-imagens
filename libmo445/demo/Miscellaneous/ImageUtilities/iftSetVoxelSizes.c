/**
 * @file
 * @brief Overwrites the Pixel/Voxel Sizes of an Image or a Set of Images to some given Values.
 * @note See the source code in @ref iftLabelImageAreaVolume.c
 *
 * @example iftSetVoxelSize.c
 * @brief Overwrites the Pixel/Voxel Sizes of an Image or a Set of Images to some given Values.
 * @author Samuel Martins
 * @date Mar 5, 2016
 */


#include "ift.h"


/************************** HEADERS **************************/
iftDict *iftGetArgumentsFromCmdLine(int argc, const char *argv[]);
void iftValidateInputs(const char *img_entry, float dx, float dy, float dz);
/*************************************************************/


int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArgumentsFromCmdLine(argc, argv);
    const char *img_entry = iftGetConstStrValFromDict("--img-entry", args);
    float dx              = iftGetDblValFromDict("--dx", args);
    float dy              = iftGetDblValFromDict("--dy", args);
    float dz              = iftGetDblValFromDict("--dz", args);

    iftValidateInputs(img_entry, dx, dy, dz);

    timer *t1 = iftTic();
    
    iftFileSet *img_paths = NULL;
    if (iftIsImageFile(img_entry)) { 
        img_paths = iftCreateFileSet(1);
        img_paths->files[0] = iftCreateFile(img_entry);
    }
    else {
        img_paths = iftLoadFileSetFromDirOrCSV(img_entry, 0, false);
    }


    for (size_t i = 0; i < img_paths->n; i++) {
        printf("[%lu/%lu] = \"%s\"\n", i+1, img_paths->n, img_paths->files[i]->path);
        iftImage *img = iftReadImageByExt(img_paths->files[i]->path);

        img->dx = dx;
        img->dy = dy;
        img->dz = dz;
        iftWriteImageByExt(img, img_paths->files[i]->path);

        iftDestroyImage(&img);
    }
    
    timer *t2 = iftToc();
    puts("Done...");
    iftPrintFormattedTime(iftCompTime(t1, t2));


    // DESTROYERS
    iftDestroyDict(&args);
    iftDestroyFileSet(&img_paths);

    return 0;
}




/************************** SOURCES **************************/
iftDict *iftGetArgumentsFromCmdLine(int argc, const char *argv[]) {
    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--img-entry", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Image Pathname or Image Directory or CSV file with the Image Pathnames."},
        {.short_name = "", .long_name = "--dx", .has_arg=true, .arg_type=IFT_DBL_TYPE,
         .required=true, .help="The X size of the pixel/voxel"},
        {.short_name = "", .long_name = "--dy", .has_arg=true, .arg_type=IFT_DBL_TYPE,
         .required=true, .help="The Y size of the pixel/voxel"},
        {.short_name = "", .long_name = "--dz", .has_arg=true, .arg_type=IFT_DBL_TYPE,
         .required=true, .help="The Z size of the pixel/voxel"}
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);
    
    
    char program_description[2048] = \
        "Overwrites the Sizes of the Pixel/Voxel from an Image or a Set of Images to some given Values.";
    
    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}


void iftValidateInputs(const char *img_entry, float dx, float dy, float dz) {
    if (iftFileExists(img_entry)) {
        if (!iftIsImageFile(img_entry) && !iftEndsWith(img_entry, ".csv")) {
            iftError("The image entry file \"%s\" is neither a valid Image nor a CSV file (*.csv)", "iftValidateInputs",
                     img_entry);
        }
    }
    else if (!iftDirExists(img_entry))
        iftError("Invalid Pathname for the Image Entry: \"%s\"...\nTry a valid Directory or a CSV file",
                 "iftValidateInputs", img_entry);

    if (dx < 0)
        iftError("Invalid dx: %f (< 0)", "iftValidateInputs", dx);
    if (dy < 0)
        iftError("Invalid dy: %f (< 0)", "iftValidateInputs", dy);
    if (dz < 0)
        iftError("Invalid dz: %f (< 0)", "iftValidateInputs", dz);

    puts("********************");
    printf("- Image Entry: \"%s\"\n", img_entry);
    printf("- dx: %f\n", dx);
    printf("- dy: %f\n", dy);
    printf("- dz: %f\n", dz);
    puts("********************\n");
}
/*************************************************************/


