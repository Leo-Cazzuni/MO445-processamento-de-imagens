#include "ift.h"

iftDict *iftGetArguments(int argc, const char **argv);

int main(int argc, const char **argv)
{
    timer       *t1=NULL,*t2=NULL;

    /*--------------------------------------------------------*/

    int MemDinInicial, MemDinFinal;
    MemDinInicial = iftMemoryUsed();

    /*--------------------------------------------------------*/


    iftImage *img        = NULL;
    iftImageTiles *tiles = NULL;
    iftImage **imgtile   = NULL;
    iftDict  *args       = NULL;
    char *input_filename = NULL, output_filename[IFT_STR_DEFAULT_SIZE], *tiling_type = NULL;
    long ntiles_x = 0, ntiles_y = 0, ntiles_z = 1, tiles_xsize = 0, tiles_ysize = 0, tiles_zsize = 1, xstride = 0, ystride = 0, zstride = 1;
    bool append_coords_to_filename = false;

    args = iftGetArguments(argc, argv);

    input_filename = iftGetStrValFromDict("--input-img", args);
    tiling_type    = iftGetStrValFromDict("--tiling-type", args);

    img = iftReadImageByExt(input_filename);

    if(iftCompareStrings(tiling_type, "EQUAL")) {
        if(!iftDictContainKey("--ntiles_x", args, NULL))
            iftError("Parameter --ntiles_x required for tiling type EQUAL!", "main");
        if(!iftDictContainKey("--ntiles_y", args, NULL))
            iftError("Parameter --ntiles_y required for tiling type EQUAL!", "main");
        if(iftIs3DImage(img) && !iftDictContainKey("--ntiles_z", args, NULL))
            iftError("Parameter --ntiles_z required for tiling type EQUAL!", "main");

        ntiles_x = iftGetLongValFromDict("--ntiles_x", args);
        ntiles_y = iftGetLongValFromDict("--ntiles_y", args);

        if(iftIs3DImage(img))
            ntiles_z = iftGetLongValFromDict("--ntiles_z", args);

    } else if(iftCompareStrings(tiling_type, "STRIDE")){

        if(!iftDictContainKey("--tile_xsize", args, NULL))
            iftError("Parameter --tile_xsize required for tiling type STRIDE!", "main");
        if(!iftDictContainKey("--tile_ysize", args, NULL))
            iftError("Parameter --tile_usize required for tiling type STRIDE!", "main");
        if(iftIs3DImage(img) && !iftDictContainKey("--tile_zsize", args, NULL))
            iftError("Parameter --tile_zsize required for tiling type STRIDE!", "main");
        if(!iftDictContainKey("--xstride", args, NULL))
            iftError("Parameter --xstride required for tiling type STRIDE!", "main");
        if(!iftDictContainKey("--ystride", args, NULL))
            iftError("Parameter --ystride required for tiling type STRIDE!", "main");
        if(iftIs3DImage(img) && !iftDictContainKey("--zstride", args, NULL))
            iftError("Parameter --zstride required for tiling type STRIDE!", "main");

        tiles_xsize = iftGetLongValFromDict("--tile_xsize", args);
        tiles_ysize = iftGetLongValFromDict("--tile_ysize", args);

        if(iftIs3DImage(img))
            tiles_zsize = iftGetLongValFromDict("--tile_zsize", args);

        xstride = iftGetLongValFromDict("--xstride", args);
        ystride = iftGetLongValFromDict("--ystride", args);
        if(iftIs3DImage(img))
            zstride = iftGetLongValFromDict("--zstride", args);

    } else {
        iftError("Invalid tiling type %s!", "main", tiling_type);
    }

    append_coords_to_filename = iftDictContainKey("--append_coords_to_filename", args, NULL);

    if(iftCompareStrings(tiling_type, "EQUAL"))
        imgtile = iftSplitImageIntoTilesByEquallyDividingAxes(img, ntiles_x, ntiles_y, ntiles_z, &tiles);
    else
        imgtile = iftSplitImageIntoTilesByStriding(img, tiles_xsize, tiles_ysize, tiles_zsize, xstride, ystride, zstride, &tiles);

    t1     = iftTic();

    const char *ext = NULL;
    if (iftIs3DImage(img)) {
        ext = ".scn";
    } else {
        if(iftIsColorImage(img)) {
            ext = ".ppm";
        } else {
            ext = ".pgm";
        }
    }

    for (int i=0; i < tiles->ntiles; i++) {
        if(!append_coords_to_filename) {
            sprintf(output_filename, "tile%04d", i);
        } else {
            sprintf(output_filename, "tile%04d_%04d_%04d_%04d", i, tiles->tile_coords[i].begin.x, tiles->tile_coords[i].begin.y,
                    tiles->tile_coords[i].begin.z);
        }
        iftWriteImageByExt(imgtile[i], "%s%s", output_filename, ext);
    }

    t2     = iftToc();
    fprintf(stdout,"Image tiles extracted in %f ms\n",iftCompTime(t1,t2));


    for (int i=0; i < tiles->ntiles; i++)
        iftDestroyImage(&imgtile[i]);

    free(imgtile);

    iftDestroyImage(&img);
    iftDestroyImageTiles(&tiles);
    iftDestroyDict(&args);
    free(input_filename);
    free(tiling_type);

    /* ---------------------------------------------------------- */

    MemDinFinal = iftMemoryUsed();
    if (MemDinInicial!=MemDinFinal)
        printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
               MemDinInicial,MemDinFinal);

    return(0);

}

iftDict *iftGetArguments(int argc, const char **argv) {
    // this array of options, n_opts, and the program description could be defined in the main or as a global variables
    iftCmdLineOpt cmd_line_opts[] = {
            {.short_name = "-i", .long_name = "--input-img", .has_arg=true, .required=true, .arg_type = IFT_STR_TYPE, .help="Input image"},
            {.short_name = "-t", .long_name = "--tiling-type", .has_arg=true, .required=true, .arg_type = IFT_STR_TYPE, .help="Tiling type [EQUAL, STRIDE]."},
            {.short_name = "", .long_name = "--ntiles_x", .has_arg=true, .required=false, .arg_type = IFT_LONG_TYPE, .help="Number of tiles in the X dimension (required for EQUAL, not used for STRIDE)"},
            {.short_name = "", .long_name = "--ntiles_y", .has_arg=true, .required=false, .arg_type = IFT_LONG_TYPE, .help="Number of tiles in the Y dimension (required for EQUAL, not used for STRIDE)"},
            {.short_name = "", .long_name = "--ntiles_z", .has_arg=true, .required=false, .arg_type = IFT_LONG_TYPE, .help="Number of tiles in the Z dimension (required for EQUAL, not used for STRIDE)"},
            {.short_name = "", .long_name = "--tile_xsize", .has_arg=true, .required=false, .arg_type = IFT_LONG_TYPE, .help="Tile size in the X dimension (required for STRIDE, not used for EQUAL)"},
            {.short_name = "", .long_name = "--tile_ysize", .has_arg=true, .required=false, .arg_type = IFT_LONG_TYPE, .help="Tile size in the Y dimension (required for STRIDE, not used for EQUAL)"},
            {.short_name = "", .long_name = "--tile_zsize", .has_arg=true, .required=false, .arg_type = IFT_LONG_TYPE, .help="Tile size in the Z dimension (required for STRIDE, not used for EQUAL)"},
            {.short_name = "", .long_name = "--xstride", .has_arg=true, .required=false, .arg_type = IFT_LONG_TYPE, .help="Stride in the X dimension (required for STRIDE, not used for EQUAL)"},
            {.short_name = "", .long_name = "--ystride", .has_arg=true, .required=false, .arg_type = IFT_LONG_TYPE, .help="Stride in the Y dimension (required for STRIDE, not used for EQUAL)"},
            {.short_name = "", .long_name = "--zstride", .has_arg=true, .required=false, .arg_type = IFT_LONG_TYPE, .help="Stride in the Z dimension (required for STRIDE, not used for EQUAL)"},
            {.short_name = "", .long_name = "--append_coords_to_filename", .has_arg=false, .required=false, .arg_type = IFT_BOOL_TYPE, .help="The image coordinates from the tile are appended to the file name"}
    };

    // we could simply assign the number 5 in this case
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

    char program_description[IFT_STR_DEFAULT_SIZE] = "This program splits a 2D/3D image into tiles. There are two types of tiling:\n" \
                                                        "** EQUAL partitions the image by equally dividing the axes into a grid of --ntiles_x \\times --ntiles_y \\times --ntiles_z tiles. \n" \
                                                        "** STRIDE partitions the image into tiles of the same given --tile_x/y/zsize, while also allowing the tiles to overlap depending on --x/y/zstride.\n" \
                                                        "   If the tiles must not overlap then use --x/y/z/stride at least equal to --tile_x/y/zsize for type STRIDE.\n"
                                                        "   Examples:  1) iftImageToTiles -i image.ppm -t STRIDE --tile_xsize 400 --tile_ysize 300 --xstride 400 --ystride 300\n" \
                                                        "                 Partitions an image into tiles of size 400x300 without overlap.\n" \
                                                        "              2) iftImageToTiles -i image.ppm -t STRIDE --tile_xsize 400 --tile_ysize 300 --xstride 300 --ystride 200\n" \
                                                        "                 Partitions an image into tiles of size 400x300 with overlap of 100 pixels in x and y.\n" \
                                                        "              3) iftImageToTiles -i image.ppm -t STRIDE --tile_xsize 400 --tile_ysize 300 --xstride 500 --ystride 400\n" \
                                                        "                 Partitions an image into tiles of size 400x300 without overlap and separated by 100 pixels in x and y.\n" \
                                                        "              4) iftImageToTiles -i image.ppm -t EQUAL --ntiles_x 4 --ntiles_y 30\n" \
                                                        "                 Partitions an image into tiles of approximately equal size using a 4 by 30 grid.\n"\
                                                        "                 If the image has size of 4000x3000 pixels, then the tiles will have dimension of 1000x100 pixels.";

    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);

    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments

    iftDestroyCmdLineParser(&parser);

    return args;
}
