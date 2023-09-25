#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftImage  *img;
  char      filename[200];
  FILE      *fp;
  int        xsize, ysize, zsize;
  timer     *t1=NULL,*t2=NULL;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc!=4)
    iftError("Usage: iftRawSceneToScene <basename> <header_of_the_scene_format.txt> <multiplication factor>","main");
  
  t1       = iftTic();

  fp = fopen(argv[2],"r");

  if (fscanf(fp,"%d %d %d",&xsize,&ysize,&zsize)!=3) 
    iftError("Reading error","iftRawSceneToScene");


  iftFImage *fimg;

  fimg = iftCreateFImage(xsize,ysize,zsize);

  if (fscanf(fp,"%f %f %f",&fimg->dx,&fimg->dy,&fimg->dz)!=3) 
    iftError("Reading error","iftRawSceneToScene");


    
  fclose(fp);
  
  sprintf(filename,"%s.raw",argv[1]);
  fp = fopen(filename,"rb");
  
  if(fread(fimg->val,sizeof(float),fimg->n,fp)!=fimg->n) 
    iftError("Reading error","iftRawSceneToScene");
  
  fclose(fp);

  img      = iftCreateImage(fimg->xsize,fimg->ysize,fimg->zsize);
  img->dx  = fimg->dx;
  img->dy  = fimg->dy;
  img->dz  = fimg->dz;
    
  float minval = iftFMinimumValue(fimg);
  float maxval = iftFMaximumValue(fimg);

  sprintf(filename,"%s.hdr",argv[1]); 
  fp=fopen(filename,"w");
  fprintf(fp,"MultiplicationFactor %d \nMinimum %f \nMaximum %f \n",atoi(argv[3]),minval,maxval);
  fclose(fp);

      
  img  = iftFImageToImage(fimg,atoi(argv[3])*(maxval-minval));
     
  iftMinimumValue(img);
  iftMaximumValue(img);

  sprintf(filename,"%s.scn",argv[1]); 
  iftWriteImage(img,filename);

  iftDestroyFImage(&fimg);  
  iftDestroyImage(&img);  

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
