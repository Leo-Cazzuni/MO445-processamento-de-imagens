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

  if (argc!=6)
    iftError("Usage: iftShapeBasedInterp <label.scn> <interp.scn> <sx> <sy> <sz>","main");

  pos = strrchr(argv[1],'.') + 1;
  sscanf(pos,"%s",ext);
  
  if (strcmp(ext,"scn")==0){
    label   = iftReadImage(argv[1]);    
  }else{
    printf("Invalid image format: %s\n",ext);
    exit(-1);
  }

  t1 = iftTic(); 
  
  nlabel  = iftShapeBasedInterp(label,atof(argv[3]),atof(argv[4]),atof(argv[5]));

  t2     = iftToc();

  iftWriteImage(nlabel,argv[2]);

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




