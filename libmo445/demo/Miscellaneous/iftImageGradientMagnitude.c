#include "ift.h"


/************************** HEADERS **************************/
iftDict *iftGetArgs(int argc, const char *argv[]);
void iftGetInputs(const iftDict *args, char **img_path, char **out_path,
                  float *adj_radius);
/*************************************************************/





int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArgs(argc, argv);

    char *img_path = NULL;
    char *out_path = NULL;
    float adj_radius = 1.0;

    iftGetInputs(args, &img_path, &out_path, &adj_radius);

    timer *t1 = iftTic();

    iftImage *img = iftReadImageByExt(img_path);
    iftAdjRel *A = iftIs3DImage(img) ? iftSpheric(adj_radius) : iftCircular(adj_radius);

    iftImage *grad_img = iftImageGradientMagnitude(img, A);
    iftWriteImageByExt(grad_img, out_path);

    puts("\nDone...");
    puts(iftFormattedTime(iftCompTime(t1, iftToc())));

    // DESTROYERS
    iftDestroyDict(&args);
    iftDestroyImage(&img);
    iftDestroyAdjRel(&A);
    iftDestroyImage(&grad_img);

    return 0;
}


/************************** SOURCES **************************/
iftDict *iftGetArgs(int argc, const char *argv[]) {
    char program_description[2048] = \
        "- Extract the Image Gradient.\n";

    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--image", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Input Image."},
        {.short_name = "-o", .long_name = "--output-gradient", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Output Image Gradient."},
        {.short_name = "-r", .long_name = "--adjacency-radius", .has_arg=true, .arg_type=IFT_DBL_TYPE,
         .required=false, .help="Adjacency Radius to compute the Gradient. Default: 1.0"},  
    };

    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}

void iftGetInputs(const iftDict *args, char **img_path, char **out_path,
                  float *adj_radius) {
    *img_path = iftGetStrValFromDict("--image", args);
    *out_path = iftGetStrValFromDict("--output-gradient", args);

    if (iftDictContainKey("--adjacency-radius", args, NULL)) {
        *adj_radius = iftGetDblValFromDict("--adjacency-radius", args);
    }
    else { *adj_radius = 1.0; }

    puts("---------------------------");
    printf("- Image: %s\n", *img_path);
    printf("- Output Gradient: %s\n", *out_path);
    puts("---------------------------");
    printf("- Adj. Radius: %f\n", *adj_radius);
    puts("---------------------------\n");
}

