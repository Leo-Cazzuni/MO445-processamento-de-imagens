#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftImage       *bin=NULL, *gt=NULL;
  char            ext[10],*pos;

  /*--------------------------------------------------------*/

  int MemDinInicial, MemDinFinal;
  MemDinInicial = iftMemoryUsed();

  /*--------------------------------------------------------*/


  if (argc!=4)
      iftError("Usage: iftFBetaScore <binary.[scn,pgm]> <groundtruth.[scn,pgm]> <beta>", "main");


  pos = strrchr(argv[1],'.') + 1;
  sscanf(pos,"%s",ext);
  
  if (strcmp(ext,"scn")==0){
    bin   = iftReadImage(argv[1]);    
  }else{
    if (strcmp(ext,"pgm")==0){
      bin   = iftReadImageP5(argv[1]);    
    }else{
      printf("Invalid image format: %s\n",ext);
      exit(-1);
    }
  }

  pos = strrchr(argv[2],'.') + 1;
  sscanf(pos,"%s",ext);
  
  if (strcmp(ext,"scn")==0){
    gt   = iftReadImage(argv[2]);    
  }else{
    if (strcmp(ext,"pgm")==0){
      gt   = iftReadImageP5(argv[2]);    
    }else{
      printf("Invalid image format: %s\n",ext);
      exit(-1);
    }
  }
  
  printf("FBeta score (beta=%f) for %s is %f\n",atof(argv[3]),argv[1],iftFBetaScore(bin,gt,atof(argv[3])));

  iftDestroyImage(&bin);
  iftDestroyImage(&gt);

  /* ---------------------------------------------------------- */

  MemDinFinal = iftMemoryUsed();
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}
