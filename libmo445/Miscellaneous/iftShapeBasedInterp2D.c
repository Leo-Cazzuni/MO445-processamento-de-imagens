#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftImage            *label=NULL, *nlabel=NULL;
  char                 ext[10],*pos;
  timer               *t1=NULL,*t2=NULL;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc!=5)
    iftError("Usage: iftShapeBasedInterp <label.pgm> <interp.pgm> <sx> <sy>","main");

  pos = strrchr(argv[1],'.') + 1;
  sscanf(pos,"%s",ext);
  
  if (strcmp(ext,"pgm")==0){
    label   = iftReadImageP5(argv[1]); 
  }else{
    printf("Invalid image format: %s\n",ext);
    exit(-1);
  }

  t1 = iftTic(); 
  
  nlabel  = iftShapeBasedInterp2D(label,atof(argv[3]),atof(argv[4]));

  t2     = iftToc();

  iftWriteImageP5(nlabel,argv[2]);

  iftDestroyImage(&label);
  iftDestroyImage(&nlabel);

  fprintf(stdout,"Interpolation in %f ms\n",iftCompTime(t1,t2));

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}




