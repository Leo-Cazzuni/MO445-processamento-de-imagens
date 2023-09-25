/**
 * @file
 * @brief Shows some examples of how to use Regular Expression in C.
 * 
 * @note See the source code in @ref iftRegexExamples.c
 *
 * <pre>
 * Some observations: 
 * * --> Zero or more occurrences 
 * + --> One or more occurrences 
 * ? --> Zero or one occurrence 
 * ^ --> Beginning of the regex 
 * $ --> Beginning of the regex 
 * ( --> Opens a group 
 * ) --> Ends a group 
 * { --> Opens a repetition 
 * } --> Opens a repetition 
 * | --> Conditional OR used in groups  
 *
 * .     --> any character 
 * \.    --> To use the character '.' --> In C, '\\.' 
 * \+    --> To use the character '+' --> In C, '\\+' 
 * \(    --> To use the character '(' --> In C, '\\(' 
 * \)    --> To use the character ')' --> In C, '\\)' 
 * \{    --> To use the character '{' --> In C, '\\{' 
 * \}    --> To use the character '}' --> In C, '\\}' 
 * \|    --> To use the character '|' --> In C, '\\|' 
 * \$    --> To use the character '$' --> In C, '\\$' 
 * \^    --> To use the character '^' --> In C, '\\^' 
 * \\    --> To use the character '\' --> In C, '\\\\'  
 *
 * [[:digit:]] or [0-9]           --> digit 
 * [[:digit:]]* or [0-9]*         --> Zero or more digits 
 * [[:digit:]]+ or [0-9]+         --> At least one digit 
 * [[:digit:]]{2,4} or [0-9]{2,4} --> From two to four digits 
 * [[:digit:]]{5,} or [0-9]{5,}   --> At least five digits 
 * [[:digit:]]{,6} or [0-9]{,6}   --> At most six digits 
 * -> The same holds to others characters  
 *
 * [abc]  --> only the letter a, b, or c 
 * [0123] --> only the digits 0, 1, 2, or 3 
 *
 * [a-z]       --> lowercase letters 
 * [A-Z]       --> uppercase letters 
 * [a-zA-Z]    --> letters 
 * [a-zA-Z0-9] --> digits and letter 
 *
 * [^0-9]    --> any character except digits 
 * [^a-zA-Z] --> any character except letters 
 *
 * [[:space:]] --> space  
 *
 * Groups: 
 * |            --> it means OR 
 * ([a-z]|-|\+) --> Either lowercase letters or hyphen '-' or plus '+' 
 * ([789]|[ijh]) --> Either the digits 7, 8, 9 or the letters i, j, h 
 * </pre>
 *
 * References: 
 * https://solarianprogrammer.com/2011/10/12/cpp-11-regex-tutorial/ 
 * http://regexone.com/ (Good Tutorial, but not with the C Regex word style)
 *
 * @example iftRegexExamples.c
 * @brief Shows some examples of how to use Regular Expression in C.
 * @author Samuel Martins
 * @date Dec 11, 2015
 */

#include "ift.h"

void iftPrintRegex(const char *str, const char *regex) {
    printf("--> Expression: %s\n", str);
    printf("--> Regex: %s\n", regex);
    printf("Matched?: ");
    printf(IFT_COLOR_YELLOW "%s\n\n" IFT_COLOR_RESET, iftRegexMatch(str, regex) ? "true" : "false");
}



int main(int argc, const char *argv[]) {
    char str[128], regex[128];

    // Matches
    strcpy(str, "0001");
    strcpy(regex, "^[0-9]+$");
    iftPrintRegex(str, regex);

    // Matches
    strcpy(str, "-10");
    strcpy(regex, "^-?[0-9]+$");
    iftPrintRegex(str, regex);

    // Matches
    strcpy(str, "100000");
    strcpy(regex, "^-?[0-9]+$");
    iftPrintRegex(str, regex);

    // Matches
    strcpy(str, "/home/sbmmartins/Documents/img_0001.pgm");
    strcpy(regex, "^(/.+)*/?img_[0-9]+\\.pgm$");
    iftPrintRegex(str, regex);

    // Matches
    strcpy(str, "000006_00000008.scn");
    strcpy(regex, "^[0-9]{6}_[0-9]{8}\\.scn$");
    iftPrintRegex(str, regex);

    // Matches
    strcpy(str, "C:\\windows\\Documents\\test.txt");
    strcpy(regex, "^(\\\\.+)*\\\\?.+\\.txt$");
    iftPrintRegex(str, regex);

    // Matches
    strcpy(str, "+55 19 3521-0339");
    strcpy(regex, "^\\+(([0-9]{2})[[:space:]]?){2}([0-9]{4}-[0-9]{4})$");
    iftPrintRegex(str, regex);

    // Matches --> Sao Paulo Phone Code: 11--19
    strcpy(str, "(19) 3521-0339");
    strcpy(regex, "^\\(1[1-9]\\) ([0-9]{4}-[0-9]{4})$");
    iftPrintRegex(str, regex);

    // Matches --> --inputImage, --input-image, --input_image
    // The first two chars are '-'
    // The third char is a letter
    // The next chars are either letters or - or _ or nothing
    strcpy(regex, "^--[a-zA-Z]([a-zA-Z]|-|_)+$");
    strcpy(str, "--inputImage");
    iftPrintRegex(str, regex);
    strcpy(str, "--input-image");
    iftPrintRegex(str, regex);
    strcpy(str, "--input_image");
    iftPrintRegex(str, regex);

    return 0;
}