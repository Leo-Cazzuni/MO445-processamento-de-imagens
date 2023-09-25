/**
 * @file
 * @brief Extracts all Objects from an Image, saving one Image for each one.
 * @note See the source code in @ref iftExtractAllObjects.c
 * 
 * @example iftExtractAllObjects.c
 * @brief Extracts all Objects from an Image, saving one Image for each one.
 * @author Samuel Martins
 * @date Mar 4, 2016
 */

#include "ift.h"


/************************** HEADERS **************************/
iftDict *iftGetArgumentsFromCmdLine(int argc, const char *argv[]);
void iftValidateInputs(const char *label_img_path);
/*************************************************************/



int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArgumentsFromCmdLine(argc, argv);

    bool binarize_labels       = iftDictContainKey("-b", args, NULL);
    const char *label_img_path = iftGetConstStrValFromDict("-i", args);
    const char *out_basename   = iftGetConstStrValFromDict("-o", args);

    iftValidateInputs(label_img_path);

    iftImage *label_img = iftReadImageByExt(label_img_path);
    iftIntArray *labels = iftGetObjectLabels(label_img);


    puts("-----------------------");
    printf("- Input Label Image: \"%s\"\n", label_img_path);
    printf("- Number of Labels (Objects): %lu\n", labels->n);
    printf("- Output Basename: \"%s\"\n", out_basename);
    puts("-----------------------\n");
    printf("- Input Image Size (x, y, z): (%d, %d, %d)\n\n", label_img->xsize, label_img->ysize, label_img->zsize);


    iftVoxel *gcs        = NULL;
    iftBoundingBox *mbbs = iftMinLabelsBoundingBox(label_img, labels, &gcs);
    const char *ext      = iftFileExt(label_img_path);

    for (int o = 0; o < labels->n; o++) {
        iftImage *out_img = iftExtractObjectInsideROI(label_img, mbbs[o], labels->val[o]);

        if (binarize_labels) {
            iftImage *bin = iftThreshold(out_img, o, o, 1);
            iftDestroyImage(&out_img);
            out_img = bin;
        }

        if (iftDictContainKey("--normalize", args, NULL)) {
            iftImage *norm_img = iftLinearStretch(out_img, iftMinimumValue(out_img), iftMaximumValue(out_img), 0, 255);
            iftDestroyImage(&out_img);
            out_img = norm_img;
        }
        
        char out_img_path[256];
        sprintf(out_img_path, "%s_%02d%s", out_basename, labels->val[o], ext);

        printf("- %s\n", out_img_path);
        printf("- Output Image Size (x, y, z): (%d, %d, %d)\n", out_img->xsize, out_img->ysize, out_img->zsize);
        printf("- Min. Bound. Box: begin: (%d, %d, %d), end (%d, %d, %d)\n",
                mbbs[o].begin.x, mbbs[o].begin.y, mbbs[o].begin.z, mbbs[o].end.x, mbbs[o].end.y, mbbs[o].end.z);
        printf("- Out. Image Geometric Center in the Input Image (x, y, z): (%d, %d, %d)\n\n", gcs[o].x, gcs[o].y, gcs[o].z);

        iftWriteImageByExt(out_img, out_img_path);    
        iftDestroyImage(&out_img);
    }

    // DESTROYERS
    iftDestroyDict(&args);
    iftDestroyImage(&label_img);
    iftDestroyIntArray(&labels);
    free(mbbs);

    return 0;
}



/************************** SOURCES **************************/
iftDict *iftGetArgumentsFromCmdLine(int argc, const char *argv[]) {
    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--input-label-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Label Image with labels (values) from [0, n_labels], where the object "\
                               "will be extracted."},
        {.short_name = "-o", .long_name = "--output-basename", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Output Basename used to save the object images"},
        {.short_name = "-n", .long_name = "--normalize", .has_arg=false,
         .required=false, .help="Normalize the Output Image to 8 bits [0,255]"},
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);
    
    
    char program_description[2048] = \
        "This program extracts all objects (Labels) from a Label Image with labels in [0, num-objects].\n" \
        "All objects are saved with a name that uses a given output basename, their label, and the file extension " \
        "from the Input Label Image.\n"
        "E.g: Basename = \"out_img\"\n" \
        "- out_img_1.scn\n" \
        "- out_img_2.scn";
    
    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}


void iftValidateInputs(const char *label_img_path) {
    if (!iftIsImageFile(label_img_path))
        iftError("Invalid Label Image: \"%s\"", "iftValidateInputs", label_img_path);
}




