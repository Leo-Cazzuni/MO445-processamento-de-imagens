#include "ift.h"

iftDict *iftGetArguments(int argc, const char *argv[]);

int main(int argc, const char *argv[]) {
  /*--------------------------------------------------------*/
  size_t MemDinInicial, MemDinFinal;
  MemDinInicial = iftMemoryUsed();

  /*--------------------------------------------------------*/

  iftImage *input = NULL, *output = NULL;
  iftDict *args = NULL;
  int vi, vf, value;

  args = iftGetArguments(argc, argv);

  input = iftReadImageByExt(iftGetConstStrValFromDict("--input-file", args));

  if(iftDictContainKey("--otsu", args, NULL)) {
    vi = iftOtsu(input);
    vf = IFT_INFINITY_INT;
  } else {
    if(!iftDictContainKey("--vi", args, NULL) || !iftDictContainKey("--vf", args, NULL)){
        iftError("Values --vi and --vf are mandatory when not using Otsu", "main");
    }

    vi = (int) iftGetLongValFromDict("--vi", args);
    vf = (int) iftGetLongValFromDict("--vf", args);
  }

  value = (int) iftGetLongValFromDict("--output-value", args);

  output = iftThreshold(input, vi, vf, value);
  
  iftWriteImageByExt(output, iftGetConstStrValFromDict("--output-file", args));

  iftDestroyImage(&input);
  iftDestroyImage(&output);

  /* ---------------------------------------------------------- */

  MemDinFinal = iftMemoryUsed();
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%ld, %ld)\n",
           MemDinInicial,MemDinFinal);


  return 0;
}


iftDict *iftGetArguments(int argc, const char *argv[]) {
  char program_description[2048] = "This is a demo program to demonstrate the usage of the in-memory compression interface." \
                                        "for the zlib library";

  // this array of options, n_opts, and the program description could be defined in the main or as a global variables
  iftCmdLineOpt cmd_line_opts[] = {
          {.short_name = "-i", .long_name = "--input-file", .has_arg=true, .arg_type=IFT_STR_TYPE, .required=true, .help="Input image file"},
          {.short_name = "-o", .long_name = "--output-file", .has_arg=true, .arg_type=IFT_STR_TYPE, .required=true, .help="Output image file"},
          {.short_name = "", .long_name = "--vi", .has_arg=true, .arg_type=IFT_LONG_TYPE, .required=false, .help="First value"},
          {.short_name = "", .long_name = "--vf", .has_arg=true, .arg_type=IFT_LONG_TYPE, .required=false, .help="Second value"},
          {.short_name = "", .long_name = "--otsu", .has_arg=false, .arg_type=IFT_LONG_TYPE, .required=false, .help="Compute via Otsu"},
          {.short_name = "", .long_name = "--output-value", .has_arg=true, .arg_type=IFT_LONG_TYPE, .required=true, .help="Output value"}
  };
  // we could simply assign the number 5 in this case
  int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);


  // Parser Setup
  iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
  iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
  iftDestroyCmdLineParser(&parser);

  return args;
}
