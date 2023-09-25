#include "ift.h"


iftDict *iftGetArgs(int argc, const char *argv[]);
void iftGetRequiredArgs(const iftDict *args, char **img_path, char **out_path);
void iftGetOptionalArgs(const iftDict *args, float *radius, float *sigma);


int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArgs(argc, argv);

    char *img_path = NULL;
    char *out_path = NULL;
    float radius, sigma;

    iftGetRequiredArgs(args, &img_path, &out_path);
    iftGetOptionalArgs(args, &radius, &sigma);

    timer *t1 = iftTic();

    iftImage *img = iftReadImageByExt(img_path);

    iftAdjRel *A = NULL;
    if (iftIs3DImage(img))
        A = iftSpheric(radius);
    else A = iftCircular(radius);

    puts("- Smoothing");
    iftImage *out_img = iftSmoothImage(img, A, sigma);
    iftWriteImageByExt(out_img, out_path);


    puts("\nDone...");
    puts(iftFormattedTime(iftCompTime(t1, iftToc())));



    iftDestroyDict(&args);
    iftDestroyImage(&img);
    iftFree(img_path);
    iftFree(out_path);

    return 0;
}



/************************** SOURCES **************************/
iftDict *iftGetArgs(int argc, const char *argv[]) {
    char program_description[2048] = \
        "- Smooth an Image by Gaussian Filter.";


    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--input-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Input image to be smoothed."},
        {.short_name = "-o", .long_name = "--output-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Output Smoothed Image."},
        {.short_name = "-r", .long_name = "--adj-radius", .has_arg=true, .arg_type=IFT_DBL_TYPE,
         .required=false, .help="Adjacency Radius.\nDefault: 1.0"},
        {.short_name = "-s", .long_name = "--sigma", .has_arg=true, .arg_type=IFT_DBL_TYPE,
         .required=false, .help="Sigma parameter.\nDefault: 15.0"},
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}



void iftGetRequiredArgs(const iftDict *args, char **img_path, char **out_path) {
    *img_path = iftGetStrValFromDict("--input-img", args);
    *out_path = iftGetStrValFromDict("--output-img", args);

    puts("-----------------------");
    printf("- Input Image: \"%s\"\n", *img_path);
    printf("- Output Image: \"%s\"\n", *out_path);
    puts("-----------------------");
}



void iftGetOptionalArgs(const iftDict *args, float *radius, float *sigma) {
    if (iftDictContainKey("--adj-radius", args, NULL))
        *radius = iftGetDblValFromDict("--adj-radius", args);
    else *radius = 1.0;

    if (iftDictContainKey("--sigma", args, NULL))
        *sigma = iftGetDblValFromDict("--sigma", args);
    else *sigma = 15.0;

    printf("- Adjacency Radius: %f\n", *radius);
    printf("- Sigma: %f\n", *sigma);
    puts("-----------------------\n");
}








