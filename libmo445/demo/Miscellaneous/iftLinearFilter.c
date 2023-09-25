#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftImage  *img[3];
  iftKernel *K;
  timer     *t1=NULL,*t2=NULL;


  if (argc!=4){
    printf("Kernel types:\n");
    printf("0: Sobel X in 2D\n");
    printf("1: Sobel Y in 2D\n");
    printf("2: Sobel X in 3D\n");
    printf("3: Sobel Y in 3D\n");
    printf("4: Sobel Z in 3D\n");
    printf("5: Gaussian in 2D\n");
    printf("6: Gaussian in 3D\n");
    printf("7: Difference of Gaussians in 2D\n");
    printf("8: Difference of Gaussians in 3D\n");
    printf("9: Sobel gradient magnitude in 2D/3D \n");
    iftError("Usage must be: iftLinearFilter <image.[pgm,scn]> <kernel type> <output_img>","main");
  }

  img[0] = iftReadImageByExt(argv[1]);

  t1     = iftTic();


  switch(atoi(argv[2])){
  case 0:
    K = iftSobelXKernel2D();
    img[1] = iftFastLinearFilter(img[0],K,0);
    iftDestroyKernel(&K);
    break;
  case 1:
    K = iftSobelYKernel2D();
    img[1] = iftFastLinearFilter(img[0],K,0);
    iftDestroyKernel(&K);
    break;
  case 2:
    K = iftSobelXKernel();
    img[1] = iftFastLinearFilter(img[0],K,0);
    iftDestroyKernel(&K);
    break;
  case 3:
    K = iftSobelYKernel();
    img[1] = iftFastLinearFilter(img[0],K,0);
    iftDestroyKernel(&K);
    break;
  case 4:
    K = iftSobelZKernel();
    img[1] = iftFastLinearFilter(img[0],K,0);
    iftDestroyKernel(&K);
    break;
  case 5:
    K = iftGaussianKernel2D(1.5,1.0);
    img[1] = iftFastLinearFilter(img[0],K,0);
    iftDestroyKernel(&K);
    break;
  case 6:
    K = iftGaussianKernel(3.0,1.5);
    img[1] = iftLinearFilter(img[0],K);
    //img[1] = iftFastLinearFilter(img[0],K,0);
    iftDestroyKernel(&K);
    break;
  case 7:
    K = iftDoGKernel2D(1.5,3.0);
    img[1] = iftFastLinearFilter(img[0],K,0);
    iftDestroyKernel(&K);
    break;
  case 8:
    K = iftDoGKernel(1.0,1.8);
    img[1] = iftFastLinearFilter(img[0],K,0);
    iftDestroyKernel(&K);
    break;
  case 9:
    img[1] = iftSobelGradientMagnitude(img[0]);
    break;
  default:
    printf("Kernel types:\n");
    printf("0: Sobel X in 2D\n");
    printf("1: Sobel Y in 2D\n");
    printf("2: Sobel X in 3D\n");
    printf("3: Sobel Y in 3D\n");
    printf("4: Sobel Z in 3D\n");
    printf("5: Gaussian in 2D\n");
    printf("6: Gaussian in 3D\n");
    printf("7: Difference of Gaussians (DoG) in 2D\n");
    printf("8: Difference of Gaussians (DoG) in 3D\n");
    printf("9: Sobel gradient magnitude in 2D/3D \n");
    iftError("Usage must be: iftLinearFilter <image.[pgm,scn]> <kernel type>","main");
  }

  t2     = iftToc();
  fprintf(stdout,"Linear filtering in %f ms\n",iftCompTime(t1,t2));

  iftDestroyImage(&img[0]);

  img[2] = iftAbs(img[1]);
  iftDestroyImage(&img[1]);
    

  iftWriteImageByExt(img[2], argv[3]);    
  iftDestroyImage(&img[2]);


  return(0);

}
