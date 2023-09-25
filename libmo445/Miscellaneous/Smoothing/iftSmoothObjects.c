#include "ift.h"


int main(int argc, char *argv[]) 
{
  iftImage       *img[3];
  iftFImage      *weight;
  iftAdjRel      *A=NULL;
  timer          *t1=NULL,*t2=NULL;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/


  if (argc!=6)
    iftError("Usage must be: iftSmoothObjects <original image> <label image> <number of iterations> <smoothing factor> <output image> ","main");

  img[0]   = iftReadImageByExt(argv[1]);    
  A        = iftSpheric(1.0);
  img[2]   = iftImageBasins(img[0],A);
  iftDestroyAdjRel(&A);
  img[1]   = iftReadImageByExt(argv[2]);
  
  t1     = iftTic();
  weight  = iftSmoothWeightImage(img[2],atof(argv[4]));
  iftDestroyImage(&img[2]);
  img[2]  = iftFastSmoothObjects(img[1],weight,atoi(argv[3]));
  t2     = iftToc();
  fprintf(stdout,"object smoothing in %f ms\n",iftCompTime(t1,t2));

  iftWriteImageByExt(img[2],argv[5]);
  
  iftDestroyImage(&img[0]);  
  iftDestroyImage(&img[1]);  
  iftDestroyImage(&img[2]);  
  iftDestroyFImage(&weight); 
  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}




