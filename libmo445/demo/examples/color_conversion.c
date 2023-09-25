#include "ift.h"

char *Basename(char *path)
{
  char *basename     = iftBasename(path);
  iftSList *slist    = iftSplitString(basename,"/");
  strcpy(basename,slist->tail->elem);
  iftDestroySList(&slist);
  return(basename);
}

int main(int argc, char *argv[]) {
    timer *tstart;

    if (argc!=4)
      iftError("Usage: color_conversion <...>\n"
	       "[1] input image .png \n"
	       "[2] color space (0: Y,Cb,Cr; 1: L,a,b; 2: R,G,B)\n"
	       "[3] output directory\n",
	       "main");

    tstart = iftTic();

    /* All color images are read and stored in Y,Cb,Cr, being
       Cb=Cr=NULL when it is a gray-scale image. */
    
    iftImage *img   = iftReadImageByExt(argv[1]);

    if (!iftIsColorImage(img))
      iftError("Input image %s is not a color image","color_conversion.c",argv[1]);

    int color_space = atoi(argv[2]);    
    iftMImage *mimg  = NULL; /* a multiband image */
      
    switch(color_space) {
    case 0: /* YCbCr: simply copy YCbCr pixel values to a multiband
	       image with three channels. */
      mimg = iftCreateMImage(img->xsize,img->ysize,img->zsize,3);
      for (int p=0; p < img->n; p++){
	mimg->val[p][0] = img->val[p];
	mimg->val[p][1] = img->Cb[p];
	mimg->val[p][2] = img->Cr[p];
      }
      break;
    case 1: /* Lab: Use function that converts a multiband image from
	       YCbCr to Lab */
      mimg = iftImageToMImage(img, LAB_CSPACE); 
      break;
    case 2: /* RGB: Use function that converts a multiband image from
	       YCbCr to RGB */
      mimg = iftImageToMImage(img, RGB_CSPACE); 
      break;
    default:
      iftError("Usage: color_conversion <...>\n"
	       "[1] input image .png \n"
	       "[2] color space (0: YCbCr; 1: Lab; 2: RGB)\n"
	       "[3] output directory\n",
	       "main");
    }
          
    iftImage *output[3];    
    int       Imax     = iftNormalizationValue(iftMaximumValue(img));
    char     *basename = Basename(argv[1]);
    char      filename[200];

    iftMakeDir(argv[3]);
    
    for (int b=0; b < 3; b++){ 
      output[b] = iftMImageToImage(mimg,Imax,b);
      sprintf(filename,"%s/%s_band%d.png",argv[3],basename,b);
      iftWriteImageByExt(output[b],filename);
      iftDestroyImage(&output[b]);
    }

    iftDestroyImage(&img);
    iftDestroyMImage(&mimg);
    iftFree(basename);
    
    printf("Done ... %s\n", iftFormattedTime(iftCompTime(tstart, iftToc())));
    return (0);
}
