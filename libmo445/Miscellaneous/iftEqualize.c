#include "ift.h"

char *Basename(char *path)
{
  char *basename     = iftBasename(path);
  iftSList *slist    = iftSplitString(basename,"/");
  strcpy(basename,slist->tail->elem);
  iftDestroySList(&slist);
  return(basename);
}

int main(int argc, char *argv[]) 
{
  iftImage       *img=NULL, *eimg=NULL;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/


  if (argc!=3)
    iftError("Usage: iftEqualize <input.[pgm,ppm,scn]> <max_val>","main");


  img  = iftReadImageByExt(argv[1]);
  eimg = iftEqualize(img, 255);
  char filename[200];
  char *basename = Basename(argv[1]);
  if (iftIs3DImage(img))
    sprintf(filename,"%s-eql.nii.gz",basename);
  else
    sprintf(filename,"%s-eql.png",basename);

  iftWriteImageByExt(eimg,filename);

  iftFree(basename);
  iftDestroyImage(&img);
  iftDestroyImage(&eimg);
  
  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}



