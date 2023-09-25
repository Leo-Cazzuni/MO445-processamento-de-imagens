#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftImage *img,*rimg,*simg;
  char  ext[10],*pos;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc!=5)
    iftError("Usage: iftGeometricTrans2D <image.[pgm,ppm]> <angle> <sx> <sy>","main");

  pos = strrchr(argv[1],'.') + 1;
  sscanf(pos,"%s",ext);
  
  if (strcmp(ext,"ppm")==0){
    img   = iftReadImageP6(argv[1]);    
  }else{
    if (strcmp(ext,"pgm")==0){
      img   = iftReadImageP5(argv[1]);    
    }else{
      printf("Invalid image format: %s\n",ext);
      exit(-1);
    }
  }
  
  simg = iftScaleImage2D(img,atof(argv[3]),atof(argv[4]));
  rimg = iftRotateImage2D(simg,atof(argv[2]));

  if (strcmp(ext,"ppm")==0){  
    iftWriteImageP6(rimg,"geometric.ppm");
  }else{
    iftWriteImageP5(rimg,"geometric.pgm");
  }
  iftDestroyImage(&rimg);
  iftDestroyImage(&simg);
  iftDestroyImage(&img);
  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);

}
