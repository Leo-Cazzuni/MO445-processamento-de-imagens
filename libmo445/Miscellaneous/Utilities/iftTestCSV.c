/**
 * @file
 * @brief It shows how to open a CSV File. 
 * @note See the source code in @ref iftTestCSV.c
 * 
 * @example iftTestCSV.c
 * @brief It shows how to open a CSV File. 
 * @author Samuel Martins
 */

#include "ift.h"


int main(int argc, const char *argv[]) {
    if (argc != 2)
        iftError("iftTestCSV <csv_file.csv", "main");

    char csv_pathname[512];
    strcpy(csv_pathname, argv[1]);

    iftCSV *csv = iftReadCSV(csv_pathname, ',');
    iftPrintCSV(csv);
    iftDestroyCSV(&csv);

    return 0;
}
