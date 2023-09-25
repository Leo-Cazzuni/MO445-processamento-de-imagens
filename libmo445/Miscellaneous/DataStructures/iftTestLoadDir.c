/**
 * @file
 * @brief This program loads a directory. 
 * @note See the source code in @ref iftTestLoadDir.c
 * 
 * @example iftTestLoadDir.c
 * @brief This program loads a directory. 
 * @author Samuel Martins
 */

#include "ift.h"



int main(int argc, char *argv[]) {
   if (argc != 3)
       iftError("iftTestLoadDir <directory> <hier_levels>\n\n" \
                "* hier_levels = 0 --> Gets all files and subdirectories\n" \
                "* hier_levels = 1 --> Gets only the files and subdirectories from the 1st hierarchical level." \
                "It does not load the files from these subdirectories.\n" \
                "* And so forth" \
, "main");

   char pathname[512]; strcpy(pathname, argv[1]);
   int hier_level = atoi(argv[2]);

   iftDir *dir = iftLoadDir(pathname, hier_level);

   iftPrintDirAsTree(dir);

   iftFile *file = NULL;
   if (dir->nfiles != 0)
       file = dir->files[0];
   iftPrintFileInfo(file);

   iftDir* dir2 = iftLoadFilesFromDirBySuffix(argv[1], "jpg");
   iftPrintDirAsTree(dir2);

   iftDestroyDir(&dir);

    return 0;
}