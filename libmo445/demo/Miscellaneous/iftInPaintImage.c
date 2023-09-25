#include "ift.h"


int main(int argc, char *argv[]) 
{
  iftImage  *img[3];
  char       ext[10],*pos;
  timer     *t1=NULL,*t2=NULL;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc!=5){
    iftError("Usage: iftInPainting <image.[ppm,pgm,scn]> <mask.[pgm,scn]> <adj_radius> <result.[ppm.pgm,scn]>","main");
  }

  pos = strrchr(argv[1],'.') + 1;
  sscanf(pos,"%s",ext);
  
  if (strcmp(ext,"scn")==0){
    img[0]   = iftReadImage(argv[1]);    
  }else{
    if (strcmp(ext,"pgm")==0){
      img[0]   = iftReadImageP5(argv[1]);    
    }else{
      if (strcmp(ext,"ppm")==0){
	img[0]   = iftReadImageP6(argv[1]);    
      }else{
	printf("Invalid image format: %s\n",ext);
	exit(-1);
      }
    }
  }

  pos = strrchr(argv[2],'.') + 1;
  sscanf(pos,"%s",ext);
  
  if (strcmp(ext,"scn")==0){
    img[1]   = iftReadImage(argv[2]);    
  }else{
    if (strcmp(ext,"pgm")==0){
      iftImage *tmp = iftReadImageP5(argv[2]);    
      img[1] = iftThreshold(tmp, 1, IFT_INFINITY_INT, 1);
      iftDestroyImage(&tmp);
    }else{
      printf("Invalid image format: %s\n",ext);
      exit(-1);
    }
  }


  t1     = iftTic();

  img[2] = iftInpaintingFromSamplesAroundMaskBoundaryOutside(img[0],img[1],atof(argv[3]));
  //img[2] = iftInPaintBoundary(img[0],img[1]);
  

  t2     = iftToc();
  fprintf(stdout,"Inpainting in %f ms\n",iftCompTime(t1,t2));

  iftDestroyImage(&img[0]);
  iftDestroyImage(&img[1]);

  

  pos = strrchr(argv[1],'.') + 1;
  sscanf(pos,"%s",ext);
  
  if (strcmp(ext,"scn")==0){
    iftWriteImage(img[2],argv[4]);
  }else{
    if (strcmp(ext,"pgm")==0){
      iftWriteImageP5(img[2],argv[4]);
    }else{
      if (strcmp(ext,"ppm")==0){
  	iftWriteImageP6(img[2],argv[4]);
      }else{
  	printf("Invalid image format: %s\n",ext);
  	exit(-1);
      }
    }
  }

  iftDestroyImage(&img[2]);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);

}
