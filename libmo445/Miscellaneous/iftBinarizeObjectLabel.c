#include "ift.h"

int main(int argc, char **argv) {
  iftImage *input = NULL, *output = NULL;

  if(argc != 4)
    iftError("usage: %s <input mask> <object id> <output mask>", "main", argv[0]);
  
  input = iftReadImageByExt(argv[1]);
  output = iftThreshold(input, atoi(argv[2]), atoi(argv[2]), 1);
  
  iftWriteImageByExt(output, argv[3]);

  iftDestroyImage(&input);
  iftDestroyImage(&output);

  return 0;
}
