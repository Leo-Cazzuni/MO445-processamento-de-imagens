/**
 * @file
 * @brief Masks an Input Image.
 * @note See the source code in @ref iftMask.c
 *
 * @example iftMask.c
 * @brief Masks an Input Image.
 * @author Samuel Martins
 * @date Jul 12, 2016
 */

#include "ift.h"


/************************** HEADERS **************************/
iftDict *iftGetArgs(int argc, const char *argv[]);
void iftGetRequiredArgs(const iftDict *args, char **img_path, char **mask_img_path, char **out_img_path);
void iftGetOptionalArgs(const iftDict *args, iftIntArray **labels);
void iftValidateRequiredArgs(const char *img_path, const char *mask_img_path, const char *out_img_path);
iftIntArray *iftGetLabelArrayFromCmdLine(const iftDict *args);
/*************************************************************/


int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArgs(argc, argv);
    
    // mandatory args
    char *img_path      = NULL;
    char *mask_img_path = NULL;
    char *out_img_path  = NULL;
    iftIntArray *labels = NULL;

    iftGetRequiredArgs(args, &img_path, &mask_img_path, &out_img_path);
    iftGetOptionalArgs(args, &labels);

    timer *t1 = iftTic();

    iftImage *img      = iftReadImageByExt(img_path);
    iftImage *mask_img = iftReadImageByExt(mask_img_path);

    puts("- Getting only a set of labels");
    if (labels != NULL) {
        iftBMap *bmap = iftCreateBMap(labels->val[labels->n-1]+1);
        for (int o = 0; o < labels->n; o++)
            iftBMapSet1(bmap, labels->val[o]);

        iftDict *labels_dict = iftIntArrayToDict(labels->val, labels->n);
        for (int p = 0; p < mask_img->n; p++)
            mask_img->val[p] *= iftBMapValue(bmap, mask_img->val[p]);
            // mask_img->val[p] *= iftDictContainKey(mask_img->val[p], labels_dict, NULL);
        iftDestroyDict(&labels_dict);
    }

    puts("- Masking Image");
    iftImage *out_img = iftMask(img, mask_img);
    iftWriteImageByExt(out_img, out_img_path);

    puts("\nDone...");
    puts(iftFormattedTime(iftCompTime(t1, iftToc())));


    // DESTROYERS
    iftDestroyDict(&args);
    iftDestroyImage(&img);
    iftDestroyImage(&mask_img);
    iftDestroyImage(&out_img);

    return 0;
}



/************************** SOURCES **************************/
iftDict *iftGetArgs(int argc, const char *argv[]) {
    char program_description[2048] = \
        "Masks an Input Image. We can select the labels for masking.\n" \
        "PS: The Mask can be a Binary Image or a Multi-Labeled Image.";

    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--input-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Input Image to be masked."},
        {.short_name = "-m", .long_name = "--mask-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Mask Image."},
        {.short_name = "-o", .long_name = "--output-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Pathname from the Output Image."},
        {.short_name = "-j", .long_name = "--obj-labels", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=false, .help="Target Objects for Masking. Ex: 1,2,5 (3 objects)\nDefault: All labels from the mask."}
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}


void iftGetRequiredArgs(const iftDict *args, char **img_path, char **mask_img_path, char **out_img_path) {
    *img_path      = iftGetStrValFromDict("--input-img", args);
    *mask_img_path = iftGetStrValFromDict("--mask-img", args);
    *out_img_path  = iftGetStrValFromDict("--output-img", args);

    puts("-----------------------");
    printf("- Input Image: \"%s\"\n", *img_path);
    printf("- Mask Image: \"%s\"\n", *mask_img_path);
    printf("- Output Image: \"%s\"\n", *out_img_path);
    puts("-----------------------");
}

void iftValidateRequiredArgs(const char *img_path, const char *mask_img_path, const char *out_img_path) {
    if (!iftIsImageFile(img_path))
        iftError("Invalid Input Image: \"%s\"", "iftValidateRequiredArgs", img_path);

    if (!iftIsImageFile(mask_img_path))
        iftError("Invalid Input Mask Image: \"%s\"", "iftValidateRequiredArgs", mask_img_path);

    if (!iftIsImagePathnameValid(out_img_path)) {
        iftError("Invalid Output Image's Pathname: \"%s\"", "iftValidateRequiredArgs", out_img_path);
    }

    char *parent_dir = iftParentDir(out_img_path);
    if (!iftDirExists(parent_dir)) {
        iftMakeDir(parent_dir);
    }
    iftFree(parent_dir);
}

void iftGetOptionalArgs(const iftDict *args, iftIntArray **labels) {
    if (iftDictContainKey("--obj-labels", args, NULL)) {
        *labels = iftGetLabelArrayFromCmdLine(args);
        printf("- Labels:");
        iftPrintIntArray((*labels)->val, (*labels)->n);
    } else *labels = NULL;
}



iftIntArray *iftGetLabelArrayFromCmdLine(const iftDict *args) {
    iftIntArray *labels = NULL;

    const char *labels_str = iftGetConstStrValFromDict("--obj-labels", args);

    if (!iftRegexMatch(labels_str, "^([0-9]+,?)+$"))
        iftError("Invalid Labels: %s", "iftGetLabelArrayFromCmdLine", labels_str);
    
    iftSList *SL = iftSplitString(labels_str, ",");
    labels       = iftCreateIntArray(SL->n);
    
    iftSNode *snode = SL->head;
    for (size_t o = 0; o < SL->n; o++) {
        labels->val[o] = atoi(snode->elem);
        snode = snode->next;
    }
    iftDestroySList(&SL);

    return labels;
}






