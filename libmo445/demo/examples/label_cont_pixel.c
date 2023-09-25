#include "ift.h"


int main(int argc, const char *argv[]) {

  if (argc != 3){
    iftError("iftLabelContPixel binary.* result.*","main");
  }

  iftImage *img      = iftReadImageByExt(argv[1]);
  iftImage *bin      = iftBinarize(img);
  iftImage *cnt      = iftLabelContPixel(bin);


  /* if (!iftIs3DImage(cnt) && cnt->xsize <= 10 && cnt->ysize <= 10){ */
  /*   for (int p=0; p < cnt->n; p++){ */
  /*     if (cnt->val[p]!=0) */
  /* 	cnt->val[p] = 9; */
  /*     else */
  /* 	cnt->val[p] = bin->val[p];	 */
  /*   } */
  /* } */
  
  iftWriteImageP2(cnt,argv[2]);
    
  iftDestroyImage(&img);
  iftDestroyImage(&bin);
  iftDestroyImage(&cnt);
		    
  return 0;
}

