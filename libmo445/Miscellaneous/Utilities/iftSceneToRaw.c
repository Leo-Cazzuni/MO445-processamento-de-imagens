#include "ift.h"

int main(int argc, char *argv[])
{
    iftImage  *img;
    timer     *t1=NULL,*t2=NULL;

    /*--------------------------------------------------------*/
	size_t MemDinInicial, MemDinFinal;
    MemDinInicial = iftMemoryUsed();

    /*--------------------------------------------------------*/

    if (argc!=3)
        iftError("Usage: %s <image original 2D/3D image name> <output raw image name>", "main", argv[0]);

    img = iftReadImageByExt(argv[1]);

    t1     = iftTic();

    iftWriteRawSceneWithInfoOnFilename(img, argv[2]);

    t2     = iftToc();
    fprintf(stdout,"Conversion in %f ms\n",iftCompTime(t1,t2));


    iftDestroyImage(&img);

    /* ---------------------------------------------------------- */

    MemDinFinal = iftMemoryUsed();
    if (MemDinInicial!=MemDinFinal)
        printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
               MemDinInicial,MemDinFinal);

    return(0);

}
