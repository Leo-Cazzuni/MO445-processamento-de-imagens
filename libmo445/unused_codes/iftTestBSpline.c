#include <ift.h>


int main(int argc, char *argv[]) 
{
  iftImage *img = iftCreateImage(4,1,1); 

  img->val[0] = 15; 
  img->val[1] = 20; 
  img->val[2] = 30; 
  img->val[3] = 25; 

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  iftFree(trash);
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/


  if (argc!=1)
    iftError("Usage: iftTestBSpline ","main");


  iftMatrix *Bm = iftBlendingMatrix(0.5);
  
  iftPrintMatrix(Bm);

  iftVoxel U[4]; 
  
  for (int p=0; p < 4; p++) {
    U[p] = iftGetVoxelCoord(img,p);
  }
  
  iftMatrix *Pm = iftGrayImagePointMatrix(img,U);

  iftPrintMatrix(Pm);
  
  iftMatrix *P = iftBSplineInterp(0.5,Bm,Pm);
  iftPrintMatrix(P);


  iftDestroyMatrix(&Bm);
  iftDestroyMatrix(&Pm);
  iftDestroyMatrix(&P);
  iftDestroyImage(&img);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}

