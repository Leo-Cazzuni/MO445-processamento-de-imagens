#include "ift.h"


/************************** HEADERS **************************/
iftDict *iftGetArgs(int argc, const char *argv[]);
void iftGetRequiredArgs(const iftDict *args, char **img_path, char **mask_path, float *stride,
                        int *patch_size, char **out_dir);
void iftGetOptionalArgs(const iftDict *args, int *n_grid_samples);
/*************************************************************/

int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArgs(argc, argv);

    char *img_path = NULL;
    char *mask_path = NULL;
    float stride;
    int patch_size;
    char *out_dir = NULL;
    int n_grid_samples = 0;
    long total_samples = 0;

    iftGetRequiredArgs(args, &img_path, &mask_path, &stride, &patch_size, &out_dir);
    iftGetOptionalArgs(args, &n_grid_samples);


    timer *t1 = iftTic();

    puts("- Reading Image");
    iftImage *img = iftReadImageByExt(img_path);
    puts("- Reading Mask");
    iftImage *mask = iftReadImageByExt(mask_path);
    puts("- Labeling Mask");
    iftAdjRel *A = NULL;
    iftImage *comp = NULL;

    if (iftIs3DImage(img))
        A = iftSpheric(sqrt(3.));
    else
        A = iftCircular(sqrt(2.));

    if (iftMaximumValue(mask) >= 1)
        comp = iftLabelComp(mask,A);
    else
        iftError("Mask is empty","main");
    int n_comp = iftMaximumValue(comp);
    iftDestroyAdjRel(&A);

    const char *img_ext = iftFileExt(img_path);
    char *img_filename = iftFilename(img_path, img_ext);

    iftImage *img_frame = iftAddFrame(img, patch_size, 0);
    iftImage *label_img = iftAddFrame(mask,patch_size,0);
    iftBoundingBoxArray **patches = (iftBoundingBoxArray **)calloc(sizeof(iftBoundingBoxArray*),n_comp);

    for (int c = 0; c < n_comp; c++) {

        iftImage *obj = iftExtractObject(comp,c+1);

        printf("- Grid Sampling on Object %02d\r",c+1);
        fflush(stdout);
        iftImage *obj_frame = iftAddFrame(obj,patch_size,0);
        iftIntArray *grid = iftGridSamplingOnMask(obj_frame, stride, -1, n_grid_samples);
        iftDestroyImage(&obj_frame);
        total_samples += grid->n;

        patches[c] = iftBoundingBoxesAroundVoxels(img_frame, grid, patch_size);

        #pragma omp parallel for
        for (int i = 0; i < patches[c]->n; i++) {
            iftBoundingBox bb = patches[c]->val[i];
            iftFillBoundingBoxInImage(label_img, bb, i + 1);

            char patch_filename[512];
            sprintf(patch_filename, "%s_patch_%03d-%03d-%03d_%03d-%03d-%03d%s", img_filename, bb.begin.x, bb.begin.y,
                    bb.begin.z,
                    bb.end.x, bb.end.y, bb.end.z, img_ext);
            char *patch_path = iftJoinPathnames(2, out_dir, patch_filename);

            iftImage *patch_img = iftExtractROI(img_frame, bb);
            iftWriteImageByExt(patch_img, patch_path);

            iftDestroyImage(&patch_img);
            iftFree(patch_path);
        }

        iftDestroyImage(&obj);
        iftDestroyIntArray(&grid);
    }
    iftDestroyImage(&img_frame);
    printf("\ngrid->n: %ld\n", total_samples);
    char out_label_img_filename[512];
    sprintf(out_label_img_filename,"%s_label_img%s",img_filename,img_ext);
    char *out_label_img_path = iftJoinPathnames(2, out_dir, out_label_img_filename);
    iftWriteImageByExt(label_img, out_label_img_path);

    iftDestroyImage(&comp);

    puts("- Saving Output CSV with the Bounding Box coordinates");
    iftBoundingBoxArray *final_patches = iftCreateBoundingBoxArray(total_samples);
    int pos = 0;
    for (int c = 0; c < n_comp; c++) {
        for (int b = 0; b < patches[c]->n; b++){
            final_patches->val[pos].begin.x = patches[c]->val[b].begin.x;
            final_patches->val[pos].begin.y = patches[c]->val[b].begin.y;
            final_patches->val[pos].begin.z = patches[c]->val[b].begin.z;
            pos++;
        }
        iftDestroyBoundingBoxArray(&(patches[c]));
    }
    char *out_csv_path = iftJoinPathnames(2, out_dir, iftConcatStrings(2, img_filename, ".csv"));
    iftWriteBoundingBoxArray(final_patches, out_csv_path);
    free(patches);


    puts("\nDone...");
    puts(iftFormattedTime(iftCompTime(t1, iftToc())));
    

    // DESTROYERS
    iftDestroyDict(&args);
    iftDestroyImage(&img);
    iftDestroyImage(&mask);
    //iftDestroyBoundingBoxArray(&patches);
    iftDestroyImage(&label_img);

    return 0;
}


/************************** SOURCES **************************/
iftDict *iftGetArgs(int argc, const char *argv[]) {
    char program_description[2048] = \
        "- Extract patches from an input image from a Grid Sampling on a Mask.\n" \
        "- The patch images will be stored in an output folder with the following filename convention:\n" \
        "FilenameInputImage_patch_x0-y0-z0_x1-y1-z1.ExtensionInputImage\n" \
        "where (x0, y0, z0) and (x1, y1, z1) are the topmost-left and bottommost-right coordinates " \
        "of the patch, respectively.\n" \
        "- A label image where each patch is filled with a label is also saved on the output dir with the filename: label_image\n" \
        "- A CSV file with the coordinates of all patches is also saved on output dir with the same filename of the input image.\nEx:\n" \
        "input image: ~/images/image_example.png, output dir: ./out\n" \
        "./out/image_example_patch_0-0-0_9-9-0.png\n" \
        "./out/image_example_patch_10-0-0_19-19-0.png\n" \
        "...\n" \
        "./out/label_image.png\n" \
        "./out/image_example.csv";

    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--input-image", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Input Image to be sampled."},
        {.short_name = "-m", .long_name = "--mask", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Binary Mask which where the sampling will be applied."},
        {.short_name = "-s", .long_name = "--stride", .has_arg=true, .arg_type=IFT_DBL_TYPE,
         .required=true, .help="Stride (radius) of the Grid Sampling (> 0)."},
        {.short_name = "-p", .long_name = "--patch-size", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=true, .help="(Integer) Size of the resulting patches (> 0)."},
        {.short_name = "-o", .long_name = "--output-directory", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Output Directory where the patches will be saved."},
        {.short_name = "-n", .long_name = "--num-grid-samples", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=false, .help="Number of samples randomly selected from the grid. Default 0: all samples are considered."}
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}





void iftGetRequiredArgs(const iftDict *args, char **img_path, char **mask_path, float *stride,
                        int *patch_size, char **out_dir) {
    *img_path = iftGetStrValFromDict("--input-image", args);
    *mask_path = iftGetStrValFromDict("--mask", args);
    *stride = iftGetDblValFromDict("--stride", args);
    *patch_size = iftGetLongValFromDict("--patch-size", args);
    *out_dir = iftGetStrValFromDict("--output-directory", args);

    if (!iftDirExists(*out_dir))
        iftMakeDir(*out_dir);

    puts("-----------------------");
    printf("- Input Image: %s\n", *img_path);
    printf("- Mask: %s\n", *mask_path);
    printf("- Radius: %f\n", *stride);
    printf("- Patch Size: %d\n", *patch_size);
    printf("- Output Dir: %s\n", *out_dir);
    puts("-----------------------");
}


void iftGetOptionalArgs(const iftDict *args, int *n_grid_samples) {
    if (iftDictContainKey("--num-grid-samples", args, NULL))
        *n_grid_samples = iftGetLongValFromDict("--num-grid-samples", args);
    else *n_grid_samples = 0;

    printf("- Number of Grid Samples: %d\n", *n_grid_samples);
    puts("-----------------------\n");
}






/*************************************************************/


