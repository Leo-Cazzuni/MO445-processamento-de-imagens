/**
 * @file
 * @brief This program splits a string at a given position. 
 * @note See the source code in @ref iftTestSplitString.c
 * 
 * @example iftTestSplitString.c
 * @brief This program splits a string at a given position. 
 * @author Samuel Martins
 */

#include "ift.h"


int main(int argc, const char *argv[]) {
    if (argc != 3)
        iftError("iftTestSplitString <string> <delimiter>", "main");

    const char *phrase    = argv[1];
    const char *delimiter = argv[2];

    printf("Original String: %s\n", argv[1]);

    puts("Split String into List");
    iftSList *SL = iftSplitString(phrase, delimiter);
    iftPrintSList(SL);

    iftDestroySList(&SL);

    return 0;
}