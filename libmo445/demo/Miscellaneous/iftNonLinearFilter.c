#include "ift.h"

int main(int argc, char *argv[])
{
  iftImage  *img[3];
  iftKernel *K;
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

  if (argc!=4){
    printf("Operation:\n");
    printf("0: Median \n");
    printf("1: Moda   \n");
    printf("2: Dilation \n");
    printf("3: Erosion  \n");
    printf("4: Closing  \n");
    printf("5: Opening  \n");
    iftError("Usage must be: iftNonLinearFilter <image.[ppm,pgm,scn]> <kernel,txt> <operation>","main");
  }

  pos = strrchr(argv[1],'.') + 1;
  sscanf(pos,"%s",ext);

  /* iftAdjRel *A=iftCircular(10.0); */
  /* K = iftCreateKernel(A); */
  /* iftWriteKernel(K,"planar_kernel.txt"); */


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

  if (strcmp(ext,"txt")==0){
    K = iftReadKernel(argv[2]);
  }else{
    printf("Invalid image format: %s\n",ext);
    exit(-1);
  }


  t1     = iftTic();

  switch(atoi(argv[3])){
  case 0:    
    img[1] = iftMedianFilter(img[0],K->A);
    break;
  case 1:
    img[1] = iftModaFilter(img[0],K->A);
    break;
  case 2:
    img[1] = iftDilateWithKernel(img[0],K);
    break;
  case 3:
    img[1] = iftErodeWithKernel(img[0],K);
    break;
  case 4:
    img[1] = iftCloseWithKernel(img[0],K);
    break;
  case 5:
    img[1] = iftOpenWithKernel(img[0],K);
    break;
  default:
    printf("Operation:\n");
    printf("0: Median \n");
    printf("1: Moda   \n");
    printf("2: Dilation \n");
    printf("3: Erosion  \n");
    printf("4: Closing  \n");
    printf("5: Opening  \n");
    iftError("Usage must be: iftNonLinearFilter <image.[pgm,scn]> <kernel.txt> <operation>","main");
  }

  t2     = iftToc();
  fprintf(stdout,"Non-Linear filtering in %f ms\n",iftCompTime(t1,t2));


  pos = strrchr(argv[1],'.') + 1;
  sscanf(pos,"%s",ext);

  if (strcmp(ext,"scn")==0){
    iftWriteImage(img[1],"result.scn");
  }else{
    if (strcmp(ext,"pgm")==0){
      iftWriteImageP2(img[1],"result.pgm");
    }else{
      if (strcmp(ext,"ppm")==0){
	iftWriteImageP2(img[1],"result.pgm");
      }else{
	printf("Invalid image format: %s\n",ext);
	exit(-1);
      }
    }
  }

  iftDestroyImage(&img[0]);
  iftDestroyImage(&img[1]);
  iftDestroyKernel(&K);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);

  return(0);

}
