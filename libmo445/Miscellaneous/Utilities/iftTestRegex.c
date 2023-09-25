/**
 * @file
 * @brief Program that checks if a string matches with a regex.
 * @note See the source code in @ref iftTestRegex.c
 * 
 * @example iftTestRegex.c
 * @brief Program that checks if a string matches with a regex.
 * @author Samuel Martins
 * @date Mar 3, 2016
 */

#include "ift.h"

int main(int argc, const char *argv[]) {
    if (argc != 3)
        iftError("iftTestRegex <string/expression to be checked> <regex>\n\n" \
                 "--> Use \\ before the space in the expression\n" \
                 "--> Use \\ before the characters: (, ), {, }, \\, |, $\n", "main");

    char *exp   = iftCopyString(argv[1]);
    char *regex = iftCopyString(argv[2]);

    printf("--> Expression: %s\n", exp);
    printf("--> Regex: %s\n", regex);
    printf("\nMatched?: ");
    printf(IFT_COLOR_YELLOW "%s\n" IFT_COLOR_RESET, iftRegexMatch(exp, regex) ? "true" : "false");

    return 0;
}