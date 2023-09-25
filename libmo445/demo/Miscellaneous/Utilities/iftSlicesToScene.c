#include "ift.h"

iftDict *iftGetArguments(int argc, const char *argv[]);

int main(int argc, const char *argv[])
{
    iftImage  *img;
    iftDict *args = NULL;
    iftFileSet *files = NULL;

    /*--------------------------------------------------------*/

    int MemDinInicial, MemDinFinal;
    MemDinInicial = iftMemoryUsed();

    /*--------------------------------------------------------*/

    args = iftGetArguments(argc, argv);

    files = iftLoadFileSetFromDirBySuffix(iftGetConstStrValFromDict("--input-dir", args), iftGetConstStrValFromDict("--suffix", args));
    iftSortFileSet(files);

    img = iftReadSlices(files);

    iftWriteImage(img, iftGetConstStrValFromDict("--output", args));

    iftDestroyFileSet(&files);
    iftDestroyDict(&args);

    /*--------------------------------------------------------*/

    MemDinFinal = iftMemoryUsed();
    iftVerifyMemory(MemDinInicial, MemDinFinal);

    /*--------------------------------------------------------*/

    return(0);

}



iftDict *iftGetArguments(int argc, const char *argv[]) {
    char program_description[2048] = "This is an only a demo program of how to use the Command Line Parser.\n" \
                                     "No image operation is performed actually.";

    // this array of options, n_opts, and the program description could be defined in the main or as a global variables
    iftCmdLineOpt cmd_line_opts[] = {
            {.short_name = "-i", .long_name = "--input-dir", .has_arg=true, .arg_type=IFT_STR_TYPE,
                    .required=true, .help="Input directory"},
            {.short_name = "-s", .long_name = "--suffix", .has_arg=true, .arg_type=IFT_STR_TYPE,
                    .required=true, .help="Suffix of input slices"},
            {.short_name = "-o", .long_name = "--output", .has_arg=true, .arg_type=IFT_STR_TYPE,
                    .required=true, .help="Output scene image"}
    };
    // we could simply assign the number 5 in this case
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);


    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}
