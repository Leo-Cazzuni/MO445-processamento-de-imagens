#include "ift.h"
#define CLUSTER_EPSILON 0.5

double gaussian_kernel(double distance, double kernel_bandwidth){
  return exp(-1.0/2.0 * (distance*distance) / (kernel_bandwidth*kernel_bandwidth));
}

double shift_sample(float *sample_feats, int sample,iftDataSet *Z, float kernel_bandwidth, iftDistanceTable *dist_table){

  float *new_feats=iftAllocFloatArray(Z->nfeats);
  float total_weight=0,weigth;
  double dist;

  for (int s=0;s<Z->nsamples;s++){
//    dist=Z->iftArcWeight(sample_feats,Z->sample[s].feat,Z->alpha,Z->nfeats);
    dist=dist_table->distance_table[sample][s];
    weigth=gaussian_kernel(dist,kernel_bandwidth);
    for (int j=0;j<Z->nfeats;j++){
      new_feats[j]+=Z->sample[s].feat[j]*weigth;
    }
    total_weight+=weigth;
  }

  for (int j=0;j<Z->nfeats;j++)
    new_feats[j]/=total_weight;

  dist=Z->iftArcWeight(new_feats,sample_feats,Z->alpha,Z->nfeats);

  for (int j=0;j<Z->nfeats;j++)
    sample_feats[j]=new_feats[j];

  iftFree(new_feats);

  return dist;

}

void do_clustering(iftDataSet *Z, float kernel_bandwidth){

  /* first apply meanshift*/
  bool *stop_moving=iftAllocBoolArray(Z->nsamples);

  /* create a new dataset for the shifted points */
  iftDataSet *Z_shifted=iftCreateDataSet(Z->nsamples,Z->nfeats);

  /* copy the dataset*/
  for (int s=0;s<Z->nsamples;s++)
    iftCopySample(&Z->sample[s],&Z_shifted->sample[s],Z->nfeats,true);

  double max_shift_dist,shift_dist;

  do{
    max_shift_dist=0;
    iftDistanceTable *dist_table = iftCompEuclDistanceTable(Z_shifted, Z);

//#pragma omp parallel for private(shift_dist) reduction(max:max_shift_dist)
    for (int s=0;s<Z->nsamples;s++){
      if (!stop_moving[s]){
        shift_dist=shift_sample(Z_shifted->sample[s].feat,s,Z,kernel_bandwidth,dist_table);

        if (shift_dist > max_shift_dist)
          max_shift_dist=shift_dist;
        if (shift_dist < IFT_EPSILON)
          stop_moving[s]=true;
        printf("%d\n",s);

      }
    }
    iftDestroyDistanceTable(&dist_table);
    printf("Max shift dist: %.3f\n",max_shift_dist);
  }while(max_shift_dist > IFT_EPSILON);

  int l=0,c;
  iftSample *cluster_modes=iftAlloc(Z->nsamples,sizeof(iftSample *));

  /* do the clustering*/
  for (int s=0;s<Z_shifted->nsamples;s++){
    for (c=0;c<l;c++){
      if (Z_shifted->iftArcWeight(Z_shifted->sample[s].feat,cluster_modes[c].feat,Z_shifted->alpha,Z_shifted->nfeats) <= CLUSTER_EPSILON){
        Z->sample[s].label=c+1;
        break;
      }
    }
    if (c==l){
      cluster_modes[l].feat=Z_shifted->sample[s].feat;
      l++;
      Z->sample[s].label=l;
    }
  }

  Z->nlabels=l;
  iftFree(cluster_modes);
  iftDestroyDataSet(&Z_shifted);

}



int main(int argc, char *argv[])
{

  iftImage        *img=NULL,*label=NULL,*aux=NULL;
  iftDataSet      *Z=NULL;
  iftColor         RGB,YCbCr;
  iftAdjRel       *A=NULL,*B=NULL;
  char             ext[10],*pos;
  timer           *t1=NULL,*t2=NULL;
  int              norm_value;


  if (argc< 5 || argc>6)
    iftError("Usage: iftClusterImageByMeanShift <image.ppm[pgm,png,scn]> <kernel_bandwith> <area> <do_smoothing(0:NO/1:YES)> <gt_image(OPTIONAL)>","main");

  img=iftReadImageByExt(argv[1]);

  /* convert the image to multi-image*/
  iftMImage *mimg;
  iftMImage *eimg;

  if (!iftIsColorImage(img)) {
    mimg = iftImageToMImage(img, GRAYNorm_CSPACE);
    if (img->zsize > 1)
      A = iftSpheric(1.0);
    else
      A = iftCircular(sqrtf(2.0));
    eimg=iftExtendMImageByAdjacencyAndVoxelCoord(mimg,A,1);
    iftDestroyMImage(&mimg);
    iftDestroyAdjRel(&A);
    mimg = eimg;
  }
  else{
    mimg = iftImageToMImage(img, YCbCrNorm_CSPACE);
    eimg = iftExtendMImageByVoxelCoord(mimg,1);
    iftDestroyMImage(&mimg);
    mimg = eimg;
  }

  A=NULL;
  Z= iftMImageToDataSet(mimg);

  /* read the gt image if it was given as parameter*/
  iftImage *gt=NULL;

  t1=iftTic();

  /* do mean_shift*/
  do_clustering(Z,atof(argv[2]));

  t2 = iftToc();
  fprintf(stdout,"%s %d %.3f %d %.2f ",argv[1],Z->nsamples,atof(argv[2]),Z->nlabels,
          iftCompTime(t1,t2));

  if(Z->nclasses>0)
      printf("%.2f ",iftTruePositives(Z));

  label = iftDataSetToLabelImage(Z, false);

  if (!iftIs3DImage(img)){
    iftImage *labels_orig=iftColorizeComp(label);
    iftWriteImageP6(labels_orig,"labels_orig.ppm");
    iftDestroyImage(&labels_orig);
  }
  else{
    iftWriteImage(label,"labels_orig.scn");
  }

  /* do smoothing */
  if (atoi(argv[4])){
    aux=label;
    label = iftSmoothRegionsByDiffusion(aux,img,0.5,2);
    iftDestroyImage(&aux);
  }

  /* this is specifically for the liver database experiments*/
//  if (!iftIsColorImage(img)){
//    for (int p=0;p<img->n;p++){
//      label->val[p]=(img->val[p] <= 20)?0:label->val[p];
//    }
//  }

  aux   = iftSelectAndPropagateRegionsAboveArea(label,atoi(argv[3]));
  iftDestroyImage(&label);
  label=aux;
  printf("%d",iftMaximumValue(label));
  printf("\n");

  if (iftIs3DImage(img)){
    iftWriteImage(label,"labels.scn");
  }
  else{
    iftWriteImageP2(label,"labels.pgm");
    aux   = iftColorizeComp(label);
    iftWriteImageP6(aux,"clusters.ppm");
    iftDestroyImage(&aux);
  }

  /*compute br and ue*/
  iftImage *border = iftBorderImage(label,0);
  if (argc ==6){
    iftImage *gt_img=iftReadImageByExt(argv[5]);
    aux=iftRelabelGrayScaleImage(gt_img);
    iftDestroyImage(&gt_img);
    gt_img=aux;
    iftImage *gt_borders=iftBorderImage(gt_img,0);
    printf("br -> %.4f\n",iftBoundaryRecall(gt_borders, border, 2.0));
    printf("ue -> %.4f\n",iftUnderSegmentation(gt_img, label));

    iftDestroyImage(&gt_img);
    iftDestroyImage(&gt_borders);
  }

  if (iftIs3DImage(img)){
    iftWriteImage(border, "border.scn");
  }
  else
    iftWriteImageP2(border, "border.pgm");

  iftDestroyImage(&border);

  if (!iftIs3DImage(img)){

    norm_value = iftNormalizationValue(iftMaximumValue(img));

    iftDestroyAdjRel(&A);
    A  = iftCircular(sqrtf(2.0));
    B          = iftCircular(0.0);
    RGB.val[0] = 0;
    RGB.val[1] = norm_value;
    RGB.val[2] = norm_value;
    YCbCr      = iftRGBtoYCbCr(RGB,norm_value);
    aux        = iftCopyImage(img);
    iftDrawBorders(aux,label,A,YCbCr,B);

    pos = strrchr(argv[1],'.') + 1;
    sscanf(pos,"%s",ext);
    if (strcmp(ext,"png")==0)
      iftWriteImagePNG(aux,"regions.png");
    else
      iftWriteImageP6(aux,"regions.ppm");

    iftDestroyImage(&aux);
    iftDestroyAdjRel(&B);

  }

  iftDestroyAdjRel(&A);
  iftDestroyImage(&gt);
  iftDestroyImage(&img);
  iftDestroyMImage(&mimg);
  iftDestroyImage(&label);
  iftDestroyDataSet(&Z);

  return(0);
}




