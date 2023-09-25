//
// Created by azael on 29/11/17.
//

#include "ift.h"

int main(int argc, const char *argv[])
{
    iftImage       *img[3];
    timer          *t1=NULL,*t2=NULL;

    if (argc!=4)
        iftError("Usage: iftSub <image1.[pgm,scn,ppm]> <image2.[pgm,scn,ppm]> <out.[pgm,scn,ppm]>","main");

    /* if (!(iftAreFileExtensionsEqual(argv[1],argv[2]))) */
    /*     iftError("Files extensions differ.","iftSub"); */

    img[0]   = iftReadImageByExt(argv[1]);
    img[1]   = iftReadImageByExt(argv[2]);

    puts("- Performing image1 - image2");
    t1     = iftTic();

    img[2] = iftSub(img[0],img[1]);
    iftDestroyImage(&img[0]);
    iftDestroyImage(&img[1]);

    t2     = iftToc();
    fprintf(stdout,"- Subtraction executed in %f ms\n",iftCompTime(t1,t2));

    iftWriteImageByExt(img[2], argv[3]);
    iftDestroyImage(&img[2]);

    return(0);
}
