#include "ift.h"


int main(int argc, char *argv[]) 
{
  iftImage  *img, ***part;
  int        nparts, row, col, xi, yi, xf, yf;
  //  int        nsamples_per_part;
  //  float      sample_spacing;
  int        xsize, ysize, p, q;
  char       filename[200];
  iftVoxel   u;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc!=5)
    iftError("Usage: iftHCluster <image.ppm> <nparts> <nsamples_per_part> <sample spacing>","main");

  img               = iftReadImageByExt(argv[1]);
  nparts            = atoi(argv[2]);
  //  nsamples_per_part = atoi(argv[3]); 
  //   sample_spacing    = atof(argv[4]);

  part              = (iftImage ***)calloc(nparts, sizeof(iftImage **));
  for (row=0; row < nparts; row++) { 
    part[row] = (iftImage **)calloc(nparts, sizeof(iftImage *)); 
  }

  /* Copy image parts to the array of parts */

  xsize   = img->xsize/nparts;
  ysize   = img->ysize/nparts;
  yi      = 0;
  yf      = ysize-1;
  u.z     = 1;

  for (row=0; row < nparts; row++) { 
    xi = 0; xf = xsize-1;
    if (row == nparts-1) { yf = img->ysize-1; }
    for (col=0; col < nparts; col++) {
      if (col == nparts-1){  xf = img->xsize-1; }
      part[row][col] = iftCreateImage(xf-xi+1,yf-yi+1,1); 
      iftSetCbCr(part[row][col],128); 
      q = 0;
      for (u.y=yi; u.y <= yf; u.y++)  
	for (u.x=xi; u.x <= xf; u.x++){  
	  p = iftGetVoxelIndex(img,u);
	  part[row][col]->val[q] = img->val[p];  
	  part[row][col]->Cb[q]  = img->Cb[p];  
	  part[row][col]->Cr[q]  = img->Cr[p];  
	  q++;
	}  
      xi = xf+1; xf += xsize;
      sprintf(filename,"part_%03d_%03d.ppm",row,col);
      iftWriteImageP6(part[row][col],filename);
      iftDestroyImage(&part[row][col]);
    }
    yi = yf+1; yf += ysize;
  }

  iftDestroyImage(&img);

  for (row=0; row < nparts; row++) { 
    free(part[row]);
  }
  free(part);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}

