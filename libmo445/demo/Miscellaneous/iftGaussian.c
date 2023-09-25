#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftImage *img[5];
  iftKnnGraph *graph;
  iftDataSet  *Z;
  int          p;
  iftVoxel     mean;
  iftColor     RGB,YCbCr;
  iftAdjRel   *A=NULL,*B=NULL;
  timer       *t1=NULL,*t2=NULL;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc!=1)
    iftError("Usage: iftGaussian","main");


  
  t1     = iftTic();
  mean.x = 50;
  mean.y = 50;
  mean.z = 0;
  img[0]    = iftCreateGaussian(200,200,1,mean,400.0,100);
  mean.x = 150;
  mean.y = 150;
  mean.z = 0;
  img[1]    = iftCreateGaussian(200,200,1,mean,1600.0,100);
  mean.x = 50;
  mean.y = 150;
  mean.z = 0;
  img[2]    = iftCreateGaussian(200,200,1,mean,20.0,100);
  mean.x = 150;
  mean.y = 50;
  mean.z = 0;
  img[3]    = iftCreateGaussian(200,200,1,mean,30.0,100);
  img[4] = iftAdd(img[0],img[1]);
  iftDestroyImage(&img[0]);
  iftDestroyImage(&img[1]);
  img[0] = iftAdd(img[2],img[3]);
  img[1] = iftAdd(img[0],img[4]);

  Z        = iftCreateDataSet(img[1]->n,4);

  iftMaximumValue(img[1]);
  for (p=0; p < Z->nsamples; p++) {
    mean.x = 50; mean.y = 50; mean.z = 0;
    Z->sample[p].feat[0] = iftVoxelDistance(iftGetVoxelCoord(img[1],p),mean);
    mean.x = 150; mean.y = 150; mean.z = 0;
    Z->sample[p].feat[1] = iftVoxelDistance(iftGetVoxelCoord(img[1],p),mean);
    mean.x = 50; mean.y = 150; mean.z = 0;
    Z->sample[p].feat[2] = iftVoxelDistance(iftGetVoxelCoord(img[1],p),mean);
    mean.x = 150; mean.y = 50; mean.z = 0;
    Z->sample[p].feat[3] = iftVoxelDistance(iftGetVoxelCoord(img[1],p),mean);
    Z->sample[p].id          = p;
    Z->sample[p].weight  = MAXWEIGHT*((float)img[1]->val[p]/(float)img[1]->maxval);
  }

  Z->ref_data = img[1];

  iftSelectUnsupTrainSamplesByWeight(Z,0.01);
  graph = iftCreateKnnGraph(Z,(int)(0.1*Z->ntrainsamples));
  iftUnsupTrain(graph,iftNormalizedCut);

  /*  iftAdjRel *C=iftCircular(1.5);
  iftImage *marker = iftAddValue(img[1],-1);
  iftImage *label  = iftDualWaterGray(img[1],marker,C);
  iftWriteImageP2(label,"label.pgm");
  exit(0);
  */

  //  graph = iftUnsupLearn(Z,0.1,0.1,iftNormalizedCut);
  iftUnsupClassify(graph,Z);

  t2     = iftToc();
  fprintf(stdout,"gaussian functions in %f ms\n",iftCompTime(t1,t2));

  iftMaximumValue(img[1]);
  for (p=0; p < Z->nsamples; p++) {
    if (Z->sample[p].status == IFT_TRAIN)
      img[1]->val[p] = img[1]->maxval;
  }
  iftWriteImageP2(img[1],"train.pgm");

  iftDestroyImage(&img[0]);
  img[0] = iftDataSetLabel(Z,"iftImage");
  iftWriteImageP2(img[0],"labels.pgm");

  RGB.val[0] = 255;
  RGB.val[1] = 0;
  RGB.val[2] = 0;
  YCbCr      = iftRGBtoYCbCr(RGB,255);
  A          = iftCircular(sqrtf(2.0));
  B          = iftCircular(0.0);
  iftImage *tmp = iftNormalize(img[1],0,255);
  iftDrawBorders(tmp,img[0],A,YCbCr,B);
  iftWriteImageP6(tmp,"result.ppm");
  iftDestroyImage(&tmp);
  iftDestroyAdjRel(&A);
  iftDestroyAdjRel(&B);
  iftDestroyImage(&img[0]);
  iftDestroyImage(&img[1]);
  iftDestroyImage(&img[2]);
  iftDestroyImage(&img[3]);
  iftDestroyImage(&img[4]);

  iftDestroyDataSet(&Z);
  iftDestroyKnnGraph(&graph);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);

}
