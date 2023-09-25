/**
 * @file
 * @brief A demo program which shows how to parse command line options from the input.
 * @note See the source code in @ref iftTestCmdLineParser1.c
 * 
 * @example iftTestCmdLineParser1.c
 * @brief A demo program which shows how to parse command line options from the input.
 * @author Samuel Martins
 * @date Feb 15, 2016
 */

#include "ift.h"


/**
 * It does all setup of the command line parser, it parses the command line and get all its arguments.
 * This avoids that such amount of code is in the main.
 */
iftDict *iftGetArguments(int argc, const char *argv[]);



int main(int argc, const char *argv[]) {
    // Firstly, you must create a function where will be defined all settings and it will be setup
    // the command line parser, returning the option/argument dictionary.
    // This could be also done inside of the main, as it is done in the program demo/Miscellaneous/iftTestCmdLineParser2.c,
    // but I prefer to create a function just to avoid a lot of code line inside the main.  
    iftDict *args = iftGetArguments(argc, argv);
    

    ///////////// Getting the REQUIRED options/parameters /////////////
    // If anyone is not passed, an exception will be raised before, when calling iftParseCmdLine()
    const char *in_img_path  = iftGetConstStrValFromDict("--input-img", args); // equal result with the key "-i" 
    const char *out_img_path = iftGetConstStrValFromDict("-o", args); // equal result with the key "--output-img" 
    // Since the equalize option is a integer (stored as a long) you should to get it this way
    int equalize_max_value   = iftGetLongValFromDict("--equalize-max-value", args); // equal result with the key "-e" 
    double displacement      = iftGetDblValFromDict("--displacement", args); // equal result with the key "-d" 
    ///////////////////////////////////////////////////////////////////
    
    ///////////// Getting the OPTIONAL options/parameters /////////////
    // These options could not be passed, then it is needed to check if they did firstly
    // 
    // Because it is a boolean and it does not have argument, we can just check if it was passed.
    // It also holds if the user option '-g' is passed instead of '--sobel-grad-mag' 
    bool apply_sobel_mag     = iftDictContainKey("--sobel-grad-mag", args, NULL); 
                            
    // Since this optional has an argument, we must define an initial value to indicate if the option
    // was passed or not. Then, we just check this value when trying to use it. 
    char *copy_path = NULL;
    if (iftDictContainKey("--save-copy", args, NULL))
        copy_path = iftGetStrValFromDict("--save-copy", args);
    ///////////////////////////////////////////////////////////////////


    // YOUR CODE IS HERE!!!
    puts("Required Options");
    printf("Input Image: %s\n", in_img_path);
    printf("Output Image: %s\n", out_img_path);
    printf("Equalize Maximum Value: %d\n", equalize_max_value);
    printf("Displacement: %lf\n", displacement);


    // USING THE OPTIONAL PARAMETERS
    if (apply_sobel_mag)
        puts("Sobel Gradient Magnitude option was passed");
    if (copy_path != NULL)
        puts("Copy option was passed");


    // DESTROYERS
    iftDestroyDict(&args);
    if (copy_path != NULL)
        free(copy_path);

    return 0;
}


iftDict *iftGetArguments(int argc, const char *argv[]) {
    char program_description[2048] = "This is an only a demo program of how to use the Command Line Parser.\n" \
                                     "No image operation is performed actually.";
                                     
    // this array of options, n_opts, and the program description could be defined in the main or as a global variables
    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--input-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Input Image"},
        {.short_name = "-e", .long_name = "--equalize-max-value", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=true, .help="Equalize the Input Image with this maximum value"},
        {.short_name = "-d", .long_name = "--displacement", .has_arg=true, .arg_type=IFT_DBL_TYPE,
         .required=true, .help="Equalize the Input Image with this maximum value"},
        {.short_name = "-o", .long_name = "--output-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Output Pathname where will be saved the image A-B"},
        // since the option below does not have an argument, we can omit the field .arg_type
        {.short_name = "-g", .long_name = "--sobel-grad-mag", .has_arg=false,
         .required=false, .help="Get the Magnitude of the Sobel from the resulting equalized image"},
        {.short_name = "",   .long_name = "--save-copy", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=false, .help="Save a copy of the Resulting Image"}
    };  
    // we could simply assign the number 5 in this case
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

    
    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}
