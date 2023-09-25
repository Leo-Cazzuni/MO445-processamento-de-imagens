#include "ift.h"

/* Reads a file with a list of displacements from p to its adjacents
   q, creating the corresponding adjacency relation */

iftAdjRel *iftAdjacencyFromFile(char *displ_file)
{
  iftAdjRel *A = (iftAdjRel *)calloc(1,sizeof(iftAdjRel));
  FILE *fp     = fopen(displ_file,"r");
  
  fscanf(fp,"%d",&A->n);
  A->dx = iftAllocIntArray(A->n);
  A->dy = iftAllocIntArray(A->n);
  A->dz = iftAllocIntArray(A->n);
  A->dt = iftAllocIntArray(A->n); /* there is no time in our adjacency
				     file */
  
  for (int i=0; i < A->n; i++){
    fscanf(fp,"%d %d %d",&A->dx[i],&A->dy[i],&A->dz[i]);
  }
  fclose(fp);

  return(A);

}

/* The next functions compute morphological dilation, erosion, closing
   and opening */


iftImage *Dilation(iftImage *img, iftAdjRel *A)
{
  iftImage *res = iftCopyImage(img);

  for (int p=0; p < img->n; p++) {
    iftVoxel u = iftGetVoxelCoord(img,p);
    for (int i=0; i < A->n; i++){
      iftVoxel v = iftGetAdjacentVoxel(A,u,i);

      if (iftValidVoxel(img,v)){
	int q = iftGetVoxelIndex(img,v);	
	if (img->val[q] > res->val[p])
	  res->val[p] = img->val[q];
      }
    }
  }

  return(res);
}

iftImage *Erosion(iftImage *img, iftAdjRel *A)
{
  iftImage *res = iftCopyImage(img);

  for (int p=0; p < img->n; p++) {
    iftVoxel u = iftGetVoxelCoord(img,p);
    for (int i=0; i < A->n; i++){
      iftVoxel v = iftGetAdjacentVoxel(A,u,i);
      if (iftValidVoxel(img,v)){
	int q = iftGetVoxelIndex(img,v);
	if (img->val[q] < res->val[p])
	  res->val[p] = img->val[q];
      }
    }
  }
  
  return(res);
}

iftImage *Closing(iftImage *img, iftAdjRel *A)
{
  iftImage *dil = Dilation(img,A);
  iftImage *res = Erosion(dil,A);
  iftDestroyImage(&dil);
  return(res);
}

iftImage *Opening(iftImage *img, iftAdjRel *A)
{
  iftImage *ero = Erosion(img,A);
  iftImage *res = Dilation(ero,A);
  iftDestroyImage(&ero);
  return(res);
}

iftImage *MorphGradient(iftImage *img, iftAdjRel *A)
{
  iftImage *dil = Dilation(img,A);
  iftImage *ero = Erosion(img,A);
  iftImage *grad = iftSub(dil,ero);

  iftDestroyImage(&dil);
  iftDestroyImage(&ero);

  return(grad);
}


/* Your first task: Add a new option in the code to compute the
   morphological gradient G of the input image I as follows. For
   grayscale images, G(I,A) can be defined by the dilation D(I,A) of I
   using adjacency A subtracted by the erosion E(I,A) of I using
   adjacency A. That is, G(I,A) = D(I,A) - E(I,A). You may use
   iftSub(D,E) to compute the subtraction D - E. For color images,
   select the maximum gradient value among channels for each pixel p
   to form the resulting gradient image. Note that, iftCreateImage
   creates an iftImage with zeros, but it does assume the image is
   grayscale. Then, Cb and Cr are NULL. To set them, you may use
   iftSetCbCr(128) if the image is an 8-bit image. You may also copy a
   color/grayscale image with iftCopyImage, and then change its values
   during the operation.
*/

int main(int argc, char *argv[]) {
    timer *tstart;

    if (argc!=5)
      iftError("Usage: adjacency <...>\n"
	       "[1] input image (.png, .pgm) \n"
	       "[2] input displacement file to define adjacency (.txt)\n"
	       "[3] operation -- 0: dilation, 1: erosion, 2: closing, 3:opening, 4: gradient\n"
	       "[4] output image (.png, .pgm)\n",
	       "main");

    tstart = iftTic();
    
    iftImage  *img   = iftReadImageByExt(argv[1]);
    iftAdjRel *A     = iftAdjacencyFromFile(argv[2]);
    int        oper  = atoi(argv[3]);
    iftImage  *res   = NULL;

    
    switch(oper) {

    case 0: /* Dilation */
      res = Dilation(img,A);
      break;
    case 1: /* Erosion */
      res = Erosion(img,A);
      break;
    case 2: /* Closing */
      res = Closing(img,A);
      break;
    case 3: /* Opening */
      res = Opening(img,A);
      break;
    case 4: /* Morphological Gradient */
      res = MorphGradient(img,A);
      break;
    default:
      iftError("Invalid operation number %d","main",oper);
    }

    iftWriteImageByExt(res,argv[4]);
    iftDestroyImage(&img);
    iftDestroyImage(&res);
    iftDestroyAdjRel(&A);
    
    printf("Done ... %s\n", iftFormattedTime(iftCompTime(tstart, iftToc())));
    return (0);
}
