#include "ift.h"


/************************** HEADERS **************************/
iftDict *iftGetArgs(int argc, const char *argv[]);
void iftGetRequiredArgs(const iftDict *args, char **img_path, int *inf, int *sup, char **out_path);
void iftGetOptionalArgs(const iftDict *args, char **label_img_path, bool *crop_image);
/*************************************************************/





int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArgs(argc, argv);

    char *img_path = NULL;
    int inf, sup;
    char *out_path = NULL;
    char *label_img_path = NULL;
    bool crop_image = false;

    iftGetRequiredArgs(args, &img_path, &inf, &sup, &out_path);
    iftGetOptionalArgs(args, &label_img_path, &crop_image);

    timer *t1 = iftTic();

    puts("- Reading Image");
    iftImage *img = iftReadImageByExt(img_path);

    iftImage *out = NULL;
    if (label_img_path) {
        puts("- Reading Label Image");
        iftImage *label_img = iftReadImageByExt(label_img_path);
    
        puts("- Normalizing in Region");
        out = iftNormalizeInRegion(img, label_img, inf, sup);
        
        if (crop_image) {
            iftImage *aux = out;
            puts("- Cropping Normalized Image");
            out = iftMask(out, label_img);
            iftDestroyImage(&aux);
        }

        iftDestroyImage(&label_img);
    }
    else {
        puts("- Normalizing");
        out = iftNormalize(img, inf, sup);
    }

    puts("- Writing Image");
    iftWriteImageByExt(out, out_path);

    puts("\nDone...");
    puts(iftFormattedTime(iftCompTime(t1, iftToc())));
    

    // DESTROYERS
    iftDestroyDict(&args);
    iftDestroyImage(&img);

    return 0;
}


/************************** SOURCES **************************/
iftDict *iftGetArgs(int argc, const char *argv[]) {
    char program_description[2048] = \
        "- Normalize an Image to [a, b]. If a mask is passed, the normalization will happen only inside it.";

    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--input-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Input Image."},
        {.short_name = "-a", .long_name = "--inferior-bound", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=true, .help="Inferior bound of the normalization range (Suggestion: 0)."},
        {.short_name = "-b", .long_name = "--superior-bound", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=true, .help="Superior bound of the normalization range (Suggestion: 4095 for medical images of 12 bits)."},
        {.short_name = "-o", .long_name = "--output-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Output Normalized Image."},
        {.short_name = "-l", .long_name = "--label-image", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=false, .help="Label Image used for a Local Normalization."},
        {.short_name = "", .long_name = "--crop-image", .has_arg=false,
         .required=false, .help="Crop the Output Normalized Image. A mask must be passed."},
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}


void iftGetRequiredArgs(const iftDict *args, char **img_path, int *inf, int *sup, char **out_path) {
    *img_path = iftGetStrValFromDict("--input-img", args);
    *inf      = iftGetLongValFromDict("--inferior-bound", args);
    *sup      = iftGetLongValFromDict("--superior-bound", args);
    *out_path = iftGetStrValFromDict("--output-img", args);

    if (!iftFileExists(*img_path))
        iftError("Input image %s does not exist", "iftGetRequiredArgs", *img_path);
    if (*inf < 0)
        iftError("Invalid Inferior bound %d... Try >= 0", "iftGetRequiredArgs", *inf);
    if (*sup < *inf)
        iftError("Superior bound less than inferior bound (%d < %d)", "iftGetRequiredArgs", *inf, *sup);


    puts("-----------------------");
    printf("- Input Image: %s\n", *img_path);
    printf("- Normalization range: [%d, %d]\n", *inf, *sup);
    printf("- Output Image: %s\n", *out_path);
    puts("-----------------------");
}

void iftGetOptionalArgs(const iftDict *args, char **label_img_path, bool *crop_image) {
    if (iftDictContainKey("--label-image", args, NULL)) {
        *label_img_path = iftGetStrValFromDict("--label-image", args);
        printf("- Label Image: %s\n", *label_img_path);

        *crop_image = iftDictContainKey("--crop-image", args, NULL);
        printf("- Crop Image: %s\n", iftBoolAsString(*crop_image));
    }
    else {
        *label_img_path = NULL;
        *crop_image = false;
    }

    puts("-----------------------\n");
}







/*************************************************************/


