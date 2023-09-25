#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftFImage *fimg;
  iftImage  *img;
  char       filename[200];
  timer     *t1=NULL,*t2=NULL;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc!=7)
    iftError("Usage: iftRawSlicesToScene <basename> <first> <last> <xsize> <ysize> <type= 0- 16bits, 1- 32bits, 2- float>","main");

  
  t1     = iftTic();

  switch(atoi(argv[6])) {

    case 0: // 16-bit slices

      img  = iftReadRawSlices(argv[1],atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5]), 16);

      sprintf(filename,"%s.scn",argv[1]);
      iftWriteImage(img,filename);
      iftDestroyImage(&img);

      break;

    case 1: // 32-bit slices

      img  = iftReadRawSlices(argv[1],atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5]), 32);


      sprintf(filename,"%s.scn",argv[1]);
      iftWriteImage(img,filename);
      iftDestroyImage(&img);
      
      break;

    case 2: // float slices: usually CT projections ranging from [0,4]

      fimg = iftFReadRawSlices(argv[1],atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5]));

      int factor   = 1000;
      float minval = iftFMinimumValue(fimg);
      float maxval = iftFMaximumValue(fimg);
      sprintf(filename,"%s.hdr",argv[1]); 
      FILE *fp=fopen(filename,"w");
      fprintf(fp,"MultiplicationFactor %d \nMinimum %f \nMaximum %f \n",factor,minval,maxval);
      fclose(fp);

      
      img  = iftFImageToImage(fimg,factor*(maxval-minval));
      

      /* sprintf(filename,"%s.npy",argv[1]); */
      /* iftWriteFImage(fimg,filename); */
      /* iftDestroyFImage(&fimg); */

      sprintf(filename,"%s.scn",argv[1]);
      iftWriteImage(img,filename);
      iftDestroyImage(&img);

      break;

    default:
      iftError("Usage: iftRawSlicesToScene <basename> <first> <last> <xsize> <ysize> <type= 0- 16bits and 1- float","main");
    }

  t2     = iftToc();
  fprintf(stdout,"Conversion in %f ms\n",iftCompTime(t1,t2));


  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);

}
