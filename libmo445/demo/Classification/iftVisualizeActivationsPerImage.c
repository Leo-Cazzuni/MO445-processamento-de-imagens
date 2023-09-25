#include "ift.h"

void SaveResults(char *dir, iftMImage *mimg, char *basename)
{
  char filename[200], ext[10];
  int  Imax;

  if (iftIs3DMImage(mimg)){
    sprintf(ext,"scn");
    Imax = 4095;
  }else{
    sprintf(ext,"png");
    Imax = 255;
  }
	
  for (int b = 0; b < mimg->m; b++) {
    iftImage *img = iftMImageToImage(mimg, Imax, b);
    sprintf(filename, "%s/%s_kernel_%03d.%s", dir, basename, b + 1,ext);
    iftWriteImageByExt(img, filename);
    iftDestroyImage(&img);
  }
}

int main(int argc, char *argv[])
{
    timer *tstart = NULL;
    int MemDinInicial, MemDinFinal;

    MemDinInicial = iftMemoryUsed(1);

    if (argc != 3)
        iftError("Usage: iftVisualizeActivationsPerImage <...>\n"
            "[1] input mimage with the activations per band (kernel) \n"
            "[2] output directory with the activation images of each band (kernel) \n",
            "main");

  tstart = iftTic();

  iftMImage  *mimg    = iftReadMImage(argv[1]);
  char   *basename    = iftFilename(argv[1],".mimg");

  SaveResults(argv[2],mimg,basename);
  iftDestroyMImage(&mimg);
  
  puts("\nDone...");
  puts(iftFormattedTime(iftCompTime(tstart, iftToc())));
  
  MemDinFinal = iftMemoryUsed();
  iftVerifyMemory(MemDinInicial, MemDinFinal);

  return(0);
}
