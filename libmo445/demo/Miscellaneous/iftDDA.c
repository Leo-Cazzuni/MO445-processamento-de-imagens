#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftVoxel    u1, u2;
  iftPoint    p1, p2;
  iftAdjRel  *B;
  iftImage   *img;
  iftFeatures   *features;
  char        ext[10],*pos;
  iftColor RGB, YCbCr; 

  RGB.val[0] = 255;  RGB.val[1] = 0;  RGB.val[2] = 255;
  YCbCr      = iftRGBtoYCbCr(RGB);

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  /* Draw line from p1 to p2 and also extract its intensity profile */

  if (argc!=8)
    iftError("Usage: iftDDA <img.[scn,pgm,ppm]> <p1.x> <p1.y> <p1.z> <p2.x> <p2.y> <p2.z>","main");

  pos = strrchr(argv[1],'.') + 1;
  sscanf(pos,"%s",ext);
  
  if (strcmp(ext,"ppm")==0){
    img   = iftReadImageP6(argv[1]);    
    B      = iftCircular(0.0);
  }else{
    if (strcmp(ext,"pgm")==0){
      img   = iftReadImageP5(argv[1]);    
      B      = iftCircular(0.0);
    }else{
      if (strcmp(ext,"scn")==0){
	img   = iftReadImage(argv[1]);    
	B      = iftSpheric(0.0);
	YCbCr.val[0]=iftMaximumValue(img);
      } else {
	fprintf(stderr,"Invalid image format: %s\n",ext);
	exit(-1);
      }
    }
  }

  p1.x = u1.x = atoi(argv[2]);
  p1.y = u1.y = atoi(argv[3]);
  p1.z = u1.z = atoi(argv[4]);

  p2.x = u2.x = atoi(argv[5]);
  p2.y = u2.y = atoi(argv[6]);
  p2.z = u2.z = atoi(argv[7]);

  timer *t1=iftTic();

  features = iftIntensityProfile(img,p1,p2);
  
  timer *t2=iftToc();

  fprintf(stdout,"Intensity profile extracted in %f ms\n",iftCompTime(t1,t2));

  iftWriteFeatures(features,"profile.txt"); 
  iftDestroyFeatures(&features); 

  t1=iftTic();

  iftDrawLine(img,u1,u2,YCbCr,B);
  
  t2=iftToc();

  fprintf(stdout,"Line drawn in %f ms\n",iftCompTime(t1,t2));

  if (strcmp(ext,"ppm")==0){
    iftWriteImageP6(img,"result.ppm"); 
  }else{
    if (strcmp(ext,"pgm")==0){
      iftWriteImageP6(img,"result.ppm"); 
    }else{
      if (strcmp(ext,"scn")==0){
	iftWriteImage(img,"result.scn"); 
      } else {
	fprintf(stderr,"Invalid image format: %s\n",ext);
	exit(-1);
      }
    }
  }

  iftDestroyImage(&img); 
  iftDestroyAdjRel(&B);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}

