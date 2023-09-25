/**
 * @file
 * @brief It returns a copy of the string in which the occurrences of old substring have been replaced
 * with new substring 
 * @note See the source code in @ref iftTestReplaceString.c
 * 
 * @example iftTestReplaceString.c
 * @brief It returns a copy of the string in which the occurrences of old substring have been replaced
 * with new substring 
 * @author Samuel Martins
 * @date Mar 21, 2016
 */
#include "ift.h"


int main(int argc, const char *argv[]) {
    if (argc != 4)
        iftError("iftTestReplaceString <string> <old_sub> <new_sub>", "main");

    const char *str     = argv[1];    
    const char *old_sub = argv[2];    
    const char *new_sub = argv[3];    


    printf("String: \"%s\"\n", str);
    printf("Old Substring: \"%s\"\n", old_sub);
    printf("New Substring: \"%s\"\n\n", new_sub);
    
    char *rep_str = iftReplaceString(str, old_sub, new_sub);
    printf("Replaced String: \"%s\"\n", rep_str);

    free(rep_str);

    return 0;
}













