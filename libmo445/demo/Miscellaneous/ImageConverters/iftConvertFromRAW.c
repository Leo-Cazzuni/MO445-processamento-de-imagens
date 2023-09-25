#include "ift.h"

/* This program should convert from raw image format to any other
   format supported by the libift. I am implementing only a few
   examples now.

   author: A.X. Falcão
   date:   May 28th 2018
*/

int main(int argc, char *argv[]) 
{
  iftImage *img;
  
  if (argc!=8)
    iftError("Usage: iftConvertFromRAW <input.raw> <output.*> <xsize> <ysize> <zsize> <data_type (uchar, int, float, etc.)> <number_of_bands>","main");

  int   xsize   = atoi(argv[3]), ysize = atoi(argv[4]), zsize = atoi(argv[5]), nbands = atoi(argv[7]);
  FILE *fp      = fopen(argv[1],"r");

  img  = iftCreateImage(xsize,ysize,zsize);

  if ((strcmp(argv[6],"uchar")==0)&&(nbands==1)) {
    unsigned char  *data = iftAllocUCharArray(img->n);
    fread(data,sizeof(unsigned char),img->n,fp);
    for (int p=0; p < img->n; p++)
      img->val[p] = (int) data[p];
    iftFree(data);
  } else {
    if ((strcmp(argv[6],"uchar")==0)&&(nbands==3)) {
      iftSetCbCr(img,128);
      unsigned char  *data = iftAllocUCharArray(nbands*img->n);
      fread(data,sizeof(unsigned char),nbands*img->n,fp);
      for (int p=0; p < img->n*nbands; p = p + nbands){
	iftColor RGB, YCbCr;
	RGB.val[0] = data[p]; RGB.val[1] = data[p+1]; RGB.val[2] = data[p+2];
	YCbCr = iftRGBtoYCbCr(RGB,255);
	img->val[p] = YCbCr.val[0];
	img->Cb[p]  = YCbCr.val[1];
	img->Cr[p]  = YCbCr.val[2];
      }
      iftFree(data);
    } else {
      if ((strcmp(argv[6],"ushort")==0)&&(nbands==1)) {
	unsigned short *data = iftAllocUShortArray(img->n);
	fread(data,sizeof(unsigned short),img->n,fp);
	for (int p=0; p < img->n; p++){
	  img->val[p] = (int) data[p];
	}
	iftFree(data);
      } else {
	if ((strcmp(argv[6],"float")==0)&&(nbands==1)) {
	  iftFImage *fimg = iftCreateFImage(xsize,ysize,zsize);
	  float     *data = iftAllocFloatArray(fimg->n);
	  fread(data,sizeof(float),fimg->n,fp);
	  for (int p=0; p < fimg->n; p++)
	    fimg->val[p] = data[p];
	  img = iftFImageToImage(fimg,65535);
	  iftDestroyFImage(&fimg);
	  iftFree(data);
	}
      }
    }
  }
      
  iftWriteImageByExt(img,argv[2]);
  iftDestroyImage(&img);
  fclose(fp);

  return(0);
}
