#include "ift.h"

int main(int argc, char *argv[])
{
  iftImage            *img=NULL, *simg=NULL;
  float                f1, f2, g1, g2;
  timer               *t1=NULL,*t2=NULL;

    /*--------------------------------------------------------*/
    size_t MemDinInicial, MemDinFinal;
    MemDinInicial = iftMemoryUsed();

    /*--------------------------------------------------------*/

  if (argc!=7)
      iftError("Usage: iftLinearStretch <image> <stretched> <input_Imin> <input_Imax> <output_Imin> <output_Imax>",
               "main");

  img = iftReadImageByExt(argv[1]);

  f1 = atof(argv[3]);
  f2 = atof(argv[4]);
  g1 = atof(argv[5]);
  g2 = atof(argv[6]);

  t1 = iftTic();

  simg  = iftLinearStretch(img,f1,f2,g1,g2);

  t2     = iftToc();

  iftWriteImageByExt(simg,argv[2]);

  iftDestroyImage(&img);
  iftDestroyImage(&simg);

  fprintf(stdout,"Image stretched in %f ms\n",iftCompTime(t1,t2));

    /* ---------------------------------------------------------- */

    MemDinFinal = iftMemoryUsed();
    if (MemDinInicial!=MemDinFinal)
        printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
               MemDinInicial,MemDinFinal);

  return(0);
}




