/**
 * @file
 * @brief Converts a Minc Image (.mnc) to Scene.
 *
 * @example iftMinc2Scn.c
 * @brief Converts a Minc Image (.mnc) to Scene.
 * @author Samuka Martins
 * @date Jul 6, 2017
 */

#include "ift.h"


/************************** HEADERS **************************/
iftDict *iftGetArgs(int argc, const char *argv[]);
void iftGetRequiredArgs(const iftDict *args, char **minc_path, char **scn_path);
void iftValidateRequiredArgs(const char *minc_path, const char *scn_path);
/*************************************************************/










int main(int argc, const char *argv[]) {
    iftDict *args   = iftGetArgs(argc, argv);
    char *minc_path = NULL;
    char *scn_path  = NULL;

    iftGetRequiredArgs(args, &minc_path, &scn_path);

    char *tmp_nii_path = iftMakeTempPathname("tmp_", ".nii", NULL);
    iftRunProgram("mnc2nii", "-nii -short %s %s", minc_path, tmp_nii_path);

    iftImage *nii = iftReadImageByExt(tmp_nii_path);
    iftWriteImageByExt(nii, scn_path);

    puts("Done...");

    iftDestroyDict(&args);
    iftDestroyImage(&nii);
    iftFree(minc_path);
    iftFree(scn_path);
    iftRemoveFile(tmp_nii_path);
    iftFree(tmp_nii_path);

    return 0;
}



/************************** SOURCES **************************/

iftDict *iftGetArgs(int argc, const char *argv[]) {
    char program_description[2048] = \
        "- Converts a Minc Image (.mnc) to Scene (.scn)\n" \
        "- REQUIRED: minc-tools (https://bic-mni.github.io)";


    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--input-minc-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Input Minc Image."},
        {.short_name = "-o", .long_name = "--output-scn-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Output Scene Image."}
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}

void iftGetRequiredArgs(const iftDict *args, char **minc_path, char **scn_path) {
    *minc_path = iftGetStrValFromDict("--input-minc-img", args);
    *scn_path  = iftGetStrValFromDict("--output-scn-img", args);

    iftValidateRequiredArgs(*minc_path, *scn_path);
}


void iftValidateRequiredArgs(const char *minc_path, const char *scn_path) {
    char *ext = iftLowerString(iftFileExt(minc_path));

    if (!iftCompareStrings(ext, ".mnc"))
        iftError("Invalid Input Minc Image extension: \"%s\"... Try .mnc", "iftGetRequiredArgs", ext);
    iftFree(ext);

    ext = iftLowerString(iftFileExt(scn_path));
    if (!iftCompareStrings(ext, ".scn"))
        iftError("Invalid Output Scene Image extension: \"%s\"... Try .scn", "iftGetRequiredArgs", ext);
    iftFree(ext);

    puts("-----------------------");
    printf("- Input Minc Image: \"%s\"\n", minc_path);
    printf("- Ouput Scene Image: \"%s\"\n", scn_path);
    puts("-----------------------\n");
}





