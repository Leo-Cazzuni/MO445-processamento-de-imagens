#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftImage    *img=NULL, *grad=NULL;
  iftAdjRel   *A=NULL;
  char        ext[10],*pos;
  timer       *t1=NULL,*t2=NULL;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc!=4)
    iftError("Usage: iftImageBasins <image> <adj_rel_radius> <output>","main");


  pos = strrchr(argv[1],'.') + 1;
  sscanf(pos,"%s",ext);
  
  img = iftReadImageByExt(argv[1]);

  t1 = iftTic();
  grad = iftImageBasins(img, A);
  
  t2 = iftTic();
  printf("Image basins: %.2f ms\n", iftCompTime(t1, t2));


  pos = strrchr(argv[3],'.') + 1;
  sscanf(pos,"%s",ext);

  if (strcmp(ext,"scn")==0){
    iftWriteImage(grad, argv[3]);
  }
  else if (strcmp(ext,"pgm")==0){
    iftWriteImageP5(grad, argv[3]);
  }
  else{
    printf("Invalid image format: %s\n",ext);
    exit(-1);
  }

  iftDestroyImage(&grad);
  iftDestroyImage(&img);
  iftDestroyAdjRel(&A);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);

}
