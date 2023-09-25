/**
 * @file
 * @brief This program loads a 3D image file and compresses it into a second file with .zscn or .scn.gz file extension,
 * @note See the source code in @ref iftTestCompression.c
 *
 * @example iftTestCompression.c
 * @brief This program loads a 3D image file and compresses it into a second file with .zscn or .scn.gz file extension,
 * @author Thiago Vallin Spina
 * @date Mar 31, 2016
 */
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "ift.h"
#include "zlib.h"

/**
 * It does all setup of the command line parser, it parses the command line and get all its arguments.
 * This avoids that such amount of code is in the main.
 */
iftDict *iftGetArguments(int argc, const char *argv[]);

bool iftCheckImageIntegrity(const iftImage *img, const iftImage *decompressed);

int main(int argc, const char *argv[])
{
    iftDict *args = NULL;
    iftImage *img = NULL;
    iftImage *decompressed = NULL;
    const char *input_file = NULL;
    const char *output_file = NULL;
    char *bname, command[IFT_STR_DEFAULT_SIZE];
    bool equal;

    args = iftGetArguments(argc, argv);

    input_file = iftGetConstStrValFromDict("--input-file", args);
    output_file = iftGetConstStrValFromDict("--output-file", args);

    if(!iftCompareStrings(iftFileExt(output_file), ".scn.gz") && !iftCompareStrings(iftFileExt(output_file), ".zscn"))
        iftError("Invalid output filename extension! Please choose .zscn or .scn.gz", "main");


    img = iftReadImageByExt(input_file);
    // Compressing the file
    iftWriteImageByExt(img, output_file);

    // Decompressing the output file
    decompressed = iftReadImageByExt(output_file);

    // Comparing values
    fprintf(stderr,"Image size: %d (%d %d %d)\n", img->n, img->xsize, img->ysize, img->zsize);
    fprintf(stderr,"Image bpp: %f %f %f\n", img->dx, img->dy, img->dz);

    fprintf(stderr,"Decompressed Image size: %d (%d %d %d)\n", decompressed->n, decompressed->xsize, decompressed->ysize, decompressed->zsize);
    fprintf(stderr,"Decompressed Image bpp: %f %f %f\n", decompressed->dx, decompressed->dy, decompressed->dz);

    // Verifying if the values above match indeed
    equal = iftCheckImageIntegrity(img, decompressed);

    if(equal)
        fprintf(stderr,"Images are equal...\n");
    else
        fprintf(stderr,"Images differ!\n");

    iftDestroyImage(&decompressed);

    // Decompressing the file with an external software
    bname = iftBasename(output_file);
    sprintf(command, "gzip -c -d -S .zscn %s > %s.scn", output_file,bname);
    if(!system(command)) {
        char *decompressed_filename = NULL;

        // Determining the decompressed filename

        decompressed_filename = iftConcatStrings(2, bname, ".scn"); // gzip removes .gz and .zscn, adding .scn

        decompressed = iftReadImageByExt(decompressed_filename);

        // Comparing the result decompressed with an external tool and the original input image
        equal = iftCheckImageIntegrity(img, decompressed);

        if (equal)
            fprintf(stderr, "Decompressed image with external tool and original image are equal...\n");
        else
            fprintf(stderr, "Decompressed image with external tool and original image differ!!\n");
        iftDestroyImage(&decompressed);

        iftFree(decompressed_filename);
    }
    iftDestroyImage(&img);
    iftFree(bname);

}


bool iftCheckImageIntegrity(const iftImage *img, const iftImage *decompressed) {
    bool equal;

    equal = img->n == decompressed->n;
    equal = equal && img->xsize == decompressed->xsize;
    equal = equal && img->ysize == decompressed->ysize;
    equal = equal && img->zsize == decompressed->zsize;
    equal = equal && iftIsVoxelSizeEqual(img, decompressed);

    for(int i = 0; i < img->n && equal; i++) {
        equal = equal && img->val[i] == decompressed->val[i];
    }

    return equal;
}

iftDict *iftGetArguments(int argc, const char *argv[]) {
    char program_description[2048] = "This is a demo program to demonstrate the usage of the .scn file compression using " \
                                        "the zlib library.";

    // this array of options, n_opts, and the program description could be defined in the main or as a global variables
    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--input-file", .has_arg=true, .arg_type=IFT_STR_TYPE, .required=true, .help="Input text file"},
        {.short_name = "-o", .long_name = "--output-file", .has_arg=true, .arg_type=IFT_STR_TYPE, .required=true, .help="Output compressed file (with .scn.gz or .zscn extension)"},
    };
    // we could simply assign the number 5 in this case
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);


    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}
