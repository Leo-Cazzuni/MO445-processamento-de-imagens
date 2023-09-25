#include "ift.h"


int main(int argc, char *argv[]) 
{
  iftImage *img1,*img2;
  char      filename[200];

  /*--------------------------------------------------------*/

  if (argc!=3)
    iftError("Usage: iftColorCoding <input.[scn,pgm,png]> <output basename>","main");

  img1 = iftReadImageByExt(argv[1]);
  img2 = iftColorCoding(img1);

  if (iftIs3DImage(img2)){
    for (int z=0; z < img2->zsize; z++) {
      iftImage *slice = iftGetXYSlice(img2,z);
      sprintf(filename,"%s%04d.png",argv[2],z);
      iftWriteImageByExt(slice,filename);
      iftDestroyImage(&slice);
    }
  } else {
    sprintf(filename,"%s.png",argv[2]);
    iftWriteImageByExt(img2,filename);
  }

  float value[256];
  for (int p=0; p < 256; p++) 
    value[p]=p/255.0;
  for (int p=0; p < 256; p++) {
    float R,G,B;
    iftHeatColorMapping(value[p],&R,&G,&B);
    printf("%d %d %d\n",(int)(255*R),(int)(255*G),(int)(255*B));
  }

  iftDestroyImage(&img1);  
  iftDestroyImage(&img2);
 
  /* ---------------------------------------------------------- */


    
  return(0);
}



