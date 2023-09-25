#include "ift.h"

iftMatrix *iftLaplacianMatrix(iftImage *img, iftAdjRel *A)
{
  iftMatrix *L = iftCreateMatrix(img->n, img->n);
  
  for (int p=0; p < img->n; p++){
    iftVoxel u = iftGetVoxelCoord(img,p);
    float degree = 0.0;
    for (int i=1; i < A->n; i++){
      iftVoxel v = iftGetAdjacentVoxel(A,u,i);
      if (iftValidVoxel(img,v)){
	int q                  = iftGetVoxelIndex(img,v); 
	iftMatrixElem(L, p, q) = 1;
	degree    += iftMatrixElem(L, p, q); 
      }
    }
    for (int i=1; i < A->n; i++){
      iftVoxel v = iftGetAdjacentVoxel(A,u,i);
      if (iftValidVoxel(img,v)){
	int q      = iftGetVoxelIndex(img,v); 
	iftMatrixElem(L, p, q) = degree - iftMatrixElem(L, p, q);
      }
    }
  }

  return(L);
}

iftFImage *iftImageGraphFourierTrans(iftImage *img, iftMatrix *M)
{
  iftFImage *gft = iftCreateFImage(img->xsize,img->ysize,img->zsize);

  for (int q=0; q < img->n; q++) {
    for (int p=0; p < img->n; p++) 
      gft->val[q] += img->val[p]*iftMatrixElem(M, p, q);
  }
  return(gft);
}

iftImage *iftInverseImageGraphFourierTrans(iftFImage *gft, iftMatrix *M)
{
  iftImage *img = iftCreateImage(gft->xsize,gft->ysize,gft->zsize);
  float val;
  
  for (int q=0; q < gft->n; q++) {
    val = 0;
    for (int p=0; p < gft->n; p++) 
      val += gft->val[p]*iftMatrixElem(M, p, q);
    img->val[q] = iftRound(val);
  }
  return(img);
}


int main(int argc, char *argv[]) 
{
  iftMatrix  *U,*S,*Vt,*L;
  iftAdjRel  *A;
  iftImage   *img;
  iftFImage  *gft;
  int         MemDinInicial, MemDinFinal;
    
  MemDinInicial = iftMemoryUsed(1);

  if (argc!=4)
    iftError("Usage: iftLaplacian <input-image.*> <adjacency radius> <output-image.*>","main");


  img  = iftReadImageByExt(argv[1]);
  if (iftIs3DImage(img))
    A    = iftSpheric(atof(argv[2]));
  else
    A    = iftCircular(atof(argv[2]));  

  L = iftLaplacianMatrix(img,A);

  
  timer *t1=iftTic();

  iftSingleValueDecomp(L,&U,&S,&Vt);
  gft  = iftImageGraphFourierTrans(img,Vt);
  /* /\* high-pass filter *\/ */
  /* for (int p=0; p < gft->n/2; p++)     */
  /*   gft->val[p]=gft->val[p]*(float)p/(gft->n/2.0);   */
  iftDestroyImage(&img);
  img = iftInverseImageGraphFourierTrans(gft,iftTransposeMatrix(Vt));
  iftWriteImageByExt(img,argv[3]);

  
  timer *t2=iftToc();

  printf("%f ms\n",iftCompTime(t1,t2));

  
  iftDestroyMatrix(&L);
  iftDestroyMatrix(&U);
  iftDestroyMatrix(&S);
  iftDestroyMatrix(&Vt);
  iftDestroyAdjRel(&A);
  iftDestroyImage(&img);
  iftDestroyFImage(&gft);
  
  MemDinFinal = iftMemoryUsed();
  iftVerifyMemory(MemDinInicial, MemDinFinal);

  return(0);
}

