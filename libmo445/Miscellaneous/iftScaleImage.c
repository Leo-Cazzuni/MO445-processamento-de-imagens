#include "ift.h"


/************************** HEADERS **************************/
iftDict *iftGetArgs(int argc, const char *argv[]);
void iftGetRequiredArgs(const iftDict *args, char **img_path, char **out_img_path);
void iftGetOptionalArgs(const iftDict *args, const iftImage *img, float *sx, float *sy, float *sz,
                        bool *apply_nearest_neighbor_intep);
/*************************************************************/





int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArgs(argc, argv);

    char *img_path = NULL;
    char *out_img_path = NULL;
    float sx, sy, sz;
    bool apply_nearest_neighbor_intep = false;

    iftGetRequiredArgs(args, &img_path, &out_img_path);

    timer *t1 = iftTic();

    iftImage *img = iftReadImageByExt(img_path);
    iftWriteImageByExt(img, "tmp.png");

    iftGetOptionalArgs(args, img, &sx, &sy, &sz, &apply_nearest_neighbor_intep);
    printf("Final Scale Factors: %f, %f, %f\n", sx, sy, sz);

    puts("- Scaling Image");
    iftImage *scaled_img = NULL;
    if (apply_nearest_neighbor_intep)
        scaled_img = iftInterpByNearestNeighbor(img, sx, sy, sz);
    else scaled_img = iftScaleImage(img, sx, sy, sz);

    iftWriteImageByExt(scaled_img, out_img_path);

    puts("\nDone...");
    puts(iftFormattedTime(iftCompTime(t1, iftToc())));
    

    // DESTROYERS
    iftDestroyDict(&args);
    iftDestroyImage(&img);
    iftDestroyImage(&scaled_img);

    return 0;
}


/************************** SOURCES **************************/
iftDict *iftGetArgs(int argc, const char *argv[]) {
    char program_description[2048] = \
        "- Scale an image by linear or nearest neighbor interpolation.\n" \
        "- Two scale inputs are availabe for scaling:\n" \
        "* By scale factors: (option -f)\nEx:\n" \
        "-f 1.5 (a single scale for all axis)\n" \
        "-f 0.1,0.2 (scale for x- and y-axis, for 2D image)\n" \
        "-f 0.1,0.2,0.3 (scale for x-, y-, and z-axis, for 3D image)\n\n" \
        "* By passing the output image shape/domain (option -s)\n" \
        "-s 100x200 (for 2D Image)\n" \
        "-s 100x200x300 (for 3D Image)";

    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--input-image", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Input Image to be scaled."},
        {.short_name = "-o", .long_name = "--output-scaled-image", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Output Scaled Image."},
        {.short_name = "-f", .long_name = "--scale-factor", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=false, .help="Scale Factor for each scale.\nEx: -f 1.5 or -f 0.5,0.1,0.3"},
        {.short_name = "-s", .long_name = "--output-shape", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=false, .help="Ouput Shape.\nEx: -s 100x200 or -s 100x200x300"},
        {.short_name = "", .long_name = "--nearest-neighbor", .has_arg=false,
         .required=false, .help="Use the nearest neighbor interpolation instead of Linear."}
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}


void iftGetRequiredArgs(const iftDict *args, char **img_path, char **out_img_path) {
    *img_path = iftGetStrValFromDict("--input-image", args);
    *out_img_path = iftGetStrValFromDict("--output-scaled-image", args);

    puts("-----------------------");
    printf("- Image Path: \"%s\"\n", *img_path);
    printf("- Output Path: \"%s\"\n", *out_img_path);
    puts("-----------------------");
}


void iftGetOptionalArgs(const iftDict *args, const iftImage *img, float *sx, float *sy, float *sz,
                        bool *apply_nearest_neighbor_intep) {
    *sx = *sy = *sz = 1.0;

    if (iftDictContainKey("--scale-factor", args, NULL) && iftDictContainKey("--output-shape", args, NULL))
        iftError("Scale factor and Output Shape were passed. Pass only one.", "iftGetOptionalArgs");

    if (iftDictContainKey("--scale-factor", args, NULL)) {
        const char *factors_str = iftGetConstStrValFromDict("--scale-factor", args);
        if (!iftRegexMatch(factors_str, "^([0-9]+(.[0-9]+)?,?)+$"))
            iftError("Invalid Scale Factors: %s", "iftGetLabelArrayFromCmdLine", factors_str);

        iftSList *SL = iftSplitString(factors_str, ",");
        int n_elems = SL->n;

        if (n_elems >= 4)
            iftError("Number of Scales Factors passed is %d >= 4. Try pass 2 or 3 values.",
                     "iftGetOptionalArgs", n_elems);
        if (iftIs3DImage(img) && n_elems == 2)
            iftError("Only 2 Scales Factors passed but Image is 3D. Try pass 1 or 3 values.",
                     "iftGetOptionalArgs");
        if (!iftIs3DImage(img) && n_elems == 3)
            iftWarning("3 Scales Factors passed but Image is 2D. The last one will be ignored.\n",
                     "iftGetOptionalArgs");

        if (n_elems == 1)
            *sx = *sy = *sz = atof(iftRemoveSListHead(SL));
        else {
            *sx = atof(iftRemoveSListHead(SL));
            *sy = atof(iftRemoveSListHead(SL));

            if (n_elems == 3)
                *sz = atof(iftRemoveSListHead(SL));
        }
        iftDestroySList(&SL);

        printf("- Scale Factors: %f, %f", *sx, *sy);
        if (iftIs3DImage(img))
            printf(", %f", *sz);
        puts("");
    }
    else if (iftDictContainKey("--output-shape", args, NULL)) {
        const char *shape_str = iftGetConstStrValFromDict("--output-shape", args);
        if (!iftRegexMatch(shape_str, "^([0-9]+(x[0-9]+)?)+$"))
            iftError("Invalid Ouput Shape: %s", "iftGetLabelArrayFromCmdLine", shape_str);

        iftSList *SL = iftSplitString(shape_str, "x");
        int n_elems = SL->n;

        if (n_elems >= 4)
            iftError("Number of Shape Values passed is %d >= 4. Try pass 2 or 3 values.",
                     "iftGetOptionalArgs", n_elems);
        if (iftIs3DImage(img) && n_elems == 2)
            iftError("Only 2 Shape Values passed but Image is 3D. Try pass 1 or 3 values.",
                     "iftGetOptionalArgs");
        if (!iftIs3DImage(img) && n_elems == 3)
            iftWarning("3 Shape Values passed but Image is 2D. The last one will be ignored.\n",
                     "iftGetOptionalArgs");



        int output_shape[3];
        output_shape[0] = atoi(iftRemoveSListHead(SL));
        *sx = ((float) output_shape[0]) / img->xsize;
        output_shape[1] = atoi(iftRemoveSListHead(SL));
        *sy = ((float) output_shape[1]) / img->ysize;
        
        if (n_elems == 3) {
            output_shape[2] = atoi(iftRemoveSListHead(SL));
            *sz = ((float) output_shape[2]) / img->zsize;    
        }

        printf("- Shape: %dx%d", output_shape[0], output_shape[1]);
        if (iftIs3DImage(img))
            printf("x%d", output_shape[2]);
        puts("");

    }
    else iftError("A scale input must be passed. Try passing -f or -s", "iftGetOptionalArgs");

    *apply_nearest_neighbor_intep = iftDictContainKey("--nearest-neighbor", args, NULL);
    if (*apply_nearest_neighbor_intep)
        puts("- Interpolation: Nearest Neighbor");
    else puts("- Interpolation: Linear");
    puts("-----------------------\n");
}
/*************************************************************/


