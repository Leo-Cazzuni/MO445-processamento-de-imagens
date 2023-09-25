#include "ift.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>


#define _SILENCE

#define PGM "pgm"
#define PPM "ppm"

iftImage *iftAddFrameXY(iftImage *img, int sxL,int sxR,int syT,int syB, int value)
{
  iftImage *fimg;
  int p, q;
  iftVoxel u;


  if (iftIs3DImage(img)) {
    fimg = iftCreateImage(img->xsize+(sxL+sxR),img->ysize+(syT+syB), img->zsize);
    iftCopyVoxelSize(img,fimg);
    iftSetImage(fimg,value);

    p = 0;
    for (u.z=0; u.z < fimg->zsize; u.z++) 
      for (u.y=syT; u.y < fimg->ysize-syB; u.y++) 
	for (u.x=sxL; u.x < fimg->xsize-sxR; u.x++) {
	  q = iftGetVoxelIndex(fimg,u);
	  fimg->val[q] = img->val[p];
	  p++;
	}
  } else {
    fimg = iftCreateImage(img->xsize+(sxL+sxR),img->ysize+(syT+syB), 1);
    iftSetImage(fimg,value);

    if (iftIsColorImage(img)){
      iftSetCbCr(fimg,128); // value = 0 => 128?
      p = 0; u.z = 0;
      for (u.y=syT; u.y < fimg->ysize-syB; u.y++) 
	for (u.x=sxL; u.x < fimg->xsize-sxR; u.x++) {
	  q = iftGetVoxelIndex(fimg,u);
	  fimg->val[q] = img->val[p];
	  fimg->Cb[q]  = img->Cb[p];
	  fimg->Cr[q]  = img->Cr[p];
	  p++;
	}
    }else{
      p = 0; u.z = 0;
      for (u.y=syT; u.y < fimg->ysize-syB; u.y++) 
	for (u.x=sxL; u.x < fimg->xsize-sxR; u.x++) {
	  q = iftGetVoxelIndex(fimg,u);
	  fimg->val[q] = img->val[p];
	  p++;
	}
    }
  }

  return(fimg);
}

int iftCompareImageName(const void * a, const void * b);


int main(int argc, char **argv) {
  if (argc != 6) {
    char msg[300];
    sprintf(msg,"usage: %s <window_X> <window_Y> <image_directory> <output_directory> <PPM|PGM>\n\n",argv[0]);
    iftError(msg, "iftImageDimensions");
  }

  int WINDOW_X = atoi(argv[1]);
  int WINDOW_Y = atoi(argv[2]);

  char image_directory[100] ;strcpy(image_directory , argv[3]);
  char output_directory[100];strcpy(output_directory, argv[4]);
  char format[100]          ;strcpy(format          , argv[5]);

  if (!iftDirExists(image_directory)) {
    char msg[300];
    sprintf(msg, "Error: the directory indicated by %s does not exist", image_directory);
    iftError(msg, "iftImageDimensions");
  }
  // insert '/' in the end of the path, if it doesn't have it
  if (image_directory[strlen(image_directory)-1] != '/')
    strcat(image_directory, "/");

  if (!iftDirExists(output_directory)) {
    char msg[300];
    sprintf(msg, "Error: the directory indicated by %s does not exist", output_directory);
    iftError(msg, "iftImageDimensions");
  }
  // insert '/' in the end of the path, if it doesn't have it
  if (output_directory[strlen(output_directory)-1] != '/')
    strcat(output_directory, "/");

  if (strcmp(format,PGM)!=0 && strcmp(format,PPM)!=0) {
    char msg[300];
    sprintf(msg, "Error: the extension indicated (%s) is not valid", format);
    iftError(msg, "iftImageDimensions");
  }

  int nimages;
  iftImageNames *img_names;

  nimages = iftCountImageNames(image_directory, format);
  img_names = iftCreateAndLoadImageNames(nimages, image_directory, format);
  qsort(img_names, nimages, sizeof(iftImageNames), iftCompareImageName);

#ifndef _SILENCE
  printf("Image Directory: %s\n", image_directory);
  printf("Number of Images: %d\n", nimages);
  puts("");
#endif 

  iftImage *img,*imgtmp;
  char filename[300];

#ifndef _SILENCE
  int nclasses = iftCountNumberOfClasses(img_names,nimages);

  int maxX=0,minX=0,maxY=0,minY=0,maxV=0.;
  int *ct    = iftAllocIntArray(nclasses+1);
  int *pmaxX = iftAllocIntArray(nclasses+1);
  int *pminX = iftAllocIntArray(nclasses+1);
  int *pmaxY = iftAllocIntArray(nclasses+1);
  int *pminY = iftAllocIntArray(nclasses+1);
  int *pmaxV = iftAllocIntArray(nclasses+1);
  float maxR=0.,minR=0.;
  float* pmaxR = iftAllocFloatArray(nclasses+1);
  float* pminR = iftAllocFloatArray(nclasses+1);

  //  float meanx=0.,meany=0.,stdevx=0.,stdevy=0;
#endif

  for(int i = 0; i < nimages ; i++) {
    //    fprintf(stdout,"%s: ",img_names[i].image_name);
    fprintf(stdout,"%6d ",i+1);
    sprintf(filename, "%s%s", image_directory, img_names[i].image_name);
    if (strcmp(format,PPM)==0)
      img = iftReadImageP6(filename);
    if (strcmp(format,PGM)==0)
      img = iftReadImageP5(filename);

#ifndef _SILENCE
    int class=img_names[i].attribute;

    int  maxvalue=iftMaximumValue(img);
    float ratio = (float)img->xsize/img->ysize;

    if ( (ct[class] == 0) || (pmaxX[class] < img->xsize) )
      pmaxX[class] = img->xsize;
    if ( (ct[class] == 0) || (pmaxY[class] < img->ysize) )
      pmaxY[class] = img->ysize;
    if ( (ct[class] == 0) || (pmaxR[class] < ratio     ) )
      pmaxR[class] = ratio;
    if ( (ct[class] == 0) || (pmaxV[class] < maxvalue  ) )
      pmaxV[class] = maxvalue;


    if ( (ct[class] == 0) || (pminX[class] > img->xsize) )
      pminX[class] = img->xsize;
    if ( (ct[class] == 0) || (pminY[class] > img->ysize) )
      pminY[class] = img->ysize;
    if ( (ct[class] == 0) || (pminR[class] > ratio     ) )
      pminR[class] = ratio;

    ct[class]++;
#endif

#ifndef _SILENCE
    fprintf(stdout,"input: %dx%d\n",img->xsize,img->ysize);
#endif

    if (img->xsize > img->ysize)
      imgtmp=iftInterp2D(img,(float)WINDOW_X/img->xsize,(float)WINDOW_X/img->xsize);
    else
      imgtmp=iftInterp2D(img,(float)WINDOW_Y/img->ysize,(float)WINDOW_Y/img->ysize);
    iftDestroyImage(&img);
    img=imgtmp;

#ifndef _SILENCE
    fprintf(stdout,"rescale: %dx%d\n",img->xsize,img->ysize);
#endif

    imgtmp=iftAddFrameXY(img,(int)ceil((float)(WINDOW_X-img->xsize)/2),(int)floor((float)(WINDOW_X-img->xsize)/2)
			    ,(int)ceil((float)(WINDOW_Y-img->ysize)/2),(int)floor((float)(WINDOW_Y-img->ysize)/2),0);
    iftDestroyImage(&img);
    img=imgtmp;

#ifndef _SILENCE
    fprintf(stdout,"rescale: %dx%d\n",img->xsize,img->ysize);
#endif

    if ( (img->xsize != WINDOW_X) || (img->ysize != WINDOW_Y) ) {
      char msg[300];
      sprintf(msg,"Error: Image dimensions are wrong: %dx%d\n\n",img->xsize,img->ysize);
      iftError(msg,"iftImageDimensions");
    }

    sprintf(filename, "%s%s", output_directory, img_names[i].image_name);
    if (strcmp(format,PPM)==0)
      iftWriteImageP6(img,filename);
    if (strcmp(format,PGM)==0)
      iftWriteImageP5(img,filename);

    iftDestroyImage(&img);
#ifndef _SILENCE
    fprintf(stdout,"%dx%d\n",img->xsize,img->ysize);
#endif
  }

#ifndef _SILENCE
  for(int i=1; i <=nclasses; i++) {
    fprintf(stdout,"%d: X-[%3d,%3d], Y-[%3d,%3d], R-[%4.2f,%4.2f], V-[%4d]\n",i,pminX[i],pmaxX[i],pminY[i],pmaxY[i],pminR[i],pmaxR[i],pmaxV[i]);
    if ( (i==1) || (minX > pminX[i]) ) minX = pminX[i];
    if ( (i==1) || (minY > pminY[i]) ) minY = pminY[i];
    if ( (i==1) || (minR > pminR[i]) ) minR = pminR[i];
    if ( (i==1) || (maxX < pmaxX[i]) ) maxX = pmaxX[i];
    if ( (i==1) || (maxY < pmaxY[i]) ) maxY = pmaxY[i];
    if ( (i==1) || (maxR < pmaxR[i]) ) maxR = pmaxR[i];
    if ( (i==1) || (maxV < pmaxV[i]) ) maxV = pmaxV[i];
  }
  fprintf(stdout,"Total: X-[%3d,%3d], Y-[%3d,%3d], R-[%4.2f,%4.2f], V-[%4d]\n",minX,maxX,minY,maxY,minR,maxR,maxV);

  free(ct);free(pmaxX);free(pmaxY);free(pminX);free(pminY);free(pminR);free(pmaxR);free(pmaxV);
#endif

  return 0;
}

int iftCompareImageName(const void * a, const void * b) {
  iftImageNames *first = (iftImageNames*) a;
  iftImageNames *second = (iftImageNames*) b;
  if ( strcmp(first->image_name, second->image_name) < 0 ) return -1;
  if ( strcmp(first->image_name, second->image_name) == 0 ) return 0;

  return 1;
}
