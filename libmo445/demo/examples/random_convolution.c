#include "ift.h"

typedef struct _kernelbank {
  iftAdjRel  *A;   /* adjacency relation */
  float    ***w;   /* for each kernel, the weight vector of each adjacent pixel, with m bands */
  int m;           /* number of bands */
  int nkernels;    /* number of kernels */
} KernelBank;  

KernelBank *RandomKernelBank(iftAdjRel *A, int nbands, int nkernels)
{
  KernelBank *K = (KernelBank *) calloc(1,sizeof(KernelBank ));

  K->A        = iftCopyAdjacency(A);
  K->m        = nbands;
  K->nkernels = nkernels;
  
  K->w      = (float ***) calloc(nkernels, sizeof(float **));
  for (int j=0; j < nkernels; j++)
    K->w[j] = (float **) calloc(A->n, sizeof(float *));
  
  for (int j=0; j < nkernels; j++)
    for (int i=0; i < A->n; i++)
      K->w[j][i] = (float *) calloc(nbands, sizeof(float ));

  /* Create random weights with mean weight equal to 0 */
  
  for (int j=0; j < nkernels; j++){
    float mean=0.0;
    for (int i=0; i < A->n; i++){
      for (int b=0; b < nbands; b++){
	K->w[j][i][b] = (float)iftRandomInteger(0,100)/100.0;
	mean += K->w[j][i][b];
      }
    }
    mean /= A->n*nbands;
    for (int i=0; i < A->n; i++){
      for (int b=0; b < nbands; b++){
	K->w[j][i][b] = K->w[j][i][b] - mean;
      }
    }    
  }
  
  /* Force the norm of the weight vector of each kernel to be one */

  for (int j=0; j < nkernels; j++){
    float wnorm=0.0;
    for (int i=0; i < A->n; i++){
      for (int b=0; b < nbands; b++){
  	wnorm += K->w[j][i][b]*K->w[j][i][b];
      }
    }
    wnorm = sqrtf(wnorm/A->n*nbands);
    for (int i=0; i < A->n; i++){
      for (int b=0; b < nbands; b++){
  	K->w[j][i][b] /= wnorm;
      }
    }
  }
  
  return(K);
}

void DestroyKernelBank(KernelBank **K)
{
  KernelBank *aux = *K;

  for (int j=0; j < aux->nkernels; j++)
    for (int i=0; i < aux->A->n; i++)
      iftFree(aux->w[j][i]);

  for (int j=0; j < aux->nkernels; j++)
    iftFree(aux->w[j]);

  iftFree(aux->w);  
  iftDestroyAdjRel(&aux->A);
  iftFree(aux);
  
  (*K) = NULL;
}

float InnerProductAtPixel(iftMImage *mimg, int p, float *w)
{
  float prod = 0.0;
  for (int b=0; b < mimg->m; b++)
    prod += mimg->val[p][b]*w[b];

  return(prod);
}

iftMImage *Convolution(iftMImage *mimg, KernelBank *K)
{
  iftMImage *conv = iftCreateMImage(mimg->xsize,mimg->ysize,mimg->zsize,K->nkernels); 
  
  for (int j=0; j < K->nkernels; j++) {
    /* convolution with kernel j */
    for (int p=0; p < mimg->n; p++) {
      conv->val[p][j] = InnerProductAtPixel(mimg,p,K->w[j][0]);
      iftVoxel u = iftMGetVoxelCoord(mimg,p);
      for (int i =1; i < K->A->n; i++) {
	iftVoxel v = iftGetAdjacentVoxel(K->A, u, i);
	if (iftMValidVoxel(mimg,v)){
	  int q = iftMGetVoxelIndex(mimg,v);
	  conv->val[p][j] += InnerProductAtPixel(mimg,q,K->w[j][i]);
	}else{
	  conv->val[p][j]=0;
	  break;
	}
      }
    }
  }

  return(conv);
}

iftMImage *ConvolutionWithMatrices(iftMImage *mimg, KernelBank *K)
{
  iftMatrix *kbank = iftCreateMatrix(K->nkernels,K->m*K->A->n);

  for (int j=0; j < K->nkernels; j++){
    int row=0;
    for (int i=0; i < K->A->n; i++){
      for (int b=0; b < K->m; b++){
	iftMatrixElem(kbank, j, row) = K->w[j][i][b];
	row++;
      }
    }
  }

  iftMatrix *XI = iftMImageToFeatureMatrix(mimg,K->A,NULL);
  iftMatrix *XJ = iftMultMatrices(XI, kbank);
  iftDestroyMatrix(&XI);
	
  iftMImage *conv = iftMatrixToMImage(XJ, mimg->xsize, mimg->ysize, mimg->zsize, K->nkernels, 'c');
  for (int p=0; p < conv->n; p++) { /* set values near the border to zero */
    iftVoxel u = iftMGetVoxelCoord(conv,p);
    for (int i =1; i < K->A->n; i++) {
      iftVoxel v = iftGetAdjacentVoxel(K->A, u, i);
      if (!iftMValidVoxel(conv,v)){
	for (int b=0; b < conv->m; b++)
	  conv->val[p][b]=0;
      }
    }
  }

  return(conv);
}

iftMImage *ReLU(iftMImage *mimg)
{
  iftMImage *relu = iftCreateMImage(mimg->xsize,mimg->ysize,mimg->zsize,mimg->m);

  for (int p=0; p < mimg->n; p++)
    for (int b=0; b < mimg->m; b++)
      if (mimg->val[p][b]>0)
	relu->val[p][b]=mimg->val[p][b];

  return(relu);
}

char *Basename(char *path)
{
  char *basename     = iftBasename(path);
  iftSList *slist    = iftSplitString(basename,"/");
  strcpy(basename,slist->tail->elem);
  iftDestroySList(&slist);
  return(basename);
}

iftMImage *MaxPooling(iftMImage *mimg, iftAdjRel *A)
{
  iftMImage *pool = iftCreateMImage(mimg->xsize,mimg->ysize,mimg->zsize,mimg->m); 
  
  for (int p=0; p < mimg->n; p++) {
    iftVoxel u = iftMGetVoxelCoord(mimg,p);
    for (int b=0; b < mimg->m; b++) {
      pool->val[p][b] = mimg->val[p][b];
      for (int i =1; i < A->n; i++) {
	iftVoxel v = iftGetAdjacentVoxel(A, u, i);
	if (iftMValidVoxel(mimg,v)){
	  int q = iftMGetVoxelIndex(mimg,v);
	  pool->val[p][b] = iftMax(pool->val[p][b],mimg->val[q][b]);
	}else{
	  pool->val[p][b]=0;
	  break;
	}
      }
    }
  }

  return(pool);
}

iftColor GrayScaleToBlueToRedColor(float intensity, float norm_value)
{
    float value = 4*(intensity/norm_value)+1;

    iftColor rgb_color;
    rgb_color.val[0] = norm_value * iftMax(0,(3-(float)fabs(value-4)-(float)fabs(value-5))/2);
    rgb_color.val[1] = norm_value * iftMax(0,(4-(float)fabs(value-2)-(float)fabs(value-4))/2);
    rgb_color.val[2] = norm_value * iftMax(0,(3-(float)fabs(value-1)-(float)fabs(value-2))/2);

    iftColor ycbcr = iftRGBtoYCbCr(rgb_color, norm_value);

    return ycbcr;
}

iftImage *GrayImageToColorImage(iftImage *img){
  iftImage *colored_image;
  int Imax = iftNormalizationValue(iftMaximumValue(img));

  colored_image = iftCreateColorImage(img->xsize, img->ysize, img->zsize,Imax);
				      
  for(int p = 0; p < img->n; p++){
    iftColor ycbcr_color = GrayScaleToBlueToRedColor((float)img->val[p],Imax);

    colored_image->val[p] = ycbcr_color.val[0];
    colored_image->Cb[p]  = ycbcr_color.val[1];
    colored_image->Cr[p]  = ycbcr_color.val[2];
  }
  
  return(colored_image);
}

int main(int argc, char *argv[]) {
    timer *tstart;

    if (argc!=5)
      iftError("Usage: random_convolution <...>\n"
	       "[1] input image .png \n"
	       "[2] number of random kernels \n"
	       "[3] adjacency radius\n"
	       "[4] output directory\n",
	       "main");

    tstart = iftTic();

    /* Create a multiband image for the input image. If it is a
       gray-scale image, use a color table to convert it into a color
       image. */
    
    iftImage  *img      = iftReadImageByExt(argv[1]);
    iftMImage *mimg     = NULL;
    char      *basename = Basename(argv[1]);
    char       filename[200];
    iftMakeDir(argv[4]);
    iftRandomSeed(IFT_RANDOM_SEED);
    
    if (iftIsColorImage(img))
      mimg = iftImageToMImage(img, LABNorm2_CSPACE); 
    else{
      iftImage *cimg = GrayImageToColorImage(img);
      sprintf(filename,"%s/%s_color.png",argv[4],basename);
      iftWriteImageByExt(cimg,filename);
      mimg           = iftImageToMImage(cimg, LABNorm2_CSPACE);
      iftDestroyImage(&cimg);
    }
    //      mimg = iftImageToMImage(img, GRAYNorm_CSPACE);

    /* Create a random kernel bank and convolve image with it */

    iftAdjRel  *A     = iftCircular(atof(argv[3]));
    KernelBank *K     = RandomKernelBank(A,mimg->m,atoi(argv[2]));
    iftMImage  *conv  = ConvolutionWithMatrices(mimg,K);//Convolution(mimg,K);//ConvolutionWithMatrices(mimg,K);//Convolution(mimg,K);
    iftDestroyAdjRel(&A);
    DestroyKernelBank(&K);

    /* Save the resulting bands in the output directory, including the output mimg */
    
    iftImage *output;
    int       Imax     = iftNormalizationValue(iftMaximumValue(img));

    sprintf(filename,"%s/%s.mimg",argv[4],basename);
    iftWriteMImage(conv,filename);
    
    for (int b=0; b < conv->m; b++){ 
      output = iftMImageToImage(conv,Imax,b);
      sprintf(filename,"%s/%s_band%d.png",argv[4],basename,b);
      iftWriteImageByExt(output,filename);
      iftDestroyImage(&output);
    }

    iftDestroyImage(&img);
    iftDestroyMImage(&mimg);
    iftDestroyMImage(&conv);
    iftFree(basename);
    
    printf("Done ... %s\n", iftFormattedTime(iftCompTime(tstart, iftToc())));
    return (0);
}
