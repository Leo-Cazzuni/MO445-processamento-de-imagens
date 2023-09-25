#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftImage            *img=NULL, *nimg=NULL;
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

  if (argc!=4)
    iftError("Usage: iftChangeAxes <input.scn> <output.scn> <0- X by Z, 1- Y by Z, 2- X by Y, 3- Invert X, 4- Invert Y, 5- Invert Z>","main");

  pos = strrchr(argv[1],'.') + 1;
  sscanf(pos,"%s",ext);
  
  if (strcmp(ext,"scn")==0){
    img   = iftReadImage(argv[1]);    
  }else{
    printf("Invalid image format: %s\n",ext);
    exit(-1);
  }

  t1 = iftTic(); 
  
  switch (atoi(argv[3])) {
  case 0:
    nimg  = iftSwitchXByZ(img);
    break;
  case 1:
    nimg  = iftSwitchYByZ(img);
    break;
  case 2:
    nimg  = iftSwitchXByY(img);
    break;
  case 3:
    nimg  = iftInvertX(img);
    break;
  case 4:
    nimg  = iftInvertY(img);
    break;
  case 5:
    nimg  = iftInvertZ(img);
    break;
  default:
    iftError("Usage: iftChangeAxes <input.scn> <output.scn> <0- X by Z, 1- Y by Z, 2- X by Y, 3- Invert X, 4- Invert Y, 5- Invert Z>","main");
  }

  t2     = iftToc();

  iftWriteImage(nimg,argv[2]);

  iftDestroyImage(&img);
  iftDestroyImage(&nimg);

  fprintf(stdout,"Orientation changed in %f ms\n",iftCompTime(t1,t2));

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDynamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}




