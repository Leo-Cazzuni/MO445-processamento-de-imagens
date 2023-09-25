#include "ift.h"


int main(int argc, char *argv[]) 
{
  iftImage       *img[3];
  iftAdjRel      *A, *B;
  timer          *t1=NULL,*t2=NULL;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/


  if (argc!=4)
    iftError("Usage: iftEnhanceEdges <image.[ppm,png,pgm,scn]> <adjacency radius> <result.[pgm,scn]>","main");

  img[0] = iftReadImageByExt(argv[1]);    
  if (iftIs3DImage(img[0])){
    A      = iftSpheric(atof(argv[2]));
    B      = iftSpheric(1.8);
  }else{
    A      = iftCircular(atof(argv[2]));
    B      = iftCircular(1.5);
  }

  t1     = iftTic();
  
  img[1]   = iftMedianFilter(img[0],B);
  img[2]   = iftImageBasins(img[1],A);

  t2     = iftToc();

  fprintf(stdout,"Edges enhanced in %f ms\n",iftCompTime(t1,t2));

  iftWriteImageByExt(img[2],argv[3]);
  
  iftDestroyImage(&img[0]);
  iftDestroyImage(&img[1]);
  iftDestroyImage(&img[2]);
  iftDestroyAdjRel(&A);
  iftDestroyAdjRel(&B);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}

