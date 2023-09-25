#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftMatrix *U,*S,*Vt,*A;
  iftDataSet *Z, *Zc;
  iftImage *bin,*rbin,*nbin;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/


  if (argc!=3)
    iftError("Usage: iftSVD <binary.pgm> <angle>","main");

  
  bin  = iftReadImageByExt(argv[1]);    
  rbin = iftRotateImage2D(bin,atof(argv[2])); // this is just to move
					      // the figure out from
					      // aligment in case it
					      // is already aligned to
					      // the x axis.
  
  //  iftWriteImageP2(rbin,"teste.pgm");

  Z   = iftObjectToDataSet(rbin);
  iftSetStatus(Z,IFT_TRAIN);

  Zc  = iftCentralizeDataSet(Z);

  timer *t1=iftTic();

  A = iftDatasetCovarianceMatrix(Zc);
   

  iftSingleValueDecomp(A,&U,&S,&Vt);

  
  timer *t2=iftToc();

  printf("%f ms\n",iftCompTime(t1,t2));


  iftPrintMatrix(U);
  iftPrintMatrix(S);
  iftPrintMatrix(Vt);
  

  nbin = iftTransformImageByMatrix(rbin,Vt);
    
  iftWriteImageByExt(nbin,"main-axis-aligned-with-x-axis.pgm");
  
  iftDestroyImage(&bin);
  iftDestroyImage(&rbin);
  iftDestroyImage(&nbin);
  iftDestroyDataSet(&Z);
  iftDestroyDataSet(&Zc);
  iftDestroyMatrix(&A);
  iftDestroyMatrix(&U);
  iftDestroyMatrix(&S);
  iftDestroyMatrix(&Vt);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}

