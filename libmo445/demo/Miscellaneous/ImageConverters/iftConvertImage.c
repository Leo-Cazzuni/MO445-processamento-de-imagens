#include "ift.h"


iftDict *iftGetArgs(int argc, const char *argv[]);


int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArgs(argc, argv);


    const char *img_path = iftGetConstStrValFromDict("--input-img", args);
    puts(img_path);
    const char *out_path = iftGetConstStrValFromDict("--output-img", args);

    char *parent_dir = iftParentDir(out_path);
    if (!iftDirExists(parent_dir))
        iftMakeDir(parent_dir);
    iftFree(parent_dir);

    iftImage *img = iftReadImageByExt(img_path); 
    iftWriteImageByExt(img, out_path);
    puts("Done...");

    iftDestroyDict(&args);
    iftDestroyImage(&img);

    return 0;
}



/************************** SOURCES **************************/
iftDict *iftGetArgs(int argc, const char *argv[]) {
    char program_description[2048] = \
        "- Convert an Image to another Image with a different format.";


    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--input-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Input image to be converted."},
        {.short_name = "-o", .long_name = "--output-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Output Converted Image with a different format."},
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}


