#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftImage  *gt_colors=NULL, *orig=NULL;
  iftImage  *gt_borders=NULL, *gt_labels=NULL;
  char       new_color;
  int        ncolors=0, i=0;
  iftColor   YCbCr[100];

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc != 5)
    iftError("Usage: gt-images-from-colors <orig.ppm> <gt_color.ppm> <gt_borders.pgm> <gt_labels.pgm>","main");

  orig       = iftReadImageP6(argv[1]);
  gt_colors  = iftReadImageP6(argv[2]);

  gt_labels  = iftCreateImage(gt_colors->xsize,gt_colors->ysize,gt_colors->zsize);

  for (i=0; i < 100; i++) 
    YCbCr[i].val[0]=YCbCr[i].val[1]=YCbCr[i].val[2]=IFT_NIL;

  /* Find the number of colors up to 100 colors */

  for (int p=0; p < gt_colors->n; p++) {
    new_color=1;
    for (i=0; (i < ncolors)&&(new_color==1); i++)
      if ((YCbCr[i].val[0]==gt_colors->val[p])&&
	  (YCbCr[i].val[1]==gt_colors->Cb[p])&&
	  (YCbCr[i].val[2]==gt_colors->Cr[p])){
	new_color=0;
      }
    if (new_color){
      YCbCr[i].val[0]=gt_colors->val[p];
      YCbCr[i].val[1]=gt_colors->Cb[p];
      YCbCr[i].val[2]=gt_colors->Cr[p];
      ncolors++;
    }	
  }

  printf("ncolors %d\n",ncolors);

  /* Create label image */

  for (int p=0; p < gt_colors->n; p++) {
    for (i=0; i < ncolors; i++)
      if ((YCbCr[i].val[0]==gt_colors->val[p])&&
	  (YCbCr[i].val[1]==gt_colors->Cb[p])&&
	  (YCbCr[i].val[2]==gt_colors->Cr[p])){
	gt_labels->val[p]=i+1;
      }
  }

  /* Compute border image */

  gt_borders  = iftBorderImage(gt_labels);
  for (int p=0; p < gt_colors->n; p++) {
    if (gt_borders->val[p]!=0)
      gt_borders->val[p]=255;
  }
  iftWriteImageP2(gt_labels,argv[4]);  
  iftWriteImageP5(gt_borders,argv[3]);
  
  /* Draw gt_borders over the original image */

  iftAdjRel *A = iftCircular(0.0);
  iftColor C1, C2;
  C1.val[0] = 255; C1.val[1] = 0; C1.val[2] = 255;
  C2 = iftRGBtoYCbCr(C1, 255);
  iftDrawBorders(orig,gt_borders,A,C2,A);
  iftWriteImageP6(orig,"orig-with-borders.ppm");
  iftDestroyAdjRel(&A);

  /* Free memory */

  iftDestroyImage(&orig);
  iftDestroyImage(&gt_borders);
  iftDestroyImage(&gt_labels);
  iftDestroyImage(&gt_colors);

    /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
     MemDinInicial,MemDinFinal);   

  return(0);
}
