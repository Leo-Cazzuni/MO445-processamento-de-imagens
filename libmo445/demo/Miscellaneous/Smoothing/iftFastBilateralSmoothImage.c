#include "ift.h"

int main(int argc, char *argv[])
{
  iftImage  *img[2];
  timer     *t1=NULL,*t2=NULL;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);
  struct mallinfo info;
  int MemDinInicial, MemDinFinal;
  free(trash);
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc!=5){
    iftError("Usage: iftFastBilateralSmoothImage <input.[ppm,pgm,png]> <spatial sigma> <radiometric sigma> <output.[ppm,pgm]>","main");
  }

  img[0]   = iftReadImageByExt(argv[1]);

  t1     = iftTic();

  img[1] = iftFastBilateralFilter2D(img[0],atof(argv[2]), atof(argv[3]));

  t2     = iftToc();
  fprintf(stdout,"Smoothing in %f ms\n",iftCompTime(t1,t2));

  iftDestroyImage(&img[0]);

  iftWriteImageByExt(img[1], argv[4]);

  iftDestroyImage(&img[1]);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
           MemDinInicial,MemDinFinal);

  return(0);

}
