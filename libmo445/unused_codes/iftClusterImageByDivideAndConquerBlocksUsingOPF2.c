#include <ift.h>

//nbins should be n^3 for some natural number n
iftDataSet *iftSupervoxelsToHistogramDataSetWithoutNormalize(const iftImage *img, const iftImage *label, int nbins, int *region_size){

  int nregions = iftMaximumValue(label);
  int normalization_value = iftNormalizationValue(iftMaximumValue(img));
  int nbits = 1;

  while ((1 << nbits) <= normalization_value) { /* number of bits per voxel */
    nbits++;
  }

  iftVerifyImageDomains(img,label,"iftSupervoxelsToHistogramDataSet");
  if (iftMinimumValue(label)<=0)
    iftExit("Minimum label value must be 1", "iftSupervoxelsToHistogramDataSet");

  iftDataSet* Z = iftCreateDataSet(nregions,nbins);
  if (iftIsColorImage(img)){
    int xsize   = iftRound(pow(nbins, 1.0 / 3.0));
    int xysize  = xsize*xsize;

    if (xsize < 1)
      iftExit("Insufficient number of bins", "iftSupervoxelsToHistogramDataSet");
    if (xsize > normalization_value)
      iftExit("Excessive number of bins", "iftSupervoxelsToHistogramDataSet");

    float binsize = ((float)pow(2, nbits))/xsize;

    iftColor YCbCr;

    int p,b,r;
    for (p=0; p < img->n; p++){
      r = label->val[p] - 1;
      region_size[r]++;

      YCbCr.val[0] = img->val[p];
      YCbCr.val[1] = img->Cb[p];
      YCbCr.val[2] = img->Cr[p];

      b = (int)( ((float)YCbCr.val[0]) / binsize )
          + xsize*(int)( ((float)YCbCr.val[1]) / binsize )
          + xysize*(int)( ((float)YCbCr.val[2]) / binsize );

      Z->sample[r].feat[b]++;
    }

  }else{ /* Gray scale image */

    float binsize = ((float)pow(2, nbits))/nbins;

    int p,r;
    for (p=0; p < img->n; p++){
      r = label->val[p] - 1;
      region_size[r]++;
      Z->sample[r].feat[(int)(img->val[p]/binsize)]++;
    }

  }

  iftSetDistanceFunction(Z,10);
  Z->ref_data      = (void*) label;
  Z->ref_data_type = IFT_REF_DATA_IMAGE;

  return(Z);
}

bool get_new_val(int *sup_diff_block, int max_labels, iftDataSet *Z, int *regions, int *index, double hist_thresh, float *grad_values, float grad_thresh){

  bool found=false;
  double coeff=0;

  float *f1=iftAllocFloatArray(Z->nfeats);
  float *f2=iftAllocFloatArray(Z->nfeats);

  for (int i=0;i<max_labels && !found;i++)
    for (int j=i+1;j<max_labels && !found;j++)
      if ((sup_diff_block[i * max_labels + j] > 10) && (regions[i] > 0 && regions[j] > 0) ){

        if (grad_values[i*max_labels +j]/sup_diff_block[i*max_labels+j] < grad_thresh){
          *index=i*max_labels +j;
          found=true;
        }
        else{
          /* check the histogram*/
          for (int m=0;m<Z->nfeats;m++){
            f1[m]= Z->sample[i].feat[m] / regions[i];
            f2[m]= Z->sample[j].feat[m] / regions[j];
          }

          coeff=Z->iftArcWeight(f1,f2,Z->alpha,Z->nfeats);
//        printf("Coef value %.3lf\n",coeff);

          if (coeff < hist_thresh){
            *index=i*max_labels +j;
            found=true;
          }
          else{
            sup_diff_block[i*max_labels +j]=0;
          }
        }
//        printf("Trying to merge sup %d and %d\n",i+1,j+1);
      }

  iftFree(f1);
  iftFree(f2);

  return found;
}

/* function to remove the lines produced by the blocks division. Right now I'm using a matrix of sup_size*sup_size to merge adjacent superpixels. This is a previous approach, I need to optimize this approach.*/

iftImage *remove_block_lines(iftImage *img, iftImage *label, iftImage *block_img, double hist_thresh, float grad_thresh){

  iftImage *new_label_img=iftCreateImage(label->xsize,label->ysize,label->zsize);
  int max_label=iftMaximumValue(label);

  /* matrix to store the common adjacency (count of superpixels) between neighboring superpixels that founds in different blocks*/
  int *sup_diff_blocks=iftAllocIntArray(max_label*max_label);
  float *grad_values=iftAllocFloatArray(max_label*max_label);

  for (int i=0;i<max_label*max_label;i++){
    sup_diff_blocks[i]=0;
    grad_values[i]=0;
  }

  iftAdjRel *A;
  if (!iftIs3DImage(img)){
    A=iftCircular(sqrtf(2.0));
  }
  else{
    A=iftSpheric(1.0);
  }

  iftImage *grad = iftImageGradientMagnitude(img,A);
//  iftWriteImageP2(grad,"gradient.pgm");

  int p,q,l1,l2;
  iftVoxel u, v;
  for (int z=0;z < label->zsize;z++)
    for (int y=0;y<label->ysize;y++){
#pragma omp parallel for private(p,q,l1,l2,u,v)
      for (int x=0;x<label->xsize;x++) {
        u.x = x;
        u.y = y;
        u.z = z;
        p = iftGetVoxelIndex(label, u);
        for (int i = 1; i < A->n; i++) {
          v.x = u.x + A->dx[i];
          v.y = u.y + A->dy[i];
          v.z = u.z + A->dz[i];
          if (iftValidVoxel(label, v)) {
            q = iftGetVoxelIndex(label, v);
            if (label->val[p] != label->val[q] && block_img->val[p] != block_img->val[q]) {
              l1=label->val[p]-1;
              l2=label->val[q]-1;
#pragma omp atomic
              sup_diff_blocks[l1 * max_label + l2]++;
#pragma omp atomic
              grad_values[l1*max_label + l2]+=grad->val[p];
            }
          }
        }
      }
    }

  /* put all values in the upper triangular matrix*/
  for (int i=0;i<max_label;i++)
    for (int j=i+1;j<max_label;j++){
      sup_diff_blocks[i*max_label+j]+=sup_diff_blocks[j*max_label+i];
      sup_diff_blocks[j*max_label+i]=0;

      grad_values[i*max_label+j]+=grad_values[j*max_label+i];
      grad_values[j*max_label+i]=0;

    }

  /*create a dataset for the histogram*/
  int *regions=iftAllocIntArray(iftMaximumValue(label));
  int nbins;
  if (iftIsColorImage(img))
    nbins=125;
  else
    nbins=32;
  iftDataSet *Z=iftSupervoxelsToHistogramDataSetWithoutNormalize(img, label, nbins, regions);

  /*join adjacent superpixels */
  int index=-1,row,col;
  int *new_labels=iftAllocIntArray(max_label+1);
  /*init new labels vector*/
  for (int i=1;i<=max_label;i++)
    new_labels[i]=i;

  int curr_val=get_new_val(sup_diff_blocks, max_label, Z, regions, &index, hist_thresh,grad_values,grad_thresh);

  while (curr_val){
    /* compute index to merge*/
    row=index/max_label;
    col=index%max_label;

    /* check if there is the same superpixel*/
    if (row == col){
      sup_diff_blocks[row*max_label +col]=0;
    }
    else{
      /*update the histogram dataset and the regions vector*/
      for (int m=0;m<Z->nfeats;m++)
        Z->sample[row].feat[m]+=Z->sample[col].feat[m];
      regions[row]+=regions[col];
      regions[col]=0;

//     printf("----------------giving sup %d value of sup %d\n",col+1,row+1);

      /* they are merged*/
      sup_diff_blocks[row*max_label +col]=0;
      grad_values[row*max_label+col]=0;
      new_labels[col+1]=row+1;

      /* update the neighbors in the same row, all neighbor superpixels of col will be neighbors of row now */
#pragma omp parallel for
      for (int j=0;j<max_label;j++){
        sup_diff_blocks[row*max_label+j]+=sup_diff_blocks[col*max_label+j];
        sup_diff_blocks[col*max_label+j]=0;

        grad_values[row*max_label+j]+=grad_values[col*max_label+j];
        grad_values[col*max_label+j]=0;

      }

      /*update the neighbors in the same col*/
#pragma omp parallel for
      for (int j=0;j<max_label;j++){
        if (j < row){
          sup_diff_blocks[j*max_label+row]+=sup_diff_blocks[j*max_label+col];
          grad_values[j*max_label+row]+=grad_values[j*max_label+col];
        }
        else{
          sup_diff_blocks[row*max_label+j]+=sup_diff_blocks[j*max_label+col];
          grad_values[row*max_label+j]+=grad_values[j*max_label+col];
        }
        sup_diff_blocks[j*max_label+col]=0;
        grad_values[j*max_label+col]=0;
      }
    }

    curr_val=get_new_val(sup_diff_blocks, max_label, Z, regions, &index, hist_thresh,grad_values,grad_thresh);
  }

  /*update the final labels in the vector*/
#pragma omp parallel for private(l1,l2)
  for (int i=0;i<=max_label;i++){
    l1=new_labels[i];
    l2=new_labels[l1];
    while (l1 != l2){
      l1=l2;
      l2=new_labels[l1];
    }
    new_labels[i]=l1;
  }

  /* substitute the new labels in the image*/
#pragma omp parallel for
  for (int i=0;i<label->n;i++)
    new_label_img->val[i]=new_labels[label->val[i]];

  /* make the labels consecutively*/
  iftImage *aux;
  aux=iftRelabelGrayScaleImage(new_label_img);
  iftDestroyImage(&new_label_img);
  new_label_img=aux;

  iftFree(new_labels);
  iftDestroyDataSet(&Z);
  iftFree(regions);
  iftDestroyImage(&grad);
  iftFree(grad_values);
  iftFree(sup_diff_blocks);

  return new_label_img;

}

int main(int argc, char *argv[])
{
  iftImage        *img=NULL,*label=NULL,*aux=NULL;
  iftDataSet      *Z=NULL;
  iftColor         RGB,YCbCr;
  iftAdjRel       *A=NULL,*B=NULL;
  char             ext[10],*pos;
  timer           *t1=NULL,*t2=NULL;
  iftKnnGraph *graph;


  if (argc<9 || argc >10)
    iftError("Usage: iftClusterImageByDivideAndConquerBlocksUsingOPF2 <image.ppm[pgm,png,scn]> <num_blocks> <train_samples_nb> <kmax_perc first level [0..1]> <kmax_perc second level [0..1]> <area> <treshold (to merge sup in differents blocks [0..1] recomended a value near 0.3)> <grad_thresh> <gt_image(OPTIONAL)>","main");

  iftRandomSeed(IFT_RANDOM_SEED);

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
  fprintf(stdout,"%s %.3f %.3f %.3f ",argv[1],atof(argv[3]),atof(argv[4]),atof(argv[5]));

  /* read the gt image if it was given as parameter*/
  iftImage *gt=NULL;
//  if (argc==9){
//    gt=iftReadImageByExt(argv[8]);
//    iftImageGTToDataSet(gt,Z);
//  }

  t1 = iftTic();
  graph= iftClusterImageByDivideAndConquerBlocksUsingOPF(Z, atoi(argv[2]), atof(argv[3]), iftNormalizedCut,atof(argv[4]), atof(argv[5]));
  t2 = iftToc();
  fprintf(stdout,"%d %.2f ",Z->nlabels,iftCompTime(t1,t2));

  if (Z->nclasses > 0){
    iftSetStatus(Z,TEST);
    printf("%.3f ",iftTruePositives(Z));
  }

  label = iftDataSetToLabelImage(Z, false);

  if (!iftIs3DImage(img)){
    iftImage *labels_orig=iftColorizeComp(label);
    iftWriteImageP6(labels_orig,"labels_orig.ppm");
    iftDestroyImage(&labels_orig);
  }
  else{
    iftWriteImage(label,"labels_orig.scn");
  }

  /* do smoothing*/
  aux=label;
  label = iftSmoothRegionsByDiffusion(aux,img,0.5,2);
  iftDestroyImage(&aux);

  iftImage *block_img=iftMImageTilesToLabelImage(mimg,atoi(argv[2]));

  /* eliminate noise inside blocks*/
  aux   = iftSelectAndPropagateRegionsAboveAreaByColor(img,label,10);
  iftDestroyImage(&label);
  label=aux;

  if (!iftIs3DImage(img)){
    iftImage *aux=iftColorizeComp(label);
    iftWriteImageP6(aux,"after_smooth.ppm");
    iftDestroyImage(&aux);
  }

   /* print regions before merging*/
  if (!iftIs3DImage(img)){
    iftDestroyAdjRel(&A);
    A  = iftCircular(sqrtf(2.0));
    B          = iftCircular(0.0);
    RGB.val[0] = 0;
    RGB.val[1] = 255;
    RGB.val[2] = 255;
    YCbCr      = iftRGBtoYCbCr(RGB,255);
    aux        = iftCopyImage(img);
    iftDrawBorders(aux,label,A,YCbCr,B);

    pos = strrchr(argv[1],'.') + 1;
    sscanf(pos,"%s",ext);
    if (strcmp(ext,"png")==0)
      iftWriteImagePNG(aux,"regions_before.png");
    else
      iftWriteImageP6(aux,"regions_before.ppm");

    iftDestroyImage(&aux);
    iftDestroyAdjRel(&B);
  }

  /* remove block lines*/
  aux= remove_block_lines(img,label, block_img,atof(argv[7]),atof(argv[8]));
  iftDestroyImage(&label);
  label=aux;
  iftDestroyImage(&block_img);

  /* eliminate short superpixels*/
  aux   = iftSelectAndPropagateRegionsAboveAreaByColor(img,label,atoi(argv[6]));
  iftDestroyImage(&label);
  label=aux;

  printf("%d\n",iftMaximumValue(label));

  /* write labels image */
  if (iftIs3DImage(img)){
    iftWriteImage(label,"labels.scn");
  }
  else{
    iftWriteImageP2(label,"labels.pgm");
    aux   = iftColorizeComp(label);
    iftWriteImageP6(aux,"clusters.ppm");
    iftDestroyImage(&aux);
  }

   /*compute br and ue if necessary*/
  iftImage *border = iftBorderImage(label,0);
  if (argc ==10){
    iftImage *gt_img=iftReadImageByExt(argv[9]);
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

    int norm_value;
    iftImage *dens=NULL;
    int s,p,q;
    iftVoxel u;

    dens = iftDataSetWeight(Z);

    norm_value = iftNormalizationValue(iftMaximumValue(img));
    RGB.val[0] = 0;
    RGB.val[1] = norm_value;
    RGB.val[2] = norm_value;
    YCbCr      = iftRGBtoYCbCr(RGB,norm_value);
    B          = iftCircular(1.5);
    aux        = iftCopyImage(img);
    for (s=0; s < graph->nnodes; s++) {
      p = graph->Z->sample[graph->node[s].sample].id;
      u = iftGetVoxelCoord(img,p);
      iftDrawPoint(aux,u,YCbCr,B);
    }
    iftWriteImageP6(aux,"samples.ppm");
    iftDestroyImage(&aux);
    iftDestroyAdjRel(&B);

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

    RGB.val[0] = norm_value;
    RGB.val[1] = 0;
    RGB.val[2] = 0;
    YCbCr      = iftRGBtoYCbCr(RGB,norm_value);
    B          = iftCircular(1.5);
    aux        = iftCopyImage(dens);
    int maxdens = iftMaximumValue(dens);
    for (p=0; p < dens->n; p++)
      aux->val[p] = (int) (((float)dens->val[p]/maxdens)*norm_value);
    for (s=0; s < graph->nnodes; s++) {
      if (graph->node[s].root==s){
        q = graph->Z->sample[graph->node[s].sample].id;
        u = iftGetVoxelCoord(img,q);
        iftDrawPoint(aux,u,YCbCr,B);
      }
    }
    iftWriteImageP6(aux,"maxima.ppm");
    iftDestroyImage(&aux);
    iftDestroyAdjRel(&B);

    /* generate an image with the outliers*/
    RGB.val[0] = norm_value;
    RGB.val[1] = 0;
    RGB.val[2] = 0;
    YCbCr      = iftRGBtoYCbCr(RGB,norm_value);
    B          = iftCircular(1.5);
    aux        = iftCopyImage(dens);
    for (p=0; p < dens->n; p++)
      aux->val[p] = (int) (((float)dens->val[p]/maxdens)*norm_value);
    int outlier_nb=0;
    for (s=0; s < Z->nsamples; s++) {
      if (Z->sample[s].status == OUTLIER){
        q = Z->sample[s].id;
        u = iftGetVoxelCoord(img,q);
        iftDrawPoint(aux,u,YCbCr,B);
        outlier_nb++;
      }
    }
    if (outlier_nb >0)
      iftWriteImageP6(aux,"outlier.ppm");
    else
      iftWriteImageP2(aux,"outlier.ppm");

    iftDestroyImage(&aux);
    iftDestroyAdjRel(&B);
    iftDestroyImage(&dens);
  }

  iftDestroyDataSetWithNoFeatVectors(&graph->Z);
  iftDestroyKnnGraph(&graph);

  iftDestroyImage(&gt);
  iftDestroyAdjRel(&A);
  iftDestroyImage(&img);
  iftDestroyMImage(&mimg);
  iftDestroyImage(&label);
  iftDestroyDataSet(&Z);

  return(0);
}


