/**
 * @file
 * @brief This program shows examples of how to create Pathnames, Files, and Directories. 
 * @note See the source code in @ref iftMakeDirsExamples.c
 * 
 * @example iftMakeDirsExamples.c
 * @brief This program shows examples of how to create Pathnames, Files, and Directories. 
 * @author Samuel Martins
 */

#include "ift.h"


int main(int argc, const char *argv[]) { 
    if (argc != 2)
        iftError("iftTestLoadDir <dirname to be created>", "main");

    char *dirname  = iftCopyString(argv[1]);
    printf("- Creating the dir \"%s\"\n\n", dirname);
    iftMakeDir(dirname);
    free(dirname);

    // Creates Temp Pathnames - NOT STORED ON DISK
    puts("- Creating tmp pathnames/string - NOT STORED ON DISK");
    puts(iftMakeTempPathname(NULL, NULL, NULL));
    puts(iftMakeTempPathname("", "", NULL)); // same behaviour of the instruction above
    puts(iftMakeTempPathname("tmp_", "", NULL)); // tmp file with prefix and no suffix
    puts("");

    // Creates Temp Files from several ways
    puts("- Creating the tmp files:");
    puts(iftMakeTempFile(NULL, NULL, NULL));
    puts(iftMakeTempFile("", NULL, NULL)); // same behaviour of the instruction above
    puts(iftMakeTempFile("tmp_", NULL, NULL)); // tmp file with prefix
    puts("");

    // Creates Temp Dirs from several ways
    puts("- Creating the tmp dirs:");
    puts(iftMakeTempDir(NULL, NULL, NULL));
    puts(iftMakeTempDir("", NULL, NULL)); // same behaviour of the instruction above
    puts(iftMakeTempDir("tmpdir_", NULL, NULL)); // tmp file with prefix
    puts("");

    return 0;
}
