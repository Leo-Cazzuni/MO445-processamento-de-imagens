#include "ift.h"

iftFImage *iftConnStrength(float *w, iftAdjRel *B, iftImage *Index, int xsize, int ysize, int zsize)
{
  int        i,p,q;
  iftVoxel   u, v;
  float      tmp;
  iftFHeap  *H;
  iftFImage *Weight;

  Weight = iftCreateFImage(xsize,ysize,zsize);
  H      = iftCreateFHeap(Weight->n,Weight->val);
  iftSetRemovalPolicyFHeap(H,MAXVALUE);

  for (p=0; p < Index->n; p++) {
    if (Index->val[p]>0){
      Weight->val[p] = -IFT_INFINITY_FLT; 
    }else{
      if (Index->val[p]==0){
	Weight->val[p] = w[Index->val[p]];
	iftInsertFHeap(H,p);
      }
    }
  }

  while(!iftEmptyFHeap(H)){
    p = iftRemoveFHeap(H);
    u = iftFGetVoxelCoord(Weight,p);
    for (i=1; i < B->n; i++) {
      v = iftGetAdjacentVoxel(B,u,i);
      if (iftFValidVoxel(Weight,v)){
	q = iftGetVoxelIndex(Weight,v);
	if ((Index->val[q]>0)&&(H->color[q]!=IFT_BLACK)){
	  tmp = iftMin(Weight->val[p],w[Index->val[q]]);
	  if (tmp > Weight->val[q]) {
	    Weight->val[q]=tmp;
	    if(H->color[q] == IFT_WHITE)
	      iftInsertFHeap(H, q);
	    else
	      iftGoUpFHeap(H, H->pos[q]);
	  }
	}
      }
    }
  }
  iftDestroyFHeap(&H);

  return(Weight);
}

iftImage  *iftSmoothImageByConn(iftImage *img, iftAdjRel *A, float sigma)
{
  iftImage  *fimg=iftCopyImage(img), *Index;
  float      Sigma = 2.0*sigma*sigma, *ws=iftAllocFloatArray(A->n);
  int        xsize, ysize, zsize; 
  iftAdjRel *B;
  
  iftMaxAdjShifts(A,&xsize,&ysize,&zsize);
  xsize = 2*xsize + 1;
  ysize = 2*ysize + 1;
  zsize = 2*zsize + 1;
  if (iftIs3DImage(img)){
    B = iftSpheric(1.0);    
  }else{
    B = iftCircular(1.0);
  }
  Index  = iftCreateImage(xsize,ysize,zsize);

  for (int p=0; p < Index->n; p++) 
    Index->val[p] = IFT_NIL;

  for (int i=0; i < A->n; i++) { 
    ws[i] = 1.0/(A->dx[i]*A->dx[i] + A->dy[i]*A->dy[i] + A->dz[i]*A->dz[i] + 1);
    iftVoxel u; 
    u.x = A->dx[i] + xsize/2; 
    u.y = A->dy[i] + ysize/2; 
    u.z = A->dz[i] + zsize/2; 
    int p = iftGetVoxelIndex(Index,u); 
    Index->val[p] = i; 
  }
    
  if (iftIsColorImage(img)){
#pragma omp parallel for shared(img,fimg,A,Sigma,ws,B,xsize,ysize,zsize,Index)
    for (int p=0; p < img->n; p++) {

      iftVoxel u   = iftGetVoxelCoord(img,p);
      float Yp     = img->val[p], Cbp    = img->Cb[p], Crp    = img->Cr[p];
      float    *wr = iftAllocFloatArray(A->n);

      for (int i=0; i < A->n; i++){
	iftVoxel v = iftGetAdjacentVoxel(A,u,i);
	if (iftValidVoxel(img,v)){
	  int q     = iftGetVoxelIndex(img,v);
	  float Yq  = img->val[q], Cbq = img->Cb[q], Crq = img->Cr[q];
	  wr[i] = (Yq-Yp)*(Yq-Yp) +
	    (Cbq-Cbp)*(Cbq-Cbp) + (Crq-Crp)*(Crq-Crp);
	  wr[i] = ws[i]*expf(-wr[i]/Sigma);
	}
      }

      iftFImage *Weight=iftConnStrength(wr,B,Index,xsize,ysize,zsize);

      float valY   = 0.0, valCb = 0.0, valCr = 0.0, sum = 0.0;
      for (int i=0; i < Index->n; i++){
	if (Index->val[i]>=0){
	  iftVoxel v;
	  v.x = u.x + A->dx[Index->val[i]];
	  v.y = u.y + A->dy[Index->val[i]];
	  v.z = u.z + A->dz[Index->val[i]];
	  if (iftValidVoxel(img,v)){
	    int q = iftGetVoxelIndex(img,v);
	    sum         += Weight->val[i];
	    valY        += ((float) img->val[q] * Weight->val[i]);
	    valCb       += ((float) img->Cb[q]  * Weight->val[i]);
	    valCr       += ((float) img->Cr[q]  * Weight->val[i]);
	  }
	}
      }

      fimg->val[p] = iftRound(valY/sum);
      fimg->Cb[p]  = iftRound(valCb/sum);
      fimg->Cr[p]  = iftRound(valCr/sum);
      free(wr);
      iftDestroyFImage(&Weight);
    }
  } else{ 
#pragma omp parallel for shared(img,fimg,A,Sigma,ws,B,xsize,ysize,zsize,Index)
    for (int p=0; p < img->n; p++) {
      iftVoxel u   = iftGetVoxelCoord(img,p);
      float Yp     = img->val[p];
      float valY   = 0.0, sum = 0.0;
      float    *wr = iftAllocFloatArray(A->n);
      for (int i=0; i < A->n; i++){
	iftVoxel v = iftGetAdjacentVoxel(A,u,i);
	if (iftValidVoxel(img,v)){
	  int q     = iftGetVoxelIndex(img,v);
	  float Yq  = img->val[q];
	  wr[i] = (Yq-Yp)*(Yq-Yp);
	  wr[i] = ws[i]*expf(-wr[i]/Sigma);
	}
      }
      iftFImage *Weight=iftConnStrength(wr,B,Index,xsize,ysize,zsize);
      for (int i=0; i < Index->n; i++){
	if (Index->val[i]>=0){
	  iftVoxel v;
	  v.x = u.x + A->dx[Index->val[i]];
	  v.y = u.y + A->dy[Index->val[i]];
	  v.z = u.z + A->dz[Index->val[i]];
	  if (iftValidVoxel(img,v)){
	    int q = iftGetVoxelIndex(img,v);
	    sum         += Weight->val[i];
	    valY        += ((float) img->val[q] * Weight->val[i]);
	  }
	}
      }      
      fimg->val[p] = iftRound(valY/sum);
      free(wr);
      iftDestroyFImage(&Weight);
    }
  }

  free(ws);
  iftCopyVoxelSize(img,fimg);
  iftDestroyImage(&Index);
  iftDestroyAdjRel(&B);

  return(fimg);
}


int main(int argc, char *argv[]) 
{
  iftImage  *img[2];
  iftAdjRel *A;
  char       ext[10],*pos;
  timer     *t1=NULL,*t2=NULL;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc!=6){
    iftError("Usage: iftSmoothImage <input.[ppm,pgm,scn]> <adj_radius> <sigma> <output.[ppm,pgm,scn]> <By connectivity (0/1)>","main");
  }

  pos = strrchr(argv[1],'.') + 1;
  sscanf(pos,"%s",ext);
  
  if (strcmp(ext,"ppm")==0){
    img[0]   = iftReadImageP6(argv[1]);    
    A        = iftCircular(atof(argv[2]));
  }else{
    if (strcmp(ext,"pgm")==0){
      img[0]   = iftReadImageP5(argv[1]);    
      A        = iftCircular(atof(argv[2]));
    }else{
      if (strcmp(ext,"scn")==0){
	img[0]   = iftReadImage(argv[1]);   
	A        = iftSpheric(atof(argv[2]));
      }else{
	fprintf(stderr,"Invalid image format: %s\n",ext);
	exit(-1);
      }
    }
  }

  t1     = iftTic();

  if (atoi(argv[5]))
    img[1] = iftSmoothImageByConn(img[0],A,atof(argv[3]));
  else
    img[1] = iftSmoothImage(img[0],A,atof(argv[3]));

  t2     = iftToc();
  fprintf(stdout,"Smoothing in %f ms\n",iftCompTime(t1,t2));

  iftDestroyImage(&img[0]);
  iftDestroyAdjRel(&A);
    
  pos = strrchr(argv[1],'.') + 1;
  sscanf(pos,"%s",ext);
  
  if (strcmp(ext,"ppm")==0){
    iftWriteImageP6(img[1],argv[4]);
  }else{
    if (strcmp(ext,"pgm")==0){
      iftWriteImageP5(img[1],argv[4]);
    }else{
      if (strcmp(ext,"scn")==0){
	iftWriteImage(img[1],argv[4]);
      }else{
	fprintf(stderr,"Invalid image format: %s\n",ext);
	exit(-1);
      }
    }
  }
  
  iftDestroyImage(&img[1]);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);

}
