#include "ift.h"

int main(int argc, char **argv) {
  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  iftImage *input = NULL, *output = NULL;
  iftAdjRel *A = NULL;

  if(argc != 3)
    iftError("usage: %s <input mask> <output mask>", "main", argv[0]);
  
  input = iftReadImageByExt(argv[1]);
  
  if(iftIs3DImage(input))
    A = iftSpheric(1.0);
  else
    A = iftCircular(1.5);

  output = iftSelectLargestComp(input, A);
  
  iftWriteImageByExt(output, argv[2]);

  iftDestroyImage(&input);
  iftDestroyImage(&output);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return 0;
}
