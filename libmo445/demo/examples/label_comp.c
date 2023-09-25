#include "ift.h"

/* examples:

../bin/label_comp ../data/believe.png 3 0.2 0.001  saida
../bin/label_comp ../data/church.png 5 0.2 0.01  saida

*/

char *Basename(char *path)
{
  char *basename     = iftBasename(path);
  iftSList *slist    = iftSplitString(basename,"/");
  strcpy(basename,slist->tail->elem);
  iftDestroySList(&slist);
  return(basename);
}

/* Computes the maximum distance between features of adjacent
   pixels */

float MaxArcWeight(iftMImage *mimg, iftAdjRel *A)
{
  float max_dist=0.0;
  int dx, dy, dz;
  
  iftMaxAdjShifts(A, &dx, &dy, &dz);

  for (int p=0; p < mimg->n; p++) {
    iftVoxel u = iftMGetVoxelCoord(mimg,p);
    if ((u.x >= dx) && (u.y >= dy) &&
	(u.x < (mimg->xsize-dx))&&(u.y < (mimg->ysize-dy))){
      for (int i=1; i < A->n; i++) {
	iftVoxel v = iftGetAdjacentVoxel(A,u,i);
	int q = iftMGetVoxelIndex(mimg,v);
	float dist=0.0;
	for (int b=0; b < mimg->m; b++){
	  dist += (mimg->val[q][b]-mimg->val[p][b])*
	    (mimg->val[q][b]-mimg->val[p][b]);
	}
	if (dist > max_dist)
	  max_dist = dist;
      }
    }
  }

  return(max_dist);
}

float Distance(iftMImage *mimg, int p, int q)
{
  float dist=0.0;

  for (int b=0; b < mimg->m; b++)
    dist += (mimg->val[q][b]-mimg->val[p][b])*(mimg->val[q][b]-mimg->val[p][b]);
  return(dist);
}
  
iftImage *LabelComp(iftMImage *mimg, iftAdjRel *A, float thres)
{
  iftImage *label=NULL;
  int l=1;
  iftFIFO *F=NULL;
  //  iftAdjRel *B = iftCircular(1.0);
  float max_dist = MaxArcWeight(mimg,A);
  // iftDestroyAdjRel(&B);
  
  thres = thres*max_dist;
  
  label  = iftCreateImage(mimg->xsize,mimg->ysize,mimg->zsize);
  F      = iftCreateFIFO(mimg->n);

  for (int r=0; r < label->n; r++){
    if (label->val[r]==0){
      label->val[r]=l;
      iftInsertFIFO(F,r);
      while(!iftEmptyFIFO(F)){
	int p = iftRemoveFIFO(F);
	iftVoxel u = iftGetVoxelCoord(label,p);
	for (int i=1; i < A->n; i++){
	  iftVoxel v = iftGetAdjacentVoxel(A,u,i);
	  if (iftValidVoxel(label,v)){
	    int q = iftGetVoxelIndex(label,v);
	    if ((Distance(mimg,r,q)<=thres)&&(label->val[q] == 0)){
	      label->val[q] = label->val[p];
	      iftInsertFIFO(F,q);
	    }
	  }
	}
      }
      l++;
    }
  }
  iftDestroyFIFO(&F);
  
  return(label);
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
    char       filename[200];

    if (argc!=6)
      iftError("Usage: label_comp <...>\n"
	       "[1] input image .png \n"
	       "[2] adjacency radius \n"
	       "[3] feature distance threshold in (0,1) \n"
	       "[4] minimum area threshold in (0,1) \n"
	       "[5] output directory\n",
	       "main");

    tstart = iftTic();

    iftImage *img = iftReadImageByExt(argv[1]); 
    char     *basename = Basename(argv[1]);    
    iftMImage *mimg = NULL;
    
    if (iftIsColorImage(img))
      mimg = iftImageToMImage(img, LABNorm2_CSPACE); 
    else{
      iftImage *cimg = GrayImageToColorImage(img);
      sprintf(filename,"%s/%s_color.png",argv[5],basename);
      iftWriteImageByExt(cimg,filename);
      mimg           = iftImageToMImage(cimg, LABNorm2_CSPACE);
      iftDestroyImage(&cimg);
    }

    iftMakeDir(argv[5]);

    iftAdjRel *A        = iftCircular(atof(argv[2]));
    iftImage  *label    = LabelComp(mimg,A,atof(argv[3]));    
    
    /* post-processing to eliminate small components */
       
    int minarea_thres   = iftMax(iftRound(atof(argv[4])*label->n),1);
    iftImage *flabel    = iftSelectRegionsAboveAreaAndPropagateTheirLabels(label, minarea_thres);
    
    /* --------------------------------------------- */
    
    iftImage  *clabel   = iftColorizeComp(flabel);
    sprintf(filename,"%s/%s_label.png",argv[5],basename);
    iftWriteImageByExt(flabel,filename);
    sprintf(filename,"%s/%s_colorlabel.png",argv[5],basename);
    iftWriteImageByExt(clabel,filename);
    
    iftDestroyImage(&label);
    iftDestroyImage(&flabel);
    iftDestroyImage(&clabel);
    iftDestroyAdjRel(&A);
    iftDestroyMImage(&mimg);
    iftFree(basename);
    
    printf("Done ... %s\n", iftFormattedTime(iftCompTime(tstart, iftToc())));
    return (0);
}
