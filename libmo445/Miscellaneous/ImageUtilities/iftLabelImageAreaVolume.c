/**
 * @file
 * @brief Computes the area/volume from each Object from an Label Image.
 * @note See the source code in @ref iftLabelImageAreaVolume.c
 *
 * @example iftLabelImageAreaVolume.c
 * Computes the area/volume from each Object from an Label Image.
 * @author Samuel Martins
 * @date Mar 5, 2016
 */

#include "ift.h"


/************************** HEADERS **************************/
iftDict *iftGetArgumentsFromCmdLine(int argc, const char *argv[]);
/*************************************************************/


int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArgumentsFromCmdLine(argc, argv);
    const char *label_img_path = iftGetConstStrValFromDict("-i", args);     

    if (!iftIsImageFile(label_img_path))
        iftError("Invalid Label Image: \"%s\"", "main", label_img_path);
    puts("********************");
    printf("- Label Image Path: \"%s\"\n", label_img_path);
    puts("********************\n");

    iftImage *label_img = iftReadImageByExt(label_img_path);

    // A target object was passed
    if (iftDictContainKey("--obj-label", args, NULL)) {
        int obj_label   = iftGetLongValFromDict("--obj-label", args);
        int n_obj_spels = iftCountObjectSpels(label_img, obj_label);
        float vol       = iftAreaVolumeOfObject(label_img, obj_label);

        printf("Object: %d\n", obj_label);
        printf("- Num of Spels: %d\n", n_obj_spels);
        if (iftIs3DImage(label_img))
            printf("- Volume: %.2f mm³\n", vol);
        else printf("- Area: %.2f mm²\n", vol);
    }
    // find the area/volume for each object (label)
    else {
        iftIntArray *n_spels = iftCountLabelSpels(label_img);
        iftFloatArray *vols  = iftAreaVolumeOfLabels(label_img);
        int n_labels         = n_spels->n - 1; // excludes the background (position 0)

        for (int o = 1; o <= n_labels; o++) {
            printf("Object: %d\n", o);
            printf("- Num of Spels: %d\n", n_spels->val[o]);
            if (iftIs3DImage(label_img))
                printf("- Volume: %.2f mm³\n", vols->val[o]);
            else printf("- Area: %.2f mm²\n", vols->val[o]);
        }

        iftDestroyIntArray(&n_spels);
        iftDestroyFloatArray(&vols);
    }

    // DESTROYERS
    iftDestroyDict(&args);
    iftDestroyImage(&label_img);

    return 0;
}



/************************** SOURCES **************************/
iftDict *iftGetArgumentsFromCmdLine(int argc, const char *argv[]) {
    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--label--img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Image where the ROI will be extracted."},
        {.short_name = "-l", .long_name = "--obj-label", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=false, .help="The Label of a specific Object. Default: Computes the volumes for all objects"}
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);
    
    
    char program_description[2048] = \
        "This program computes the area/volume of each Object from a Label Image. \n" \
        "If a given Object Label is specified, then the program only computes its area/volume.";
    
    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}
/*************************************************************/

