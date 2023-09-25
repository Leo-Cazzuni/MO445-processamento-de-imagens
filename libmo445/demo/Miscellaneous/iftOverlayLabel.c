//
// Created by Thiago Vallin Spina on 5/11/17.
//


#include "ift.h"

iftDict *iftGetArguments(int argc, const char *argv[]);

int main(int argc, const char *argv[])
{
    iftImage  *img = NULL,*label = NULL,*rend = NULL;
    timer     *t1=NULL,*t2=NULL;
    iftColorTable *cmap = NULL;
    int max_lb;
    iftDict *args = NULL;

    /*--------------------------------------------------------*/
    size_t MemDinInicial, MemDinFinal;
    MemDinInicial = iftMemoryUsed();

    /*--------------------------------------------------------*/

    args = iftGetArguments(argc, argv);

    img    = iftReadImageByExt(iftGetConstStrValFromDict("--input", args));
    label  = iftReadImageByExt(iftGetConstStrValFromDict("--label", args));

    float opacity = (float)iftGetDblValFromDict("--object-opacity", args);
    bool fill = iftDictContainKey("--fill", args, NULL);

    t1  = iftTic();

    max_lb  = iftMaximumValue(label);
    if(max_lb >= 6 && iftCompareStrings(iftGetConstStrValFromDict("--colormap", args), "JET")) {
        cmap = iftBlueToRedColorTable(max_lb + 1);
    } else {
        if(iftCompareStrings(iftGetConstStrValFromDict("--colormap", args), "JET"))
            iftWarning("Less than 7 colors in the label map, using RANDOM colormap instead", "main");

        cmap = iftCreateColorTable(max_lb + 1);
    }
    t2  = iftToc();

    fprintf(stdout,"Colormap creation in %f ms\n",iftCompTime(t1,t2));

    t1 = iftTic();

    iftAdjRel *A    = NULL;

    if(iftIs3DImage(img)) {
        A = iftSpheric(1.0);
    } else {
        A = iftCircular(1.0);
    }

    iftDrawLabels(img, label, cmap, A, fill, opacity);
    char *bname = iftBasename(iftGetConstStrValFromDict("--output", args));

    if(iftIs3DImage(img)) {
        char *folder = iftJoinPathnames(2, bname, "xy");
        iftMakeDir(folder);

        for(int z = 0; z < label->zsize; z++) {
            rend = iftGetXYSlice(img, z);

            char fname[IFT_STR_DEFAULT_SIZE];
            sprintf(fname, "xy_%05d.png", z);
            char *filename = iftJoinPathnames(2, folder, fname);

            iftWriteImageByExt(rend, filename);

            iftDestroyImage(&rend);
            iftFree(filename);
        }
        iftFree(folder);
        folder = iftJoinPathnames(2, bname, "yz");
        iftMakeDir(folder);

        for(int x = 0; x < label->xsize; x++) {
            rend = iftGetYZSlice(img, x);

            char fname[IFT_STR_DEFAULT_SIZE];
            sprintf(fname, "yz_%05d.png", x);
            char *filename = iftJoinPathnames(2, folder, fname);

            iftWriteImageByExt(rend, filename);

            iftDestroyImage(&rend);
            iftFree(filename);
        }
        iftFree(folder);
        folder = iftJoinPathnames(2, bname, "zx");
        iftMakeDir(folder);

        for(int y = 0; y < label->ysize; y++) {
            rend = iftGetZXSlice(img, y);

            char fname[IFT_STR_DEFAULT_SIZE];
            sprintf(fname, "zx_%05d.png", y);
            char *filename = iftJoinPathnames(2, folder, fname);

            iftWriteImageByExt(rend, filename);

            iftDestroyImage(&rend);
            iftFree(filename);

        }
        iftFree(folder);
    } else {
        char filename[IFT_STR_DEFAULT_SIZE];
        sprintf(filename, "%s.png", bname);
        iftWriteImageByExt(img, filename);
    }
    iftFree(bname);

    t2  = iftToc();

    fprintf(stdout,"Overlay creation in %f ms\n",iftCompTime(t1,t2));

    iftDestroyAdjRel(&A);
    iftDestroyImage(&label);
    iftDestroyImage(&rend);
    iftDestroyImage(&img);

    iftDestroyColorTable(&cmap);

    /* ---------------------------------------------------------- */

    MemDinFinal = iftMemoryUsed();
    if (MemDinInicial!=MemDinFinal)
        printf("\n\nDinamic memory was not completely deallocated (%lu, %lu)\n",
               MemDinInicial,MemDinFinal);

    return(0);
}


iftDict *iftGetArguments(int argc, const char *argv[]) {
    char program_description[2048] = "This is a program renders the objects in a scene";

    // this array of options, n_opts, and the program description could be defined in the main or as a global variables
    iftCmdLineOpt cmd_line_opts[] = {
            {.short_name = "-i", .long_name = "--input", .has_arg=true, .arg_type=IFT_STR_TYPE,
                    .required=true, .help="Input image (2D -- .ppm, .pgm, .jpeg -- or 3D)"},
            {.short_name = "-l", .long_name = "--label", .has_arg=true, .arg_type=IFT_STR_TYPE,
                    .required=true, .help="Input label (2D -- .pgm, .png -- or 3D)"},
            {.short_name = "-o", .long_name = "--output", .has_arg=true, .arg_type=IFT_STR_TYPE,
                    .required=true, .help="Output basename for the 2D image or slices. The images will be saved as PNGs. In the 3D case, a directory will be created with the basename."},
            {.short_name = "-a", .long_name = "--object-opacity", .has_arg=true, .arg_type=IFT_DBL_TYPE,
                    .required=true, .help="Object opacity withing [0.0, 1.0]"},
            {.short_name = "-c", .long_name = "--colormap", .has_arg=true, .arg_type=IFT_STR_TYPE,
                    .required=true, .help="Colormap to be used {RANDOM, JET}"},
            {.short_name = "-f", .long_name = "--fill", .has_arg=false, .arg_type=IFT_BOOL_TYPE,
                    .required=false, .help="Fills the object with color"}

    };
    // we could simply assign the number 5 in this case
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}