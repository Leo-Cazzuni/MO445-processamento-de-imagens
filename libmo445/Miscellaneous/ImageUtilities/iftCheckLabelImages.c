/**
 * @brief It checks if the images from a directory or a CSV file are label images, that is,
 * if they have label in the range [0..n_objects]
 * @author Samuel Martins
 * @date Fev 18, 2016
 */

#include "ift.h"

/************************** HEADERS **************************/
iftDict *iftGetArguments(int argc, const char *argv[]);
void iftValidateInputs(const char *img_entry);
void iftCheckLabelImages(iftFileSet *img_files, int n_objects);
/*************************************************************/


int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArguments(argc, argv);

    const char *img_entry = iftGetConstStrValFromDict("--img-entry", args);
    // Optional arguments
    int n_objects = 0;
    if (iftDictContainKey("--num-objects", args, NULL))
        n_objects = iftGetLongValFromDict("--num-objects", args);

    iftValidateInputs(img_entry);

    if (iftRegexMatch(img_entry, "^.+\\.(pgm|png|scn)$")) {
        iftImage *label_img = iftReadImageByExt(img_entry);

        if (n_objects == 0)
            n_objects = iftMaximumValue(label_img);
        
        puts("--------------------");
        printf("Label Image: \"%s\"\n", img_entry);
        printf("Number of Objects: %d\n\n", n_objects);
        puts("--------------------\n");

        if (iftIsLabelImage(label_img, n_objects))
            printf("- TRUE - It is a Label Image with %d objects!\n\n", n_objects);
        else printf("- FALSE - It is NOT a Label Image with %d objects!\n\n", n_objects);

        iftDestroyImage(&label_img);
    }
    else {
        puts("--------------------");
        printf("Label Image Entry: \"%s\"\n", img_entry);
        iftFileSet *img_files = iftLoadFileSetFromDirOrCSV(img_entry, 0, false);
        iftCheckLabelImages(img_files, n_objects);        
        puts("--------------------");

        iftDestroyFileSet(&img_files);
    }


    iftDestroyDict(&args);

    return 0;
}



/************************** SOURCES **************************/
iftDict *iftGetArguments(int argc, const char *argv[]) {
    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--img-entry", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Label Image (PGM, PNG, SCN) OR Label Image Directory OR a CSV file with the label image pathnames"},
        {.short_name = "-n", .long_name = "--num-objects", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=false, .help="Number of Objects of the input labeled images (except the background)."}
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);
    
    char program_description[2048] = \
        "This program checks if a given image (.pgm or .scn) or images from a directory or a CSV file are label images with the same number of objects, " \
        "that is, if they have labels in the range [0..num_objects]\n" \
        "If the number of objects is not passed, this one will be the maximum voxel value from the " \
        "first image.";
    
    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}


void iftValidateInputs(const char *img_entry) {    
    if (iftFileExists(img_entry)) {
        char *lower_img_entry = iftLowerString(img_entry);
        if (!iftRegexMatch(lower_img_entry, "^.+\\.(pgm|png|scn|csv)$"))
            iftError("File extension should be .pgm or .scn or .csv, but it is %s",
                     "iftValidateInputs", iftFileExt(img_entry));
        free(lower_img_entry);
    }
    else if (!iftDirExists(img_entry))
        iftError("Invalid Pathname for the Image Entry: \"%s\"... Try a valid label image or a directory " \
                 "of label images or a CSV file",
                 "iftValidateInputs", img_entry);
}


void iftCheckLabelImages(iftFileSet *img_files, int n_objects) {
    if (img_files->n > 0) {
        if (n_objects <= 0) {
            iftImage *label_img = iftReadImage(img_files->files[0]->path);
            n_objects           = iftMaximumValue(label_img);
            iftDestroyImage(&label_img);
        }

        printf("Number of Objects: %d\n", n_objects);
        puts("--------------------\n");

        // #pragma omp parallel for
        for (size_t i = 0; i < img_files->n; i++) {
            puts("----------------");
            printf("- %lu/%lu: %s", i+1, img_files->n, img_files->files[i]->path);
            iftImage *label_img = NULL;

            if (iftFileExists(img_files->files[i]->path)) {
                if (iftRegexMatch(img_files->files[i]->path, "^.+\\.(pgm|png|scn)$")) {
                    label_img = iftReadImage(img_files->files[i]->path);
                    
                    if (!iftIsLabelImage(label_img, n_objects)) {
                        printf("Image \"%s\" is not a label image OR it is missing some label\n",
                                 img_files->files[i]->path);
                    }
                    else puts(" - OK!");
                    iftDestroyImage(&label_img);   
                }
                else {
                    puts("");
                    iftWarning("Image extension should be .pgm or .scn, but it is \"%s\"", "iftCheckLabelImages",
                              iftFileExt(img_files->files[i]->path));
                    puts("");
                }
            }
            else {
                iftError("File \"%s\" does not exist", "iftCheckLabelImages",
                         img_files->files[i]->path);
            }
            puts("----------------");
        }
    }
    else puts("Number of Files is 0");
}
/*************************************************************/















