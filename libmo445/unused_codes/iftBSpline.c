#include "iftBSpline.h"
/**
@file
@brief A description is missing here
*/
/* ----------------------- Public methods ------------------------------ */

iftMatrix *iftBlendingMatrix(float tension)
{
  iftMatrix *Bm;
  double     beta1, beta2, sigma;

  if ((tension <= IFT_EPSILON) || (tension > 1)){
    iftError("Tension parameter must be in (0,1]","iftBlendingMatrix");
  }
    
  beta1 = 1.0;
  beta2 = 12.0*(1-tension)/(tension);
  sigma = beta2 + 2*pow(beta1,3.0) + 4*pow(beta1,2.0) + 4*beta1 + 2.0;
 
  Bm          =  iftCreateMatrix(4,4);
  Bm->val[0]  = -(2.0*pow(beta1,3.0))/sigma;
  Bm->val[1]  =  (2.0*(beta2 + pow(beta1,3.0) + pow(beta1,2.0) + beta1))/sigma;
  Bm->val[2]  = -(2.0*(beta2 + pow(beta1,2.0) + beta1          + 1))/sigma;
  Bm->val[3]  =  2.0/sigma;
  Bm->val[4]  =  (6.0*pow(beta1,3.0))/sigma;
  Bm->val[5]  = -(3.0*(beta2 + 2.0*pow(beta1,3.0) + 2.0*pow(beta1,2.0)))/sigma;
  Bm->val[6]  =  (3.0*(beta2 + 2.0*pow(beta1,2.0)))/sigma;
  Bm->val[7]  =  0.0;
  Bm->val[8]  = -(6.0*pow(beta1,3.0))/sigma;
  Bm->val[9]  =  (6.0*(pow(beta1,3.0) - beta1))/sigma;
  Bm->val[10] =  (6.0*beta1)/sigma;
  Bm->val[11] =  0.0;
  Bm->val[12] =  (2.0*pow(beta1,3.0))/sigma;
  Bm->val[13] =  (beta2 + 4.0*(pow(beta1,2.0) + beta1))/sigma;
  Bm->val[14] =  2.0/sigma;
  Bm->val[15] =  0.0;

  return(Bm);
}

iftMatrix *iftColorImagePointMatrix(iftImage *img, iftVoxel *u)
{
  iftMatrix *Pm;
  int p;

    Pm          = iftCreateMatrix(6,4);  
    Pm->val[0]  = u[0].x;     
    Pm->val[1]  = u[0].y;     
    Pm->val[2]  = u[0].z;     
    p           = iftGetVoxelIndex(img,u[0]);  
    Pm->val[3]  = img->val[p];     
    Pm->val[4]  = img->Cb[p];     
    Pm->val[5]  = img->Cr[p];     

    Pm->val[6]  = u[1].x;     
    Pm->val[7]  = u[1].y;     
    Pm->val[8]  = u[1].z;     
    p           = iftGetVoxelIndex(img,u[1]);  
    Pm->val[9]  = img->val[p];     
    Pm->val[10] = img->Cb[p];     
    Pm->val[11] = img->Cr[p];     

    Pm->val[12] = u[2].x;     
    Pm->val[13] = u[2].y;     
    Pm->val[14] = u[2].z;     
    p           = iftGetVoxelIndex(img,u[2]);  
    Pm->val[15] = img->val[p];     
    Pm->val[16] = img->Cb[p];     
    Pm->val[17] = img->Cr[p];     

    Pm->val[18] = u[3].x;     
    Pm->val[19] = u[3].y;     
    Pm->val[20] = u[3].z;     
    p           = iftGetVoxelIndex(img,u[3]);  
    Pm->val[21] = img->val[p];     
    Pm->val[22] = img->Cb[p];     
    Pm->val[23] = img->Cr[p];     

  return(Pm);
}

iftMatrix *iftGrayImagePointMatrix(iftImage *img, iftVoxel *u)
{
  iftMatrix *Pm;

    Pm          = iftCreateMatrix(4,4);  
    Pm->val[0]  = u[0].x;     
    Pm->val[1]  = u[0].y;     
    Pm->val[2]  = u[0].z;     
    Pm->val[3]  = img->val[iftGetVoxelIndex(img,u[0])];     
    Pm->val[4]  = u[1].x;     
    Pm->val[5]  = u[1].y;     
    Pm->val[6]  = u[1].z;     
    Pm->val[7]  = img->val[iftGetVoxelIndex(img,u[1])];     
    Pm->val[8]  = u[2].x;     
    Pm->val[9]  = u[2].y;     
    Pm->val[10] = u[2].z;     
    Pm->val[11] = img->val[iftGetVoxelIndex(img,u[2])];     
    Pm->val[12] = u[3].x;     
    Pm->val[13] = u[3].y;     
    Pm->val[14] = u[3].z;     
    Pm->val[15] = img->val[iftGetVoxelIndex(img,u[3])];     

  return(Pm);
}

iftMatrix *iftBSplineInterp(float u, iftMatrix *Bm, iftMatrix *Pm)
{

  if ((u < 0.0)||(u > 1.0))
    iftError("Desired point u must be in [0,1]","iftBSplineInterp");
  
  iftMatrix *U = iftCreateMatrix(4,1);
  iftMatrix *P, *aux; 

  U->val[0] = pow(u,3.0);
  U->val[1] = pow(u,2.0);
  U->val[2] = u;
  U->val[3] = 1.0;

  aux = iftMultMatrices(Bm,Pm);
  P   = iftMultMatrices(U,aux);

  iftDestroyMatrix(&aux);
  iftDestroyMatrix(&U);

  return(P);
}
