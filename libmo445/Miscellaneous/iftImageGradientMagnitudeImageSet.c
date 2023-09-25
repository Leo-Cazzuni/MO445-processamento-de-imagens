#include "ift.h"


/************************** HEADERS **************************/
iftDict *iftGetArgs(int argc, const char *argv[]);
void iftGetInputs(const iftDict *args, iftFileSet **img_set, char **out_dir,
                  float *adj_radius);
/*************************************************************/





int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArgs(argc, argv);

    iftFileSet *img_set = NULL;
    char *out_dir = NULL;
    float adj_radius = 1.0;

    iftGetInputs(args, &img_set, &out_dir, &adj_radius);

    timer *t1 = iftTic();

    iftImage *img0 = iftReadImageByExt(img_set->files[0]->path);
    iftAdjRel *A = iftIs3DImage(img0) ? iftSpheric(adj_radius) : iftCircular(adj_radius);

    #pragma omp parallel for
    for (long f = 0; f < img_set->n; f++) {
        char *filename = iftFilename(img_set->files[f]->path, NULL);
        char *out_path = iftJoinPathnames(2, out_dir, filename);
        printf("[%ld/%ld]\n%s\n%s\n\n", f, img_set->n - 1, img_set->files[f]->path, out_path);

        iftImage *img = iftReadImageByExt(img_set->files[f]->path);
        iftImage *grad_img = iftImageGradientMagnitude(img, A);
        iftWriteImageByExt(grad_img, out_path);

        iftFree(filename);
        iftFree(out_path);
        iftDestroyImage(&img);
        iftDestroyImage(&grad_img);
    }


    puts("\nDone...");
    puts(iftFormattedTime(iftCompTime(t1, iftToc())));

    // DESTROYERS
    iftDestroyDict(&args);
    iftDestroyFileSet(&img_set);
    iftDestroyAdjRel(&A);

    return 0;
}


/************************** SOURCES **************************/
iftDict *iftGetArgs(int argc, const char *argv[]) {
    char program_description[2048] = \
        "- Extract the Gradient for an Image Set.\n";

    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--image-entry", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="CSV or Dir with the image pathnames."},
        {.short_name = "-o", .long_name = "--output-dir", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Output Dir with the image gradients (with the same image filenames)."},
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

void iftGetInputs(const iftDict *args, iftFileSet **img_set, char **out_dir,
                  float *adj_radius) {
    const char *img_entry = iftGetConstStrValFromDict("--image-entry", args);
    *img_set = iftLoadFileSetFromDirOrCSV(img_entry, 0, true);
    *out_dir = iftGetStrValFromDict("--output-dir", args);
    if (!iftDirExists(*out_dir)) {
        iftMakeDir(*out_dir);
    }

    if (iftDictContainKey("--adjacency-radius", args, NULL)) {
        *adj_radius = iftGetDblValFromDict("--adjacency-radius", args);
    }
    else { *adj_radius = 1.0; }

    puts("---------------------------");
    printf("- Image Entry: %s\n", img_entry);
    printf("- Output Gradient: %s\n", *out_dir);
    puts("---------------------------");
    printf("- Adj. Radius: %f\n", *adj_radius);
    puts("---------------------------\n");
}

