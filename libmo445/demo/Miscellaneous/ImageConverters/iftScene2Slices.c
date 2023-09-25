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


  if (argc!=4){
    iftError("Usage: iftScene2Slices <scene file (*.scn)> <output basename (name with no extensions)> <cut_type(0:axial|1:coronal|2:sagittal)>","main");
  }
  
  iftImage *img = iftReadImageByExt(argv[1]);
  
  if (!iftIs3DImage(img)){
    iftError("The image must be a 3D image","main");
  }

  char cut_plane=atoi(argv[3]);

  switch (cut_plane){
    case 0:
      for (int z=0; z < img->zsize; z++) {
        iftImage *slice = iftGetXYSlice(img,z);
        char filename[200];
        sprintf(filename,"%s%05d.pgm",argv[2],z);
        iftWriteImageByExt(slice,filename);
        iftDestroyImage(&slice);
      }
      break;
    case 1:
      for (int y=0; y < img->ysize; y++) {
        iftImage *slice = iftGetZXSlice(img,y);
        char filename[200];
        sprintf(filename,"%s%05d.pgm",argv[2],y);
        iftWriteImageByExt(slice,filename);
        iftDestroyImage(&slice);
      }
      break;
    case 2:
      for (int x=0; x < img->xsize; x++) {
        iftImage *slice = iftGetYZSlice(img,x);
        char filename[200];
        sprintf(filename,"%s%05d.pgm",argv[2],x);
        iftWriteImageByExt(slice,filename);
        iftDestroyImage(&slice);
      }
      break;
    default:
      iftError("The cut_type must be one of the following values (0:axial|1:coronal|2:sagittal)>","main");

  }

  iftDestroyImage(&img);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}




