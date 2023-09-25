#include "ift.h"

int main(int argc, const char *argv[]) {
  /*--------------------------------------------------------*/
  size_t MemDinInicial, MemDinFinal;
  MemDinInicial = iftMemoryUsed();

  /*--------------------------------------------------------*/
  if (argc!=3){
    iftError("Used: iftWriteMImageToImage <input> <basename>", "main");
  }

  iftMImage *input = NULL;

  input = iftReadMImage(argv[1]);
  
  iftWriteMImageBands(input, argv[2]);

  iftDestroyMImage(&input);

  /* ---------------------------------------------------------- */

  MemDinFinal = iftMemoryUsed();
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%ld, %ld)\n",
           MemDinInicial,MemDinFinal);


  return 0;
}


