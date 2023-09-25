#include "ift.h"

iftDict *iftGetArgumentsFromCmdLine(int argc, const char **argv);

int main(int argc, const char **argv) {
  iftImage *img = NULL;
  iftDict *args = NULL;
  const char *img_path = NULL;

  args = iftGetArgumentsFromCmdLine(argc, argv);

  img_path = iftGetConstStrValFromDict("--input_img", args);
  img = iftReadImageByExt(img_path);

  printf("Image file: %s\n", img_path);
  printf("Image size: %d %d %d\nIs colored? %d\nMaximum value: %d\nMinimum value: %d\nVoxel size: %f %f %f\nBPP: %d\n",
         img->xsize, img->ysize, img->zsize, iftIsColorImage(img), iftMaximumValue(img), iftMinimumValue(img),
         img->dx, img->dy, img->dz, iftRadiometricResolution(img));

  iftDestroyImage(&img);
  iftDestroyDict(&args);

  return 0;
}


iftDict *iftGetArgumentsFromCmdLine(int argc, const char **argv) {
  iftCmdLineOpt cmd_line_opts[] = {
          {.short_name = "-i", .long_name = "--input_img", .has_arg=true, .arg_type=IFT_STR_TYPE,
                  .required=true, .help="Input image"}
  };
  int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);


  char program_description[IFT_STR_DEFAULT_SIZE] = "This program loads a set of file names and samples it according to a criterion.";

  // Parser Setup
  iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
  iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
  iftDestroyCmdLineParser(&parser);

  return args;
}