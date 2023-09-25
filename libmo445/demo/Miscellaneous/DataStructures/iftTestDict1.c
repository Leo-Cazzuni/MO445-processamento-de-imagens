/**
 * @file
 * @brief It tests creation, insertions, and printing of the Generic dictionary.
 * To see how to get elements from the dict, please see the program demo/Miscellaneous/iftTestDict2.c
 * @note See the source code in @ref iftTestDict1.c
 * 
 * @example iftTestDict1.c
 * @brief A demo program which shows how to parse command line options from the input.
 * @author Samuel Martins
 * @date Feb 1st, 2016
 */

#include "ift.h"

int main(int argc, const char *argv[]) {
    ////////////////////////////////////////////////////////////////////////////////////////////
    // - You can insert pairs of keys and values of "any" datatype in the same dict.          //
    // - But, remember that the allowed datatypes to the keys are:                            //
    //   char, unsigned char, string (char*), short, int, long, unsigned short, unsigned int, //
    //   unsigned long, float, double                                                         //
    // - In turn, values also allows bool and pointers.                                       //
    // - Pointers to structs are AUTOMATICALLY converted to void* before insertion.           //
    // - WE MUST CASTING LITERAL CHAR AND BOOL VALUES, ELSE THEY WILL BE CONVERTED TO INTEGER //
    //                                                                                        //
    // - When inserting a new value from an existing, its value is replaced by the new one    //
    ////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Initially, the dict has size equals to IFT_INITIAL_DICT_SIZE.
     * If the dict is FULL, when trying to insert a new element, the dictionary is RESIZED and all its elements are rehashed.
     */
    iftDict *dict = iftCreateDict();
    iftImage *img = iftCreateImage(5, 10, 15);

    // mix of key and value datatypes
    puts("************************************************");
    iftInsertIntoDict("name", "Samuel", dict);
    iftInsertIntoDict((char) 'a', (bool) true, dict);
    iftInsertIntoDict(1.99, "it's cheap", dict);  // int values are automatically converted to long
    iftInsertIntoDict(-1000, 0.1234f, dict); // float values are automatically converted to double 
    iftInsertIntoDict("avatar", img, dict);     
    iftInsertIntoDict("name", "Batman", dict); // the value is replaced, since the key already exists


    iftPrintDict(dict);
    puts("");
    iftPrintDictMinified(dict);
    puts("");
    iftPrintDictAsArray(dict); // prints from index [0] to [dict->size-1]
    puts("************************************************\n");

    iftRemoveValFromDict("name", dict);
    iftRemoveValFromDict("avatar", dict);
    iftPrintDict(dict);


    // The Dictionary Destroyer does not deallocate the pointers. Then, we must destroy it manually.
    // This can be done removing all pointers from the dict or using the variable that also pointer to it.
    iftDestroyImage(&img);
    iftDestroyDict(&dict);

    return 0;
}