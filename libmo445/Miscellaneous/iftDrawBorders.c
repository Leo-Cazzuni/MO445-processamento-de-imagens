#include "ift.h"

iftImage *DrawSupervoxelBorder(const iftImage *img, const iftImage *label, bool get_margins, int border_value)
{
 iftAdjRel *A;
 iftImage  *border = iftCopyImage(img);
 int        p,q,i; 
 iftVoxel   u, v;
    
  if (iftIs3DImage(label))
    A = iftSpheric(1.0);
  else
    A = iftCircular(1.0);

  if (get_margins){
    for(p=0; p < label->n; p++){
      u = iftGetVoxelCoord(label, p);
      for(i=1; i < A->n; i++){
        v = iftGetAdjacentVoxel(A,u,i);
        if (iftValidVoxel(label, v)){
          q = iftGetVoxelIndex(label, v);
          if (label->val[p] > label->val[q]){
            border->val[p] = border_value;
            break;
          }
        } else {
          border->val[p] = border_value;
        }
      }
    }
  }
  else{
    for(p=0; p < label->n; p++) {
      u = iftGetVoxelCoord(label, p);
      for (i = 1; i < A->n; i++) {
        v = iftGetAdjacentVoxel(A, u, i);
        if (iftValidVoxel(label, v)) {
          q = iftGetVoxelIndex(label, v);
          if (label->val[p] > label->val[q]) {
            border->val[p] = border_value;
            break;
          }
        }
      }
    }
  }

    iftDestroyAdjRel(&A);
    return(border);
}

int main(int argc, char *argv[]) 
{
  iftImage       *img=NULL, *label=NULL, *border=NULL;
  iftAdjRel      *A, *B;
  iftColor        RGB, YCbCr;
  int             normvalue;
  timer          *t1=NULL,*t2=NULL;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/


  if (argc!=8)
    iftError("Usage: iftDrawBorders <orig.[pgm,png,scn]> <label.[pgm,png,scn]> <out.[pgm,png,scn]> <Red [0,1]> <Green [0,1]> <Blue [0,1]> <thickness [1,)>","main");

  t1 = iftTic(); 

  img        = iftReadImageByExt(argv[1]);
  label      = iftReadImageByExt(argv[2]);
  if (iftIs3DImage(img)){
    if ((label->xsize!=img->xsize)||(label->ysize!=img->ysize)||(label->zsize!=img->zsize)) {
      float sx,sy,sz;
      sz = (float)label->zsize/(float)img->zsize;
      sx = (float)label->xsize/(float)img->xsize;
      sy = (float)label->ysize/(float)img->ysize;
      iftImage *interp_img = iftInterp(img, sx, sy, sz);
      iftDestroyImage(&img);
      img = interp_img;
    }
  } else {
    if ((label->xsize!=img->xsize)||(label->ysize!=img->ysize)) {
      float sx,sy;
      sx = (float)label->xsize/(float)img->xsize;
      sy = (float)label->ysize/(float)img->ysize;
      iftImage *interp_img = iftInterp2D(img, sx, sy);
      iftDestroyImage(&img);
      img = interp_img;
    }
  }

  normvalue  =  iftNormalizationValue(iftMaximumValue(img)); 
  RGB.val[0] = normvalue*atof(argv[4]);
  RGB.val[1] = normvalue*atof(argv[5]);
  RGB.val[2] = normvalue*atof(argv[6]);

  if (iftIs3DImage(label)){
    border     = DrawSupervoxelBorder(img,label,0,normvalue);
    iftWriteImageByExt(border,argv[3]);
  } else {
    border   = iftBorderImage(label,0);
    A        = iftCircular(1.0);
    B        = iftCircular(atof(argv[7]));
    YCbCr    = iftRGBtoYCbCr(RGB, normvalue);  
    iftDrawBorders(img,border,A,YCbCr,B);
    iftWriteImageByExt(img,argv[3]);
    iftDestroyAdjRel(&A);
    iftDestroyAdjRel(&B);
  }
  
  iftDestroyImage(&label);
  iftDestroyImage(&img);
  iftDestroyImage(&border);

  t2     = iftToc();

  fprintf(stdout,"Computational time in %f ms\n",iftCompTime(t1,t2));


  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}



