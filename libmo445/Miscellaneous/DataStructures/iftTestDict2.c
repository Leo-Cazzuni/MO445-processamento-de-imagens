/**
 * @brief It shows how to get the elements from the generic dictionary.
 * This examples only uses string keys, but you could use any other type (except bool and pointer types) instead of.
 * To see antoher example about dict insertion, please see the program demo/Miscellaneous/iftTestDict1.c
 * @author Samuel Martins
 * @date Feb 1st, 2016
 */

#include "ift.h"

int main(int argc, const char *argv[]) {
    iftDict *dict = iftCreateDict();
    dict->erase_elements = true; // enables the iftDestroyDict to deallocate all its elements

    iftImage *img = iftCreateImage(5, 10, 15);

    // When inserting an array (or a matrix or a dict), it is insert its reference and not a copy
    // Be careful, because if you deallocate the array (variable), the array (reference) inserted into
    // dict will be pointing to an invalid region
    // If you set dict->erase_elementes = true, the iftDestroyDict will deallocate all its elements,
    // then, you don't need to do that with the variables
    iftIntArray *iarr = iftCreateIntArray(3);
    iarr->val[0] = 10;
    iarr->val[1] = 11;
    iarr->val[2] = 12;

    iftDblArray *darr = iftCreateDblArray(3);
    darr->val[0] = 10.5;
    darr->val[1] = 11.5;
    darr->val[2] = 12.5;

    iftStrArray *sarr = iftCreateStrArray(3);
    strcpy(sarr->val[0], "zero");
    strcpy(sarr->val[1], "first");
    strcpy(sarr->val[2], "second");

    iftIntMatrix *imat = iftCreateIntMatrix(2, 2);
    imat->val[0] = 0;
    imat->val[1] = 1;
    imat->val[2] = 2;
    imat->val[3] = 3;

    iftMatrix *mat = iftCreateMatrix(2, 2);
    mat->val[0] = 0.5;
    mat->val[1] = 1.5;
    mat->val[2] = 2.5;
    mat->val[3] = 3.5;

    iftStrMatrix *smat = iftCreateStrMatrix(2, 2);
    strcpy(smat->val[0], "00");
    strcpy(smat->val[1], "01");
    strcpy(smat->val[2], "10");
    strcpy(smat->val[3], "11");

    iftInsertIntoDict("name", "Batman", dict);
    iftInsertIntoDict("initial", (char) 'B', dict);
    iftInsertIntoDict("age", 32, dict);
    iftInsertIntoDict("height", 1.90, dict);
    iftInsertIntoDict("super-hero", (bool) true, dict);
    iftInsertIntoDict("avatar-image", img, dict);
    iftInsertIntoDict("int-array", iarr, dict);
    iftInsertIntoDict("dbl-array", darr, dict);
    iftInsertIntoDict("str-array", sarr, dict);
    iftInsertIntoDict("int-matrix", imat, dict);
    iftInsertIntoDict("dbl-matrix", mat, dict);
    iftInsertIntoDict("str-matrix", smat, dict);

    
    // GETS THE DICT VALUES WITHOUT REMOVING THEM
    char *name             = iftGetStrValFromDict("name", dict); // gets a COPY string - YOU MUST TO DEALLOCATE IT AFTER
    const char *const_name = iftGetConstStrValFromDict("name", dict); // gets a const string - YOU DON'T NEED TO DEALLOCATE IT
    char initial           = iftGetCharValFromDict("initial", dict); 
    long age               = iftGetLongValFromDict("age", dict); // try always to use long variables instead of int in order to avoid possible data loss,
                                                            // since the dict only handles with long
    double height     = iftGetDblValFromDict("height", dict); // try always to use double variables instead of float in order to avoid possible data loss,
                                                              // since the dict only handles with double
    bool super_hero   = iftGetBoolValFromDict("super-hero", dict);                                                          // 
    iftImage *avatar  = iftGetPtrValFromDict("avatar-image", dict); // gets the pointer (not a copy)

    printf("name: %s\n", name);
    printf("const name: %s\n", const_name);
    printf("initial: %c\n", initial);
    printf("age: %ld\n", age);
    printf("height: %lf m\n", height);
    printf("super hero?: %s\n", super_hero ? "true" : "false");
    printf("Image sizes (x, y, z): %d, %d, %d\n\n", avatar->xsize, avatar->ysize, avatar->zsize);

    // the dict keep having the all elements
    puts("************************************************");
    iftPrintDict(dict);
    puts("");
    iftPrintDictMinified(dict);
    puts("");
    iftPrintDictAsArray(dict); // prints from index [0] to [dict->size-1]
    puts("************************************************\n");

    // if you want to remove some element(s) from the dict to have more free buckets
    iftRemoveValFromDict("initial", dict);
    iftRemoveValFromDict("height", dict);
    iftRemoveValFromDict("age", dict);

    // the dict keep having the all elements
    puts("************************************************");
    iftPrintDict(dict);
    puts("");
    iftPrintDictMinified(dict);
    puts("");
    iftPrintDictAsArray(dict); // prints from index [0] to [dict->size-1]
    puts("************************************************\n");

    // gets a copy
    iftDict *copy = iftCopyDict(dict);
    puts("dict");
    iftPrintDictAsArray(dict);
    puts("\ncopy");
    iftPrintDictAsArray(copy); 

    iftDestroyDict(&dict);
    puts("\ncopy");
    iftPrintDictAsArray(copy); 



    free(name);
    iftDestroyDict(&copy);
    iftDestroyImage(&avatar);

    return 0;
}