#include "ift.h"

int main(int argc, char *argv[])
{

  if (argc != 2)
    iftError("Usage: iftCorrectBinaryGTImage <gt_image>","main");

  iftImage *gt_img=iftReadImageByExt(argv[1]);

  iftImage *aux=iftRelabelGrayScaleImage(gt_img,0);

  /* we only allow two values for the label image, 0 for bg and 1 for object*/
  if (iftMaximumValue(aux) > 1){
    for (int s=0;s<aux->n;s++)
      if (aux->val[s] > 1)
        aux->val[s]=1;
  }

  if (!iftIs3DImage(gt_img))
    iftWriteImageP2(aux,"new_label.pgm");
  else
    iftWriteImage(aux,"new_label.scn");

  iftDestroyImage(&gt_img);
  iftDestroyImage(&aux);

  return(0);
}




