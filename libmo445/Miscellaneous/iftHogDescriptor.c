//
// Created by peixinho on 4/5/16.
//

#include <ift.h>

iftDict *iftGetArgumentsFromCmdLine(int argc, const char *argv[]) {
    iftCmdLineOpt cmd_line_opts[] = {
            {.short_name = "-i", .long_name = "--image", .has_arg=true, .arg_type=IFT_STR_TYPE,
                    .required=true, .help="Image Pathname."},
            {.short_name = "-o", .long_name = "--output-file", .has_arg=true, .arg_type=IFT_STR_TYPE,
                    .required=true, .help="Output path file."}
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

    char program_description[IFT_STR_DEFAULT_SIZE] = \
        "Computes the HOG descriptor for an image.";

    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}

int main(int argc, const char** argv) {

    iftDict* dict = iftCreateDictWithInitialSize(1400);

    iftInsertIntoDict("1", 1, dict);
    iftInsertIntoDict("3", 3, dict);
    iftInsertIntoDict("2000", 2000, dict);

    iftRemoveValFromDict("2000", dict);
    iftRemoveValFromDict("1", dict);
    iftRemoveValFromDict("3", dict);


    iftInsertIntoDict("1", 1, dict);
    iftInsertIntoDict("3", 3, dict);
    iftInsertIntoDict("2000", 2000, dict);

    iftPrintDict(dict);

    for (int i = dict->firstKey; i != IFT_NIL; i = dict->table[i].next) {
        printf("%s\n", dict->table[i].key.str_val);
    }

    fflush(stdout);

    long mem1 = iftMemoryUsed();

    iftDict* args = iftGetArgumentsFromCmdLine(argc, argv);

    iftImage* img = iftReadImageByExt(iftGetStrValFromDict("--image", args));

    iftHog* hog = iftCreateHog2D(8, 2, 1, 8);

    timer* t1 = iftTic();
    iftFeatures* feats = iftExtractHOG2D(hog, img);
    timer* t2 = iftToc();

    iftPrintFormattedTime(iftCompTime(t1, t2));

    iftWriteFeatures(feats, iftGetStrValFromDict("--output-file", args));

    iftDestroyFeatures(&feats);
    iftDestroyImage(&img);
    iftDestroyHog(&hog);
    iftDestroyDict(&args);

    long mem2 = iftMemoryUsed();

    iftVerifyMemory(mem1, mem2);

    return 0;
}