#include "ift.h"


int main(int argc, char *argv[]) 
{

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/


  if (argc<6){
    iftError("Usage: iftSlices2Scene <basename> <ext> <first> <last> <output scene (*.[scn,nii.gz])> [linear stretch max value]","main");
  }
  
  int  first = atoi(argv[3]);
  int  last  = atoi(argv[4]);
  char filename[200];
  sprintf(filename,"%s%06d.%s",argv[1],first,argv[2]);  
  iftImage *img = iftReadImageByExt(filename); 
  int  xsize = img->xsize, ysize = img->ysize, zsize = last-first+1;
  iftDestroyImage(&img);
  img   = iftCreateImage(xsize,ysize,zsize);
  for (int z=0; z < img->zsize; z++) {    
    sprintf(filename,"%s%06d.%s",argv[1],z+1,argv[2]);
    fprintf(stderr,"Reading %s\n",filename);
    iftImage *slice = iftReadImageByExt(filename);
    iftPutXYSlice(img,slice,z);
    iftDestroyImage(&slice);
  }

  if(argc >6) {
    iftImage *stretch = iftLinearStretch(img, iftMinimumValue(img), iftMaximumValue(img), 0, atoi(argv[6]));
    iftDestroyImage(&img);
    img = stretch;
  }

  iftWriteImageByExt(img,argv[5]);
  iftDestroyImage(&img);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}




