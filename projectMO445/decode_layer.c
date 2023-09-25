#include "ift.h"

/* Author: Alexandre Xavier Falcão (September, 10th 2023) 

   Description: Decodes the output of a given layer into an object
   saliency map. It may use the model of each training image
   (model_type=0), the consolidated model (model_type=1) with the
   saved kernel weights, or the consolidated model (model_type != 0
   and != 1) with adaptive weights.

*/


float *LoadKernelWeights(char *filename)
{
  int    number_of_kernels;
  FILE  *fp;
  float *weight;
  
  fp = fopen(filename, "r");
  fscanf(fp, "%d", &number_of_kernels);
  weight = iftAllocFloatArray(number_of_kernels);
  for (int k = 0; k < number_of_kernels; k++) {
    fscanf(fp, "%f ", &weight[k]);
  }
  fclose(fp);

  return(weight);
}

iftAdjRel *GetDiskAdjacency(iftImage *img, iftFLIMLayer layer)
{
  iftAdjRel *A;
  float radius=0.0; 
   
  if (iftIs3DImage(img)){
    for (int i=0; i < 3; i++){
      radius += powf(layer.kernel_size[i],2);
    }
    radius = sqrtf(radius);
    A = iftSpheric(radius);
  }else{
    for (int i=0; i < 2; i++){
      radius += powf(layer.kernel_size[i],2);
    }
    radius = sqrtf(radius)/2.0;
    A = iftCircular(radius);
  }
  return(A);
}

/* Complete the code below to compute adaptive kernel weights */

float *AdaptiveWeights(iftMImage *mimg, float perc_thres)
{
  float *weight     = iftAllocFloatArray(mimg->m);

  for (int b=0; b < mimg->m; b++){



  }

  return(weight);
}

int main(int argc, char *argv[])
{

  /* Example: decode_layer 1 arch.json flim_models 0 salie */
  
  if ((argc!=7)&&(argc!=6)){ 
    iftError("Usage: decode_layer <P1> <P2> <P3> <P4> <P5>\n"
	     "[1] layer\n"
	     "[2] architecture of the network\n"	 
	     "[3] folder with the models\n"	 
	     "[4] model_type (0: one model per image, 1: consolidated model with the saved kernel weights, Otherwise: consolidated model with adaptive weights)\n"
	     "[5] output folder with the salience maps\n"
	     "[6] optional folder with regions of interest (.nii.gz or .png)\n",
	     "main");
  }
  
  timer *tstart = iftTic();

  int layer          = atoi(argv[1]);
  char *filename     = iftAllocCharArray(512);
  sprintf(filename,"layer%d",layer);
  iftFileSet *fs     = iftLoadFileSetFromDirBySuffix(filename, ".mimg", true);
  iftFLIMArch *arch  = iftReadFLIMArch(argv[2]);    
  char *model_dir    = argv[3];
  int model_type = atoi(argv[4]);
  char *output_dir   = argv[5];
  iftMakeDir(output_dir);
  char *roi_dir      = NULL;
  
  if (argc == 7)
    roi_dir = argv[6];
  
  int Imax           = 255;
  
  for(int i = 0; i < fs->n; i++) {
    printf("Processing image %d of %ld\r", i + 1, fs->n);
    char *basename   = iftFilename(fs->files[i]->path,".mimg");      
    iftMImage *mimg  = iftReadMImage(fs->files[i]->path);
    iftImage *mask   = NULL;
    
    /* Compute the scale factor w.r.t. the input of the network */
    float scale[3];
    sprintf(filename,"./layer0/%s.mimg",basename);
    iftMImage *input = iftReadMImage(filename);
    
    scale[0] = (float)input->xsize/(float)mimg->xsize;
    scale[1] = (float)input->ysize/(float)mimg->ysize;
    scale[2] = (float)input->zsize/(float)mimg->zsize;
    iftDestroyMImage(&input);
    
    /* Estimate the salience map */
    
    float *weight=NULL;
    if (model_type==0){
	sprintf(filename,"%s/%s-conv%d-weights.txt",
		model_dir,basename,layer);
      if (iftFileExists(filename)){ 	
	weight = LoadKernelWeights(filename);
      }
    }else{
      sprintf(filename,"%s/conv%d-weights.txt",model_dir,layer);
      if (model_type==1){
	if (iftFileExists(filename)){ 	
	  weight = LoadKernelWeights(filename);
	}
      } else {
	weight = AdaptiveWeights(mimg, 0.10); 
      }	
    }
    
    if (weight != NULL){ 	
      iftFImage *salie = iftCreateFImage(mimg->xsize,mimg->ysize,mimg->zsize);
      /* decode layer */
      for (int p=0; p < mimg->n; p++){
	for (int b=0; b < mimg->m; b++){
	  salie->val[p] += mimg->val[p][b]*weight[b]; 
	}
	if (salie->val[p]<0)
	  salie->val[p]=0; /* ReLU (or Sigmoid?) */ 
      }
      iftFree(weight); 
      
      /* Normalize and Interpolate (upsampling) the salience map */
      iftImage *salie_map   = iftFImageToImage(salie,Imax);	
      iftImage *interp_map = NULL;

      if (argc == 7){
	if (iftIs3DMImage(mimg)){
	  sprintf(filename,"%s/%s.nii.gz",roi_dir,basename);
	}else{
	  sprintf(filename,"%s/%s.png",roi_dir,basename);
	}
	mask = iftReadImageByExt(filename);
      }

      if (iftIs3DMImage(mimg)){
	interp_map = iftInterp(salie_map,scale[0],scale[1],scale[2]);
	sprintf(filename,"%s/%s_layer%d.nii.gz",output_dir,basename,layer);
      }else{
	interp_map = iftInterp2D(salie_map,scale[0],scale[1]);	  
	sprintf(filename,"%s/%s_layer%d.png",output_dir,basename,layer);
      }
      
      /* post-process and save the salience map */
           
      iftAdjRel *A    = GetDiskAdjacency(interp_map,arch->layer[0]);	
      iftImage *close = iftClose(interp_map,A,NULL);
      if (mask != NULL){
	iftDestroyImage(&interp_map);
	interp_map = iftMask(close,mask);
	iftWriteImageByExt(interp_map,filename);
	iftDestroyImage(&mask);
      }else{
	iftWriteImageByExt(close,filename);
      }
      iftDestroyImage(&interp_map);
      iftDestroyAdjRel(&A);
      iftDestroyImage(&salie_map);
      iftDestroyImage(&close);
    }
    
    iftDestroyMImage(&mimg);
    iftFree(basename);
  }
  
  iftFree(filename);
  iftDestroyFileSet(&fs);
  iftDestroyFLIMArch(&arch);
  
  printf("\nDone ... %s\n", iftFormattedTime(iftCompTime(tstart, iftToc())));
  
  return (0);
}
