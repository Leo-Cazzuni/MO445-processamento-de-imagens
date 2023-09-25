#include "iftFLIM.h"
#include "iftFLIM.cuh"


iftMImage *ReadInputMImage(char *filename);

iftLabeledSet *LabelMarkers(iftMImage *img, iftLabeledSet *S);

iftMatrix *ComputeKernelBank(iftDataSet *Z, int *ngroups, uchar grouping_method, iftKmeansDistance distance);

iftDataSet *ComputeSeedDataSet(iftMImage *img, iftLabeledSet *S, iftAdjRel *A, int nsamples);

void NormalizeImageByZScore(iftMImage *img, float *mean, float *stdev);

void NormalizeImagePerBand(iftMImage *img, iftImage *label);

iftMatrix *LearnKernelBank(iftMImage *input, iftLabeledSet *M, iftAdjRel *A, int nsamples, int nkernels_per_image, int nkernels_per_marker);

iftMatrix *ConsensusKernelbank(iftFileSet *fs_seeds, char *inputdata_dir, int noutput_channels, float stdev_factor);

void StatisticsFromAllSeeds(iftFileSet *fs_seeds, char *inputdata_dir, float *mean, float *stdev, float stdev_factor);

iftMatrix *SelectRelevantKernelsByPCA(iftMatrix *kernels, int number_of_kernels,float  stdev_factor);

iftMatrix *SelectRelevantKernelsByKmeans(iftMatrix *K, int ngroups, iftKmeansDistance distance);

void ReadMeanStdev(char *basepath, float *mean, float *stdev, int ninput_channels);

void WriteMeanStdev(char *basepath, float *mean, float *stdev, int ninput_channels);

float *ReadBias(char *basepath);

void   WriteBias(char *basepath, float *bias, int number_of_kernels);

iftImage *SubsampleImage(iftImage *img, int stride);

void MImageToFeatureMatrixAtRow(iftMImage *mimg, iftAdjRel *A, iftMatrix *matrix, int initRow);

void FLIMExtractFeaturesPerImage(iftFileSet *fs_images, int first, int last, char *orig_dir, iftFLIMArch *arch,
                                 char *param_dir, char *feat_dir, char *object_dir);

void FLIMExtractFeaturesFromLayerPerImage(iftFileSet *fs_images, int first, int last, char *orig_dir, iftFLIMArch *arch,
                                 char *param_dir, int layer_index, char *feat_dir, char *object_dir);

void FLIMExtractFeaturesPerBatch(iftFileSet *fs_images, int first, int last, char *orig_dir, iftFLIMArch *arch,
                                 char *param_dir, char *feat_dir, char *object_dir);

void FLIMExtractFeaturesFromLayerPerBatch(iftFileSet *fs_images, int first, int last, char *orig_dir, iftFLIMArch *arch,
                                          char *param_dir, int layer_index, char *feat_dir, char *object_dir);

iftMImage **
FLIMConvolutionalLayer(iftMImage **input, int nimages, iftImage **mask, iftFLIMArch *arch, int layer, int layer_index, int atrous_factor, char *param_dir);

iftMatrix *KernelBankByPCA(iftDataSet *Z, int *nkernels);





/* ------------ private functions --------------------------------- */

iftMatrix *KernelBankByPCA(iftDataSet *Z, int *nkernels)
{
  /* Estimate the initial kernel bank by PCA */

  iftMatrix *kern_pca  = iftRotationMatrixByPCA(Z);
  iftMatrix *kern_rot  = iftTransposeMatrix(kern_pca);
  iftDestroyMatrix(&kern_pca);
  *nkernels            = iftMin(*nkernels, kern_rot->ncols);
  iftMatrix *kern_bank = iftCreateMatrix(*nkernels, kern_rot->nrows);
  for (int col = 0; col < *nkernels; col++) {
    for (int row = 0; row < kern_rot->nrows; row++) {
      iftMatrixElem(kern_bank, col, row) = iftMatrixElem(kern_rot, col, row);
    }
  }
  iftDestroyMatrix(&kern_rot);
  
  return(kern_bank);
}

iftAdjRel *iftFLIMAdjRelFromKernel(iftFLIMLayer layer, bool dim3D) {
    iftAdjRel *A;

    if (dim3D) {
        A = iftCuboidWithDilationForConv(layer.kernel_size[0], layer.kernel_size[1], layer.kernel_size[2],
                                  layer.dilation_rate[0], layer.dilation_rate[1], layer.dilation_rate[2]);
    } else {
        A = iftRectangularWithDilationForConv(layer.kernel_size[0], layer.kernel_size[1], layer.dilation_rate[0],
                                       layer.dilation_rate[1]);
    }

    return (A);
}

iftAdjRel *iftFLIMAdaptiveAdjRelFromKernel(iftFLIMLayer layer, int atrous_factor, bool dim3D)
{
    iftAdjRel *A;

    if (dim3D) {
        A = iftCuboidWithDilationForConv(layer.kernel_size[0], layer.kernel_size[1], layer.kernel_size[2],
                                  layer.dilation_rate[0]*atrous_factor,
                                  layer.dilation_rate[1]*atrous_factor,
                                  layer.dilation_rate[2]*atrous_factor);
    } else {
        A = iftRectangularWithDilationForConv(layer.kernel_size[0], layer.kernel_size[1], layer.dilation_rate[0]*atrous_factor,
                                       layer.dilation_rate[1]*atrous_factor);
    }

    return (A);
}

void MImageToFeatureMatrixAtRow(iftMImage *mimg, iftAdjRel *A, iftMatrix *matrix, int initRow) {
#pragma omp parallel for
    for (int p = 0; p < mimg->n; p++) {
        iftVoxel u = iftMGetVoxelCoord(mimg, p);
        for (int i = 0; i < A->n; i++) {
            iftVoxel v = iftGetAdjacentVoxel(A, u, i);
            if (iftMValidVoxel(mimg, v)) {
                int q = iftMGetVoxelIndex(mimg, v);
                for (int b = 0; b < mimg->m; b++) {
                    iftMatrixElem(matrix, b + i * mimg->m, initRow + p) = mimg->val[q][b];
                }
            }
        }
    }
}

void FLIMExtractFeaturesPerBatch(iftFileSet *fs_images, int first, int last, char *orig_dir, iftFLIMArch *arch,
                                 char *param_dir, char *feat_dir, char *object_dir) {
    iftMImage **input = NULL, **output = NULL;
    iftImage **mask = NULL;
    char filename[200];
    int nimages = last - first + 1;
    char ext[20];
    sprintf(ext, "%s", iftFileExt(fs_images->files[0]->path));
    char **basename = (char **) calloc(nimages, sizeof(char *));
    input = (iftMImage **) calloc(nimages, sizeof(iftMImage *));
    output = NULL;
    if (object_dir != NULL)
        mask = (iftImage **) calloc(nimages, sizeof(iftImage *));

    /* Load batch */

    for (int i = first, k = 0; i <= last; i++, k++) {
        sprintf(filename, "%s/%s", orig_dir, fs_images->files[i]->path);
	if (strcmp(ext,".mimg") == 0){
	  input[k] = iftReadMImage(filename);
        }else{
	  input[k] = ReadInputMImage(filename);
        }
        basename[k] = iftFilename(fs_images->files[i]->path, ext);
        if (mask != NULL) {
	        sprintf(filename, "%s/%s%s", object_dir, basename[k],ext);
            if (iftFileExists(filename)) {
                mask[k] = iftReadImageByExt(filename);
                int stride = mask[k]->xsize / input[k]->xsize;
                if (stride > 1) {
                    iftImage *aux = SubsampleImage(mask[k], stride);
                    iftDestroyImage(&mask[k]);
                    mask[k] = aux;
                }
            }
        }
    }

    /* For each layer do */

    for (int l = 0; l < arch->nlayers; l++) {
      output = FLIMConvolutionalLayer(input, nimages, mask, arch, l, l+1, 1, param_dir);
      for (int k = 0; k < nimages; k++) {
	iftDestroyMImage(&input[k]);
	input[k] = output[k];
	output[k] = NULL;
      }
      iftFree(output);
    }

    for (int k = 0; k < nimages; k++) {
        sprintf(filename, "%s/%s.mimg", feat_dir, basename[k]);
        iftFree(basename[k]);
        iftWriteMImage(input[k], filename);
        iftDestroyMImage(&input[k]);
        if (mask != NULL)
            iftDestroyImage(&mask[k]);
    }
    iftFree(mask);
    iftFree(input);
    iftFree(basename);
}

void FLIMExtractFeaturesFromLayerPerBatch(iftFileSet *fs_images, int first, int last, char *orig_dir, iftFLIMArch *arch,
                                 char *param_dir, int layer_index, char *feat_dir, char *object_dir) {
    iftMImage **input = NULL, **output = NULL;
    iftImage **mask = NULL;
    char filename[200];
    int nimages = last - first + 1;
    char ext[20];
    sprintf(ext, "%s", iftFileExt(fs_images->files[0]->path));
    char **basename = (char **) calloc(nimages, sizeof(char *));
    input = (iftMImage **) calloc(nimages, sizeof(iftMImage *));
    output = NULL;
    if (object_dir != NULL)
        mask = (iftImage **) calloc(nimages, sizeof(iftImage *));

    /* Load batch */

    for (int i = first, k = 0; i <= last; i++, k++) {
        sprintf(filename, "%s/%s", orig_dir, fs_images->files[i]->path);
        if (strcmp(ext,".mimg") == 0){
            input[k] = iftReadMImage(filename);
        }else{
            input[k] = ReadInputMImage(filename);
        }
        basename[k] = iftFilename(fs_images->files[i]->path, ext);
        if (mask != NULL) {
            if (iftIs3DMImage(input[k])){
                sprintf(filename, "%s/%s.nii.gz", object_dir, basename[k]);
            } else {
                sprintf(filename, "%s/%s.png", object_dir, basename[k]);
            }
            if (iftFileExists(filename)) {
                mask[k] = iftReadImageByExt(filename);
                int stride = mask[k]->xsize / input[k]->xsize;
                if (stride > 1) {
                    iftImage *aux = SubsampleImage(mask[k], stride);
                    iftDestroyImage(&mask[k]);
                    mask[k] = aux;
                }
            }
        }
    }

    /* For each layer do */

    output = FLIMConvolutionalLayer(input, nimages, mask, arch, 0, layer_index, 1, param_dir);
    for (int k = 0; k < nimages; k++) {
        iftDestroyMImage(&input[k]);
        input[k] = output[k];
        output[k] = NULL;
    }
    iftFree(output);

    for (int k = 0; k < nimages; k++) {
        sprintf(filename, "%s/%s.mimg", feat_dir, basename[k]);
        iftFree(basename[k]);
        iftWriteMImage(input[k], filename);
        iftDestroyMImage(&input[k]);
        if (mask != NULL)
            iftDestroyImage(&mask[k]);
    }
    iftFree(mask);
    iftFree(input);
    iftFree(basename);
}

void FLIMExtractFeaturesPerImage(iftFileSet *fs_images, int first, int last, char *orig_dir, iftFLIMArch *arch,
                                 char *param_dir, char *feat_dir, char *object_dir) {
    iftMImage **input = NULL, **output = NULL;
    iftImage **mask = NULL;
    char filename[200];

    printf("\n");

    for (int i = first; i <= last; i++) {
        input = (iftMImage **) calloc(1, sizeof(iftMImage *));     
	char ext[20];
	sprintf(ext, "%s", iftFileExt(fs_images->files[i]->path));
	sprintf(filename, "%s/%s", orig_dir, fs_images->files[i]->path);
	if (strcmp(ext,".mimg") == 0){
	  input[0] = iftReadMImage(filename);
	}else{
	  input[0] = ReadInputMImage(filename);
        }
        char *basename = iftFilename(fs_images->files[i]->path, ext);
        if (object_dir != NULL) {
            mask = (iftImage **) calloc(1, sizeof(iftImage *));
	        sprintf(filename, "%s/%s%s", object_dir, basename, ext);
            if (iftFileExists(filename)) {
                mask[0] = iftReadImageByExt(filename);
                int stride = mask[0]->xsize / input[0]->xsize;
                if (stride > 1) {
                    iftImage *aux = SubsampleImage(mask[0], stride);
                    iftDestroyImage(&mask[0]);
                    mask[0] = aux;
                }
            }
        }

        printf("Processing file %s: image %d of %d files\n", basename, i + 1, last - first + 1);
        fflush(stdout);

        for (int l = 0; l < arch->nlayers; l++) {
	  output = FLIMConvolutionalLayer(input, 1, mask, arch, l, l+1, 1, param_dir);
	  iftDestroyMImage(&input[0]);
	  input[0] = output[0];
	  output[0] = NULL;
	  iftFree(output);
        }

        sprintf(filename, "%s/%s.mimg", feat_dir, basename);
        iftFree(basename);
        iftWriteMImage(input[0], filename);
        iftDestroyMImage(&input[0]);
        iftFree(input);
        if (mask != NULL) {
            iftDestroyImage(&mask[0]);
            iftFree(mask);
        }
    }
}

void FLIMExtractFeaturesFromLayerPerImage(iftFileSet *fs_images, int first, int last, char *orig_dir, iftFLIMArch *arch,
                                 char *param_dir, int layer_index, char *feat_dir, char *object_dir) {
    iftMImage **input = NULL, **output = NULL;
    iftImage **mask = NULL;
    char filename[200];

    printf("\n");

    for (int i = first; i <= last; i++) {

        char ext[20];
        sprintf(ext, "%s", iftFileExt(fs_images->files[i]->path));
        sprintf(filename, "%s/%s", orig_dir, fs_images->files[i]->path);
        input = (iftMImage **) calloc(1, sizeof(iftMImage *));
        if (strcmp(ext,".mimg") == 0){
            input[0] = iftReadMImage(filename);
        }else{
            input[0] = ReadInputMImage(filename);
        }
        char *basename = iftFilename(fs_images->files[i]->path, ext);
        if (object_dir != NULL) {
            mask = (iftImage **) calloc(1, sizeof(iftImage *));
            if (iftIs3DMImage(input[0])){
                sprintf(filename, "%s/%s.nii.gz", object_dir, basename);
            } else {
                sprintf(filename, "%s/%s.png", object_dir, basename);
            }
            if (iftFileExists(filename)) {
                mask[0] = iftReadImageByExt(filename);
                int stride = mask[0]->xsize / input[0]->xsize;
                if (stride > 1) {
                    iftImage *aux = SubsampleImage(mask[0], stride);
                    iftDestroyImage(&mask[0]);
                    mask[0] = aux;
                }
            }
        }

        printf("Processing file %s: image %d of %d files\n", basename, i + 1, last - first + 1);
        fflush(stdout);

        output = FLIMConvolutionalLayer(input, 1, mask, arch, 0, layer_index, 1, param_dir);
        iftDestroyMImage(&input[0]);
        input[0] = output[0];
        output[0] = NULL;
        iftFree(output);

        sprintf(filename, "%s/%s.mimg", feat_dir, basename);
        iftFree(basename);
        iftWriteMImage(input[0], filename);
        iftDestroyMImage(&input[0]);
        iftFree(input);
        if (mask != NULL) {
            iftDestroyImage(&mask[0]);
            iftFree(mask);
        }
    }
}

iftMImage *ReadInputMImage(char *filename) {
    iftImage  *img     = iftReadImageByExt(filename);
    iftMImage *input   = iftImageToMImage(img, LABNorm2_CSPACE); 
    iftDestroyImage(&img);    
    return (input);
}

iftLabeledSet *LabelMarkers(iftMImage *img, iftLabeledSet *S) {
    iftLabeledSet *M = NULL, *seed = S;
    iftImage *markers = iftCreateImage(img->xsize, img->ysize, img->zsize);
    iftAdjRel *A = NULL;

    if (iftIs3DMImage(img))
        A = iftSpheric(sqrtf(3.0));
    else
        A = iftCircular(sqrtf(2.0));

    while (seed != NULL) {
        int p = seed->elem;
        markers->val[p] = 255;
        seed = seed->next;
    }

    iftImage *lbmarkers = iftLabelComp(markers, A);
    iftDestroyAdjRel(&A);
    iftDestroyImage(&markers);

    seed = S;
    while (seed != NULL) {
        int p = seed->elem;
        iftInsertLabeledSet(&M, p, lbmarkers->val[p]);
        seed = seed->next;
    }

    iftDestroyImage(&lbmarkers);

    return (M);
}

iftDataSet *ComputeSeedDataSet(iftMImage *img, iftLabeledSet *S, iftAdjRel *A, int nsamples) {
  int tensor_size = img->m * A->n;
  iftDataSet *Z = iftCreateDataSet(nsamples, tensor_size);
  int ninput_channels = img->m;

  Z->nclasses = 0;
  int s       = 0;
  iftLabeledSet *seed = S;
  while (seed != NULL) {
    int p = seed->elem;
    Z->sample[s].id = s;
    Z->sample[s].truelabel = seed->label;
    if (Z->sample[s].truelabel > Z->nclasses)
      Z->nclasses = Z->sample[s].truelabel;
    iftVoxel u = iftMGetVoxelCoord(img, p);
    int j = 0;
    for (int k = 0; k < A->n; k++) {
      iftVoxel v = iftGetAdjacentVoxel(A, u, k);
      if (iftMValidVoxel(img, v)) {
	int q = iftMGetVoxelIndex(img, v);
	for (int b = 0; b < ninput_channels; b++) {
	  Z->sample[s].feat[j] = img->val[q][b];
	  j++;
	}
      } else {
	for (int b = 0; b < img->m; b++) {
	  Z->sample[s].feat[j] = 0;
	  j++;
	}
      }
    }
    s++;
    seed = seed->next;
  }
  
  iftSetStatus(Z, IFT_TRAIN);
  iftAddStatus(Z, IFT_SUPERVISED);
  
  return (Z);
}

iftMatrix *ComputeKernelBank(iftDataSet *Z, int *ngroups, uchar grouping_method, iftKmeansDistance distance) {
  iftMatrix *kernels = NULL;
  iftGraph *graph = NULL; /* For IOPF and IW */
  iftKnnGraph *knngraph = NULL;
  int i = 0;
  int *groupsize=NULL;

  if (distance == NULL)
    distance = iftEuclideanDistance;
  
  iftRandomSeed(42);

  if (*ngroups >= Z->nsamples) { /* use all markers instead */
    *ngroups = Z->nsamples;
    kernels  = iftCreateMatrix(*ngroups, Z->nfeats);
    for (int s = 0; s < Z->nsamples; s++) {
      iftUnitNorm(Z->sample[s].feat, Z->nfeats);
      for (int j = 0; j < Z->nfeats; j++){
	iftMatrixElem(kernels, i, j) = Z->sample[s].feat[j];
      }
      i++;
    }
    return(kernels);
  }
  
  switch (grouping_method) {
    
  case 0: /* KMeans */
    kernels = iftCreateMatrix(*ngroups, Z->nfeats);
    iftKMeans(Z, *ngroups, 100, 0.001, NULL, NULL, distance);
    groupsize = iftAllocIntArray(iftCountNumberOfGroupsDataSet(Z)+1);
    for (int s = 0; s < Z->nsamples; s++) {
      groupsize[Z->sample[s].group]++;
    }
    for (int s = 0; s < Z->nsamples; s++) {
      if (iftHasSampleStatus(Z->sample[s], IFT_PROTOTYPE)) {
	iftUnitNorm(Z->sample[s].feat, Z->nfeats);
	for (int j = 0; j < Z->nfeats; j++){
	  iftMatrixElem(kernels, i, j) = Z->sample[s].feat[j];
	}
	i++;
      }
    }
    
      
    break;

  case 1: /* IW in a complete graph */
      
    iftAddStatus(Z,IFT_TRAIN);
    graph = iftCreateGraph(Z);
    graph->is_complete = true;
    iftIteratedWatersheds(graph, *ngroups, 100, IFT_MAX, IFT_DATA_FEAT, true);
    kernels = iftCreateMatrix(*ngroups, Z->nfeats);
    for (int i = 0; i < *ngroups; i++) {
      int u = graph->centroids[i];
      int s = graph->node[u].sample;
      //iftUnitNorm(Z->sample[s].feat, Z->nfeats);
      for (int j = 0; j < Z->nfeats; j++)
	iftMatrixElem(kernels, i, j) = Z->sample[s].feat[j];
    }
    iftDestroyGraph(&graph);
    
    break;
    
  case 2: /* OPF C Clusters */
    
    iftAddStatus(Z,IFT_TRAIN);
    knngraph = iftCreateKnnGraph(Z,iftMax(Z->nsamples-1,1));
    *ngroups = iftUnsupTrainWithCClusters(knngraph, *ngroups);
    
    kernels = iftCreateMatrix(*ngroups, Z->nfeats);
    for (int s = 0; s < Z->nsamples; s++) {
      if (iftHasSampleStatus(Z->sample[s], IFT_PROTOTYPE)) {
	//iftUnitNorm(Z->sample[s].feat, Z->nfeats);
	for (int j = 0; j < Z->nfeats; j++){
	  iftMatrixElem(kernels, i, j) = Z->sample[s].feat[j];
	}
	i++;
      }
    }
    iftDestroyKnnGraph(&knngraph);
    
    break;
    
  default:
    iftError("Grouping method must be 0 for KMeans, 1 for IW", "ComputeKernelBank");
  }
  iftFree(groupsize);
  
  return (kernels);
}

void NormalizeImageByZScore(iftMImage *img, float *mean, float *stdev) {
    int ninput_channels = img->m;

#pragma omp parallel for
    for (int p = 0; p < img->n; p++) {
        for (int b = 0; b < ninput_channels; b++) {
            img->val[p][b] = (img->val[p][b] - mean[b]) / stdev[b];
        }
    }
}

void NormalizeImagePerBand(iftMImage *img, iftImage *label) {
    if (label == NULL) {
        for (int b = 0; b < img->m; b++) {
            float Imin = IFT_INFINITY_FLT, Imax = IFT_INFINITY_FLT_NEG;
            for (int p = 0; p < img->n; p++) {
                if (img->val[p][b] > Imax)
                    Imax = img->val[p][b];
                if ((img->val[p][b] < Imin) && (img->val[p][b] > 0))
                    Imin = img->val[p][b];
            }
            if (Imin < Imax)
                for (int p = 0; p < img->n; p++) {
                    img->val[p][b] = (img->val[p][b] - Imin) / (Imax - Imin);
                    if (img->val[p][b] < 0)
                        img->val[p][b] = 0;
                }
        }
    } else {
        for (int b = 0; b < img->m; b++) {
            float Imin = IFT_INFINITY_FLT, Imax = IFT_INFINITY_FLT_NEG;
            for (int p = 0; p < img->n; p++) {
                if (label->val[p] != 0) {
                    if (img->val[p][b] > Imax)
                        Imax = img->val[p][b];
                    if ((img->val[p][b] < Imin) && (img->val[p][b] > 0))
                        Imin = img->val[p][b];
                }
            }
            if (Imin < Imax)
                for (int p = 0; p < img->n; p++) {
                    if (label->val[p] != 0) {
                        img->val[p][b] = (img->val[p][b] - Imin) / (Imax - Imin);
                        if (img->val[p][b] < 0)
                            img->val[p][b] = 0;
                    } else {
                        img->val[p][b] = 0;
                    }
                }
        }
    }
}

iftMatrix *SelectRelevantKernelsByPCA(iftMatrix *kernels, int number_of_kernels,float stdev_factor) {
    iftMatrix *kern_t = iftTransposeMatrix(kernels);
    iftDataSet *Z = iftFeatureMatrixToDataSet(kern_t);
    iftDestroyMatrix(&kern_t);

    iftSetStatus(Z, IFT_TRAIN);
    iftNormalizeDataSetByZScoreInPlace(Z,NULL,stdev_factor);

    iftMatrix *kern_pca = iftRotationMatrixByPCA(Z);
    iftMatrix *kern_rot = iftTransposeMatrix(kern_pca);
    iftDestroyMatrix(&kern_pca);

    number_of_kernels = iftMin(number_of_kernels, kern_rot->ncols);
    iftMatrix *M = iftCreateMatrix(number_of_kernels, kern_rot->nrows);

    for (int k = 0; k < number_of_kernels; k++) {
        for (int j = 0; j < kern_rot->nrows; j++) {
            iftMatrixElem(M, k, j) = iftMatrixElem(kern_rot, k, j);
        }
    }

    iftDestroyMatrix(&kern_rot);
    iftDestroyDataSet(&Z);
    
    return (M);
}

iftMatrix *SelectRelevantKernelsByKmeans(iftMatrix *K, int ngroups, iftKmeansDistance distance) {
    iftMatrix *Kt = iftTransposeMatrix(K);
    iftDataSet *Z = iftFeatureMatrixToDataSet(Kt);
    iftDestroyMatrix(&Kt);

    iftRandomSeed(42);


    iftMatrix *kernels = iftCreateMatrix(ngroups, Z->nfeats);

    iftKMeans(Z, ngroups, 100, 0.001, NULL, NULL, distance);
    int i = 0;
    for (int s = 0; s < Z->nsamples; s++) {
        if (iftHasSampleStatus(Z->sample[s], IFT_PROTOTYPE)) {
	  //iftUnitNorm(Z->sample[s].feat, Z->nfeats);
            for (int j = 0; j < Z->nfeats; j++)
                iftMatrixElem(kernels, i, j) = Z->sample[s].feat[j];
            i++;
        }
    }
    iftDestroyDataSet(&Z);
    return (kernels);
}

iftMatrix *LearnKernelBank(iftMImage *input, iftLabeledSet *M, iftAdjRel *A, int nsamples, int nkernels_per_image, int nkernels_per_marker) {
    int tensor_size     = input->m * A->n;
    iftDataSet *Z       = ComputeSeedDataSet(input, M, A, nsamples);
    iftMatrix **kernels = (iftMatrix **) calloc(Z->nclasses + 1, sizeof(iftMatrix *));
    int total_nkernels  = 0;
    
    for (int c = 1; c <= Z->nclasses; c++) {
        iftDataSet *Z1  = iftExtractSamplesFromClass(Z, c);	
        int nkernels    = nkernels_per_marker;
	/* 0: kmeans, 1: iterated watershed and 2: OPF c clusters */
	//kernels[c]      = ComputeKernelBank(Z1, &nkernels, 0, iftCosineDistance2);
	kernels[c]      = ComputeKernelBank(Z1, &nkernels, 0, iftEuclideanDistance);
        total_nkernels += nkernels;
        iftDestroyDataSet(&Z1);
    }
    
    iftMatrix *kernelbank = iftCreateMatrix(total_nkernels, tensor_size);

    int k = 0;
    for (int c = 1; c <= Z->nclasses; c++) {
        for (int col = 0; col < kernels[c]->ncols; col++, k++) {
            for (int row = 0; row < kernels[c]->nrows; row++) {
                iftMatrixElem(kernelbank, k, row) = iftMatrixElem(kernels[c], col, row);
            }
        }
    }

    for (int c = 0; c <= Z->nclasses; c++) {
        iftDestroyMatrix(&kernels[c]);
    }
    iftFree(kernels);
    iftDestroyDataSet(&Z);
	   
    if (kernelbank->ncols > nkernels_per_image) { /* force a number of kernels per image */
      //      iftMatrix *Rkernels = SelectRelevantKernelsByKmeans(kernelbank, nkernels_per_image, iftCosineDistance2);
      iftMatrix *Rkernels = SelectRelevantKernelsByKmeans(kernelbank, nkernels_per_image, iftEuclideanDistance);
      iftDestroyMatrix(&kernelbank);
      kernelbank = Rkernels;
    }
    
   return (kernelbank);
}

iftMImage **
FLIMConvolutionalLayer(iftMImage **input, int nimages, iftImage **mask, iftFLIMArch *arch, int layer, int layer_index, int atrous_factor, char *param_dir) {
    /* Set input parameters */

    iftAdjRel *A = iftFLIMAdaptiveAdjRelFromKernel(arch->layer[layer], atrous_factor, iftIs3DMImage(input[0]));
    int ninput_channels = input[0]->m;
    char *basename = NULL;
    iftMImage **output = (iftMImage **) calloc(nimages, sizeof(iftMImage *));

    /* Read consensus parameters of the current layer */

    char filename[200];
    sprintf(filename, "%s/conv%d-kernels.npy", param_dir, layer_index);
    iftMatrix *kernelbank = iftReadMatrix(filename);
    float *mean  = iftAllocFloatArray(ninput_channels);
    float *stdev = iftAllocFloatArray(ninput_channels);
    sprintf(filename, "%s/conv%d", param_dir, layer_index);
    ReadMeanStdev(filename, mean, stdev, ninput_channels);
    /* BIAS: use environment variable to use the bias idea */
    float *bias = NULL;
    char *use_bias = getenv("USE_BIAS");

    if (use_bias!=NULL){
    	bias = ReadBias(filename);
    } 
    
    /* Apply convolutional layer */
    for (int i = 0; i < nimages; i++) {

        /* marker-based normalization */

    /* BIAS: bias do not need normalization, but fill the matrix with mean */
	iftMatrix *imgM;    
	if (use_bias==NULL){
	  NormalizeImageByZScore(input[i], mean, stdev);
	  imgM = iftMImageToFeatureMatrix(input[i], A, NULL);
	}else{
          imgM = iftMImageToFeatureMatrix(input[i], A, mean);
	}

	/* convolution */

	iftMatrix *conv = iftMultMatrices(imgM, kernelbank);
	iftDestroyMatrix(&imgM);

	iftMImage *activ = iftMatrixToMImage(conv, input[i]->xsize, input[i]->ysize, input[i]->zsize, kernelbank->ncols, 'c');

	
        iftDestroyMatrix(&conv);
	
        if (mask != NULL) { /* apply mask whenever it is the case */		
	  int stride = mask[i]->xsize / activ->xsize;
	  if (stride > 1) {
	    iftImage *aux = SubsampleImage(mask[i], stride);
	    iftDestroyImage(&mask[i]);
	    mask[i] = aux;
	  }
	  
#pragma omp parallel for
	  for (int p = 0; p < activ->n; p++) {
	    if (mask[i]->val[p] == 0)
	      for (int b = 0; b < activ->m; b++) {
		activ->val[p][b] = 0;
	      }
	  }
        }

        /* activation in place */
	
        if (arch->layer[layer].relu) { /* ReLU in place */
#pragma omp parallel for
	  for (int p = 0; p < activ->n; p++) {
	    for (int b = 0; b < activ->m; b++) {
	      /* BIAS: check for the environment variable */
	      if (use_bias!=NULL){
		      activ->val[p][b] += bias[b];
	      }
	      if (activ->val[p][b] < 0)
		activ->val[p][b] = 0;
	    }
	  }
        }
        
        /* pooling */
	
        if (strcmp(arch->layer[layer].pool_type, "no_pool") != 0) {
            iftMImage *pool = NULL;
            if (strcmp(arch->layer[layer].pool_type, "avg_pool") == 0) { /* ignore the stride to learn the model */
                pool = iftFLIMAtrousAveragePooling(activ, arch->layer[layer].pool_size[0], arch->layer[layer].pool_size[1], arch->layer[layer].pool_size[2], atrous_factor, arch->layer[layer].pool_stride);
                iftDestroyMImage(&activ);
                activ = pool;
            } else {
                if (strcmp(arch->layer[layer].pool_type, "max_pool") == 0) { /* ignore the stride to learn the model */
                    pool = iftFLIMAtrousMaxPooling(activ, arch->layer[layer].pool_size[0], arch->layer[layer].pool_size[1], arch->layer[layer].pool_size[2], atrous_factor, arch->layer[layer].pool_stride);
                    iftDestroyMImage(&activ);
                    activ = pool;
                } else {
                    iftWarning("Invalid pooling type has been ignore", "FLIMConvolutionalLayer");
                }
            }
        }

        output[i] = iftCopyMImage(activ);
        iftDestroyMImage(&activ);
    }

    iftDestroyAdjRel(&A);
    iftDestroyMatrix(&kernelbank);
    iftFree(mean);
    iftFree(stdev);
    /* BIAS: free bias array */
    if (use_bias!=NULL){
	    iftFree(bias);
    }
    return output;
}

iftMatrix *ConsensusKernelbank(iftFileSet *fs_seeds, char *inputdata_dir, int noutput_channels, float stdev_factor) {
    int nkernels = fs_seeds->n;
    iftMatrix **kernels = (iftMatrix **) calloc(nkernels, sizeof(iftMatrix *));
    int ncols = 0;
    int nrows = 0;
    char filename[200];

    /* Load kernels from the training images */
    for (int i = 0; i < nkernels; i++) {
        char *basename = iftFilename(fs_seeds->files[i]->path, "-seeds.txt");
        sprintf(filename, "%s/%s-kernels.npy", inputdata_dir, basename);
        kernels[i] = iftReadMatrix(filename);
        ncols += kernels[i]->ncols;
        iftFree(basename);
    }
    nrows = kernels[0]->nrows;

    /* Copy all kernels into a single matrix */

    iftMatrix *Mkernels = iftCreateMatrix(ncols, nrows);
    int k = 0;
    for (int i = 0; i < nkernels; i++) {
        for (int col = 0; col < kernels[i]->ncols; col++, k++) {
            for (int row = 0; row < kernels[i]->nrows; row++) {
                iftMatrixElem(Mkernels, k, row) = iftMatrixElem(kernels[i], col, row);
            }
        }
    }

    for (int i = 0; i < nkernels; i++) {
        iftDestroyMatrix(&kernels[i]);
    }
    iftFree(kernels);

    /* Reduce the number of kernels into the desired number of output channels */

    iftMatrix *Rkernels = NULL;
    
    if (Mkernels->ncols <= noutput_channels) {
        Rkernels = iftCopyMatrix(Mkernels);
    } else {
      printf("number of kernels %d, feature space dimension %d\n",Mkernels->ncols,Mkernels->nrows);
      if (Mkernels->ncols > Mkernels->nrows){
      	Rkernels = SelectRelevantKernelsByPCA(Mkernels, noutput_channels, stdev_factor);
      	printf("\n By PCA\n");
      }else{
	//	Rkernels = SelectRelevantKernelsByKmeans(Mkernels, noutput_channels, iftCosineDistance2);
	Rkernels = SelectRelevantKernelsByKmeans(Mkernels, noutput_channels, iftEuclideanDistance);
	printf("\n By Kmeans\n");
      }
    }

    iftDestroyMatrix(&Mkernels);

    return (Rkernels);
}

void StatisticsFromAllSeeds(iftFileSet *fs_seeds, char *inputdata_dir, float *mean, float *stdev, float stdev_factor) {
    int nseeds = 0, ninput_channels = 0;
    char *basename = NULL;
    char filename[200];
    iftMImage *input = NULL;

    for (int i = 0; i < fs_seeds->n; i++) {
        basename = iftFilename(fs_seeds->files[i]->path, "-seeds.txt");
        sprintf(filename, "%s/%s.mimg", inputdata_dir, basename);
        input = iftReadMImage(filename);
        iftFree(basename);

        ninput_channels = input->m;
        iftLabeledSet *S = iftMReadSeeds(input, fs_seeds->files[i]->path);
        while (S != NULL) {
            int l;
            int p = iftRemoveLabeledSet(&S, &l);
            nseeds += 1;
            for (int b = 0; b < ninput_channels; b++) {
                mean[b] += input->val[p][b];
            }
        }
        iftDestroyMImage(&input);
    }

    for (int b = 0; b < ninput_channels; b++) {
        mean[b] = mean[b] / nseeds;
    }

    for (int i = 0; i < fs_seeds->n; i++) {
        basename = iftFilename(fs_seeds->files[i]->path, "-seeds.txt");
        sprintf(filename, "%s/%s.mimg", inputdata_dir, basename);
        input = iftReadMImage(filename);
        iftFree(basename);

        ninput_channels = input->m;
        iftLabeledSet *S = iftMReadSeeds(input, fs_seeds->files[i]->path);
        while (S != NULL) {
            int l;
            int p = iftRemoveLabeledSet(&S, &l);
            for (int b = 0; b < ninput_channels; b++) {
                stdev[b] += (input->val[p][b] - mean[b]) * (input->val[p][b] - mean[b]);
            }
        }
        iftDestroyMImage(&input);
    }

    for (int b = 0; b < ninput_channels; b++) {
        stdev[b] = sqrtf(stdev[b] / nseeds) + stdev_factor;
    }

}

void ReadMeanStdev(char *basepath, float *mean, float *stdev, int ninput_channels) {
    char filename[2][200];
    FILE *fp[2];

    sprintf(filename[0], "%s-mean.txt", basepath);
    sprintf(filename[1], "%s-stdev.txt", basepath);
    fp[0] = fopen(filename[0], "r");
    fp[1] = fopen(filename[1], "r");
    for (int b = 0; b < ninput_channels; b++) {
        if (fscanf(fp[0], "%f", &mean[b]) != 1);
        if (fscanf(fp[1], "%f", &stdev[b]) != 1);
    }
    fclose(fp[0]);
    fclose(fp[1]);
}

void WriteMeanStdev(char *basepath, float *mean, float *stdev, int ninput_channels) {
    char filename[2][200];
    FILE *fp[2];

    sprintf(filename[0], "%s-mean.txt", basepath);
    sprintf(filename[1], "%s-stdev.txt", basepath);
    fp[0] = fopen(filename[0], "w");
    fp[1] = fopen(filename[1], "w");
    for (int b = 0; b < ninput_channels; b++) {
        fprintf(fp[0], "%f ", mean[b]);
        fprintf(fp[1], "%f ", stdev[b]);
    }
    fclose(fp[0]);
    fclose(fp[1]);
}

float *ReadBias(char *basepath) {
    char filename[200];
    FILE *fp;
    int number_of_kernels;
    
    sprintf(filename, "%s-bias.txt", basepath);
    fp = fopen(filename, "r");
    if (fscanf(fp, "%d", &number_of_kernels)!=1);
    float *bias = iftAllocFloatArray(number_of_kernels);
    for (int k = 0; k < number_of_kernels; k++) {
      if (fscanf(fp, "%f", &bias[k])!=1);
    }
    fclose(fp);

    return(bias);
}

void WriteBias(char *basepath, float *bias, int number_of_kernels) {
    char filename[200];
    FILE *fp;

    sprintf(filename, "%s-bias.txt", basepath);
    fp = fopen(filename, "w");
    fprintf(fp, "%d\n", number_of_kernels);
    for (int k = 0; k < number_of_kernels; k++) {
        fprintf(fp, "%f ", bias[k]);
    }
    fclose(fp);
}

iftImage *SubsampleImage(iftImage *img, int stride) {
    iftImage *simg = iftCreateImage(ceilf(img->xsize / (float) stride), ceilf(img->ysize / (float) stride),
                                    iftMax(ceilf(img->zsize / (float) stride), 1));
    int q = 0;
    iftVoxel u;
    for (u.z = 0; u.z < img->zsize; u.z = u.z + stride) {
        for (u.y = 0; u.y < img->ysize; u.y = u.y + stride) {
            for (u.x = 0; u.x < img->xsize; u.x = u.x + stride) {
                int p = iftGetVoxelIndex(img, u);
                simg->val[q] = img->val[p];
                q++;
            }
        }
    }

    return (simg);
}

/* --------------------- public functions ----------------------------- */

iftFLIMArch *iftReadFLIMArch(char *filename) {
    iftDict *arch_dict = iftReadJson(filename);
    int nlayers = iftGetLongValFromDict("nlayers", arch_dict);
    float stdev_factor = iftGetDblValFromDict("stdev_factor", arch_dict);
    bool intrinsic_atrous = FALSE;
    if (iftDictContainKey("apply_intrinsic_atrous", arch_dict, NULL)) {
        intrinsic_atrous = iftGetBoolValFromDict("apply_intrinsic_atrous", arch_dict);
    }
    iftFLIMArch *arch = (iftFLIMArch *) calloc(1, sizeof(iftFLIMArch));

    arch->nlayers = nlayers;
    arch->stdev_factor = stdev_factor;
    arch->layer = (iftFLIMLayer *) calloc(arch->nlayers, sizeof(iftFLIMLayer));
    arch->apply_intrinsic_atrous = intrinsic_atrous;

    for (int l = 0; l < arch->nlayers; l++) {

        char name[50];
        sprintf(name, "layer%d", l + 1);
        iftDict *layer_dict = iftGetDictFromDict(name, arch_dict);
        iftDict *conv_dict = iftGetDictFromDict("conv", layer_dict);
        iftIntArray *input = iftGetIntArrayFromDict("kernel_size", conv_dict);
        for (int i = 0; i < 3; i++)
            arch->layer[l].kernel_size[i] = input->val[i];
        iftFree(input);
        input = iftGetIntArrayFromDict("dilation_rate", conv_dict);
        for (int i = 0; i < 3; i++)
            arch->layer[l].dilation_rate[i] = input->val[i];
        iftFree(input);
        arch->layer[l].nkernels_per_image = iftGetLongValFromDict("nkernels_per_image", conv_dict);
        arch->layer[l].nkernels_per_marker = iftGetLongValFromDict("nkernels_per_marker", conv_dict);
        arch->layer[l].noutput_channels = iftGetLongValFromDict("noutput_channels", conv_dict);
        arch->layer[l].relu = iftGetBoolValFromDict("relu", layer_dict);
        iftDict *pool_dict = iftGetDictFromDict("pooling", layer_dict);
        arch->layer[l].pool_type = iftGetStrValFromDict("type", pool_dict);
        input = iftGetIntArrayFromDict("size", pool_dict);
        for (int i = 0; i < 3; i++)
            arch->layer[l].pool_size[i] = input->val[i];
        iftFree(input);
        arch->layer[l].pool_stride = iftGetLongValFromDict("stride", pool_dict);

        iftDestroyDict(&conv_dict);
        iftDestroyDict(&pool_dict);

        iftDestroyDict(&layer_dict);
    }

    iftDestroyDict(&arch_dict);

    return (arch);
}

void iftWriteFLIMArch(iftFLIMArch *arch, char *filename)
{
    iftDict *new_arch = iftCreateDict();

    /* Dictionary does not copy nested dicts, and therefore all nested dicts must be allocated first */
    iftDict *layer[arch->nlayers];
    iftDict *conv[arch->nlayers];
    iftDict *pooling[arch->nlayers];

    for (int l = 0; l < arch->nlayers; l++) {
        layer[l] = iftCreateDict();
        conv[l] = iftCreateDict();
        pooling[l] = iftCreateDict();
    }

    iftInsertIntoDict("stdev_factor",arch->stdev_factor, new_arch);
    iftInsertIntoDict("nlayers",arch->nlayers, new_arch);
    iftInsertIntoDict("apply_intrinsic_atrous",arch->apply_intrinsic_atrous, new_arch);

    for (int l = 0; l < arch->nlayers; l++){

        char layername[100];
        sprintf(layername,"layer%d",l+1);

        iftIntArray *kernel_size = iftCreateIntArray(3);
        for (int i = 0; i < 3; i++) kernel_size->val[i] = arch->layer[l].kernel_size[i];
        long nkernels_per_marker = arch->layer[l].nkernels_per_marker;
        iftIntArray *dilation_rate = iftCreateIntArray(3);
        for (int i = 0; i < 3; i++) dilation_rate->val[i] = arch->layer[l].dilation_rate[i];
        long nkernels_per_image = arch->layer[l].nkernels_per_image;
        long noutput_channels = arch->layer[l].noutput_channels;
        iftInsertIntoDict("kernel_size",kernel_size,conv[l]);
        iftInsertIntoDict("nkernels_per_marker",nkernels_per_marker,conv[l]);
        iftInsertIntoDict("dilation_rate",dilation_rate,conv[l]);
        iftInsertIntoDict("nkernels_per_image",nkernels_per_image,conv[l]);
        iftInsertIntoDict("noutput_channels",noutput_channels,conv[l]);
        iftInsertIntoDict("conv",conv[l],layer[l]);

        bool relu = TRUE;
        iftInsertIntoDict("relu",relu,layer[l]);

        char *pool_type = arch->layer[l].pool_type;
        iftIntArray *pool_size = iftCreateIntArray(3);
	for (int i = 0; i < 3; i++) pool_size->val[i] = arch->layer[l].pool_size[i];
        long pool_stride = arch->layer[l].pool_stride;
        iftInsertIntoDict("type",pool_type,pooling[l]);
        iftInsertIntoDict("size",pool_size,pooling[l]);
        iftInsertIntoDict("stride",pool_stride,pooling[l]);
        iftInsertIntoDict("pooling",pooling[l],layer[l]);

        iftInsertIntoDict(layername,layer[l],new_arch);
    }

    iftWriteJson(new_arch,filename);
    for (int l = 0; l < arch->nlayers; l++) {
        iftDestroyDict(&layer[l]);
        iftDestroyDict(&conv[l]);
        iftDestroyDict(&pooling[l]);
    }
    iftDestroyDict(&new_arch);
}

void iftDestroyFLIMArch(iftFLIMArch **arch) {
    iftFLIMArch *aux = *arch;

    if (aux != NULL) {
        for (int l = 0; l < aux->nlayers; l++) {
            iftFree(aux->layer[l].pool_type);
        }
        iftFree(aux->layer);
        iftFree(aux);
        *arch = NULL;
    }
}

void iftFLIMLearnModel(char *orig_dir, char *markers_dir, char *param_dir, iftFLIMArch *arch) {

    /* Set input parameters */

    iftMakeDir("tmp");

    iftFileSet *fs_orig  = iftLoadFileSetFromDirOrCSV(orig_dir, 1, 1);
    iftFileSet *fs_seeds = iftLoadFileSetFromDirBySuffix(markers_dir, "-seeds.txt", 1);
    iftAdjRel  *A        = NULL;
    iftMImage **output   = NULL;
    iftMImage *input     = NULL;
    int ninput_channels  = 0;
    int nimages          = fs_seeds->n;
    int atrous_factor    = 1;
    char *basename       = NULL;
    char filename[200], ext[10];
    
    sprintf(ext, "%s", iftFileExt(fs_orig->files[0]->path));
    printf("%s\n",ext);
    
    /* Generate input layer */
    for (int i = 0; i < nimages; i++) {
        basename = iftFilename(fs_seeds->files[i]->path, "-seeds.txt");
        sprintf(filename, "%s/%s%s", orig_dir, basename, ext);
        if (strcmp(ext,".mimg")==0){
            input = iftReadMImage(filename);
        } else {
            input = ReadInputMImage(filename);
        }	
        sprintf(filename, "tmp/%s.mimg", basename);
        iftWriteMImage(input, filename);
        iftFree(basename);
        iftDestroyMImage(&input);
    }

    /* For each layer do */

    for (int l = 0; l < arch->nlayers; l++) {
        basename        = iftFilename(fs_seeds->files[0]->path, "-seeds.txt");
        sprintf(filename, "tmp/%s.mimg", basename);
        input           = iftReadMImage(filename);
        if (l == 0) {
            A = iftFLIMAdjRelFromKernel(arch->layer[l], iftIs3DMImage(input));
        } else {
            if (arch->apply_intrinsic_atrous) {
                atrous_factor *= arch->layer[l - 1].pool_stride;
                printf("Updating atrous factor\n");
                fflush(stdout);
            }
	    A = iftFLIMAdaptiveAdjRelFromKernel(arch->layer[l], atrous_factor, iftIs3DMImage(input));
        }
        ninput_channels = input->m;
        iftDestroyMImage(&input);

        /* Learn and save marker-based normalization parameters and kernels from each training image */

        float *mean  = iftAllocFloatArray(ninput_channels);
        float *stdev = iftAllocFloatArray(ninput_channels);
        StatisticsFromAllSeeds(fs_seeds, "tmp", mean, stdev, arch->stdev_factor);	
	printf("\nLayer %d\n", l + 1);
        fflush(stdout);

        for (int i = 0; i < nimages; i++) {

            basename = iftFilename(fs_seeds->files[i]->path, "-seeds.txt");
            sprintf(filename, "tmp/%s.mimg", basename);
            input    = iftReadMImage(filename);
	    
            printf("Processing file %s: %d of %d files\r", basename, i + 1, nimages);
            fflush(stdout);

            /* Apply marker-based image normalization */

            NormalizeImageByZScore(input, mean, stdev);
	    sprintf(filename, "tmp/%s-norm.mimg", basename);
	    iftWriteMImage(input,filename);

            /* Assign a distinct label to each marker */

            iftLabeledSet *S = iftMReadSeeds(input, fs_seeds->files[i]->path);
            iftLabeledSet *M = LabelMarkers(input, S);
            iftDestroyLabeledSet(&S);

            /* Learn and save kernel bank */

            int nsamples          = iftLabeledSetSize(M);
            iftMatrix *kernelbank = LearnKernelBank(input, M, A, nsamples, arch->layer[l].nkernels_per_image, arch->layer[l].nkernels_per_marker);
            iftDestroyLabeledSet(&M);
            sprintf(filename, "tmp/%s-kernels.npy", basename);
            iftWriteMatrix(kernelbank, filename);
	    iftDestroyMatrix(&kernelbank);	    
	    iftDestroyMImage(&input);
	    iftFree(basename);
        }

        /* Create a consensus layer (i.e., merge kernels) and save
	   final kernel bank for layer l */

        iftMatrix *kernelbank = ConsensusKernelbank(fs_seeds, "tmp", arch->layer[l].noutput_channels, arch->stdev_factor);

	printf("\n nkernels %d\n", kernelbank->ncols);	
    
	/* BIAS: estimate bias array */
	char * use_bias = getenv("USE_BIAS");
	float * bias = NULL;
	if (use_bias!=NULL){
	  bias = iftAllocFloatArray(kernelbank->ncols);        
	  for (int col=0; col < kernelbank->ncols; col++){ 
	    int row = 0; 
	    for (int adj=0; adj < A->n; adj++){ 
	      for (int ch=0; ch < ninput_channels; ch++){ 
		iftMatrixElem(kernelbank,col,row) = 
		  iftMatrixElem(kernelbank,col,row) / stdev[ch]; 
		bias[col] -= (mean[ch]*iftMatrixElem(kernelbank,col,row)); 
		row++; 
	      } 
	    } 
	  }
	  
	  sprintf(filename, "%s/conv%d", param_dir, l+1);
	  WriteBias(filename, bias, kernelbank->ncols); 
	  iftFree(bias); 
	}

	// updating number of kernels on the architecture
        arch->layer[l].noutput_channels = kernelbank->ncols;
        sprintf(filename, "%s/conv%d-kernels.npy", param_dir, l+1);
        iftWriteMatrix(kernelbank, filename);
        iftDestroyMatrix(&kernelbank);
        sprintf(filename, "%s/conv%d", param_dir, l+1);
        WriteMeanStdev(filename, mean, stdev, ninput_channels);
        // making a copy of mean and stdev for the python implementation
        sprintf(filename, "%s/m-norm%d", param_dir, l+1);
        WriteMeanStdev(filename, mean, stdev, ninput_channels);
        iftFree(mean);
        iftFree(stdev);
    	iftDestroyAdjRel(&A);

        /* Apply convolutional layer using the consensus kernel bank and the statistics from all markers */
        int pool_stride            = arch->layer[l].pool_stride;
        arch->layer[l].pool_stride = 1;
        for (int i = 0; i < nimages; i++) {
            basename = iftFilename(fs_seeds->files[i]->path, "-seeds.txt");
            sprintf(filename, "tmp/%s.mimg", basename);
            input = iftReadMImage(filename);
            iftFree(basename);
            output = FLIMConvolutionalLayer(&input, 1, NULL, arch, l, l+1, atrous_factor, param_dir);
            iftDestroyMImage(&input);
	    
            iftWriteMImage(output[0], filename);
            iftDestroyMImage(&output[0]);
            iftFree(output);
        }
        arch->layer[l].pool_stride = pool_stride;
    }
    
    iftRemoveDir("tmp");
    iftDestroyFileSet(&fs_seeds);
    iftDestroyFileSet(&fs_orig);
}

void iftFLIMLearnModelPCA(char *orig_dir, char *markers_dir, char *param_dir, iftFLIMArch *arch) {

    /* Set input parameters */

    iftMakeDir("tmp");

    iftFileSet *fs_orig  = iftLoadFileSetFromDirOrCSV(orig_dir, 1, 1);
    iftFileSet *fs_seeds = iftLoadFileSetFromDirBySuffix(markers_dir, "-seeds.txt", 1);
    iftAdjRel  *A        = NULL;
    iftMImage **output   = NULL;
    iftMImage *input     = NULL;
    int ninput_channels  = 0;
    int nimages          = fs_seeds->n;
    int atrous_factor    = 1;
    char *basename       = NULL;
    char filename[200], ext[10];
    
    sprintf(ext, "%s", iftFileExt(fs_orig->files[0]->path));
    printf("%s\n",ext);
    
    /* Generate input layer */
    for (int i = 0; i < nimages; i++) {
        basename = iftFilename(fs_seeds->files[i]->path, "-seeds.txt");
        sprintf(filename, "%s/%s%s", orig_dir, basename, ext);
        if (strcmp(ext,".mimg")==0){
            input = iftReadMImage(filename);
        } else {
            input = ReadInputMImage(filename);
        }	
        sprintf(filename, "tmp/%s.mimg", basename);
        iftWriteMImage(input, filename);
        iftFree(basename);
        iftDestroyMImage(&input);
    }

    /* For each layer do */

    for (int l = 0; l < arch->nlayers; l++) {
        basename        = iftFilename(fs_seeds->files[0]->path, "-seeds.txt");
        sprintf(filename, "tmp/%s.mimg", basename);
        input           = iftReadMImage(filename);
        if (l == 0) {
            A = iftFLIMAdjRelFromKernel(arch->layer[l], iftIs3DMImage(input));
        } else {
            if (arch->apply_intrinsic_atrous) {
                atrous_factor *= arch->layer[l - 1].pool_stride;
                printf("Updating atrous factor\n");
                fflush(stdout);
            }
	    A = iftFLIMAdaptiveAdjRelFromKernel(arch->layer[l], atrous_factor, iftIs3DMImage(input));
        }
        ninput_channels = input->m;
        iftDestroyMImage(&input);

        /* Compute mean and standard deviation for marker-based
	   normalization */

        float *mean  = iftAllocFloatArray(ninput_channels);
        float *stdev = iftAllocFloatArray(ninput_channels);	
        StatisticsFromAllSeeds(fs_seeds, "tmp", mean, stdev, arch->stdev_factor);
	/* Create and normalize the patch dataset from all seeds
	   (using all training images) */
	
	iftDataSet *Z = iftDataSetFromAllSeeds(markers_dir,"tmp",A);
	iftNormalizeDataSetByZScoreInPlace(Z, NULL, arch->stdev_factor);

	/* Compute kernelbank for the current layer */

	int number_of_kernels = arch->layer[l].noutput_channels;
	iftMatrix *kernelbank = KernelBankByPCA(Z,&number_of_kernels);
	arch->layer[l].noutput_channels = number_of_kernels;

	printf("\n Layer %d\n", l + 1);
	printf("\n Number of kernels=%d\n",arch->layer[l].noutput_channels);
        fflush(stdout);

	/* BIAS: read bias array */
    char * use_bias = getenv("USE_BIAS");
	float * bias = NULL;
	if (use_bias!=NULL){
        bias = iftAllocFloatArray(kernelbank->ncols);        
        for (int col=0; col < kernelbank->ncols; col++){ 
        int row = 0; 
        for (int adj=0; adj < A->n; adj++){ 
            for (int ch=0; ch < ninput_channels; ch++){ 
            iftMatrixElem(kernelbank,col,row) = 
                    iftMatrixElem(kernelbank,col,row) / stdev[ch]; 
            bias[col] -= (mean[ch]*iftMatrixElem(kernelbank,col,row)); 
            row++; 
            } 
        } 
        } 
	}
	
	sprintf(filename, "%s/conv%d", param_dir, l+1);
        WriteMeanStdev(filename, mean, stdev, ninput_channels);
        iftFree(mean);
        iftFree(stdev);

	/* BIAS: write and free bias array */
    if (use_bias !=NULL){
		WriteBias(filename, bias, kernelbank->ncols); 
        iftFree(bias);
	}
	
	iftDestroyAdjRel(&A);

        sprintf(filename, "%s/conv%d-kernels.npy", param_dir, l+1);
        iftWriteMatrix(kernelbank, filename);
	iftDestroyMatrix(&kernelbank);
	iftDestroyDataSet(&Z);

        int pool_stride            = arch->layer[l].pool_stride;
        arch->layer[l].pool_stride = 1;
        for (int i = 0; i < nimages; i++) {
            basename = iftFilename(fs_seeds->files[i]->path, "-seeds.txt");
            sprintf(filename, "tmp/%s.mimg", basename);
            input = iftReadMImage(filename);
            iftFree(basename);
            output = FLIMConvolutionalLayer(&input, 1, NULL, arch, l, l+1, atrous_factor, param_dir);
            iftDestroyMImage(&input);
	    
            iftWriteMImage(output[0], filename);
            iftDestroyMImage(&output[0]);
            iftFree(output);
        }
        arch->layer[l].pool_stride = pool_stride;
    }
    
    //iftRemoveDir("tmp");
    iftDestroyFileSet(&fs_seeds);
    iftDestroyFileSet(&fs_orig);
}

void iftFLIMLearnLayer(char *activ_dir, char *markers_dir, char *param_dir, int layer_index, iftFLIMArch *arch, char *output_dir)
{
    /* Set input parameters */

    iftMakeDir("tmp");

    iftFileSet *fs_activ = iftLoadFileSetFromDirOrCSV(activ_dir, 1, 1);
    iftFileSet *fs_seeds = iftLoadFileSetFromDirBySuffix(markers_dir, "-seeds.txt", 1);
    iftAdjRel *A         = NULL;
    iftMImage **output   = NULL;
    iftMImage *input     = NULL;
    int ninput_channels  = 0;
    int nimages          = fs_seeds->n;
    int atrous_factor    = 1;
    char *basename = NULL;
    char filename[200], ext[10];
    
    sprintf(ext, "%s", iftFileExt(fs_activ->files[0]->path));

    /* Generate input layer */
    for (int i = 0; i < nimages; i++) {
        basename = iftFilename(fs_seeds->files[i]->path, "-seeds.txt");
        sprintf(filename, "%s/%s%s", activ_dir, basename, ext);
        if (strcmp(ext,".mimg")==0){
            input = iftReadMImage(filename);
        } else {
            input = ReadInputMImage(filename);
        }
        sprintf(filename, "tmp/%s.mimg", basename);
        iftWriteMImage(input, filename);
        iftFree(basename);
        iftDestroyMImage(&input);
    }

    /* For a specific layer do */

    basename = iftFilename(fs_seeds->files[0]->path, "-seeds.txt");
    sprintf(filename, "tmp/%s.mimg", basename);
    input           = iftReadMImage(filename);
    if (layer_index == 1) {
        A = iftFLIMAdjRelFromKernel(arch->layer[0], iftIs3DMImage(input));
    } else {
        // reading atrous factor already updated from previous layers
        sprintf(filename,"%s/intrinsic_atrous.txt",param_dir);
        if (!iftFileExists(filename)){
            FILE *fp = fopen(filename, "w");
            fprintf(fp, "%d", 1);
            fclose(fp);
        }
        char *data = iftReadFileAsString(filename);
        atrous_factor = atoi(data);
        iftFree(data);
        A = iftFLIMAdaptiveAdjRelFromKernel(arch->layer[0], atrous_factor, iftIs3DMImage(input));
    }
    ninput_channels = input->m;
    iftDestroyMImage(&input);

    /* Learn and save marker-based normalization parameters and kernels from each training image */

    float *mean  = iftAllocFloatArray(ninput_channels);
    float *stdev = iftAllocFloatArray(ninput_channels);
    StatisticsFromAllSeeds(fs_seeds, "tmp", mean, stdev, arch->stdev_factor);
    
    printf("\nLayer %d\n", layer_index);
    fflush(stdout);

    for (int i = 0; i < nimages; i++) {

        basename = iftFilename(fs_seeds->files[i]->path, "-seeds.txt");
        sprintf(filename, "tmp/%s.mimg", basename);
        input = iftReadMImage(filename);
	
        printf("Processing file %s: %d of %d files\r", basename, i + 1, nimages);
        fflush(stdout);

        /* Learn mean and standard deviation */

        iftLabeledSet *S = iftMReadSeeds(input, fs_seeds->files[i]->path);
        iftLabeledSet *M = LabelMarkers(input, S);
        iftDestroyLabeledSet(&S);

        /* Apply marker-based image normalization */

        NormalizeImageByZScore(input, mean, stdev);
        sprintf(filename, "tmp/%s-norm.mimg", basename);
        iftWriteMImage(input,filename);

        /* Learn and save kernel bank */

        int nsamples          = iftLabeledSetSize(M);
        iftMatrix *kernelbank = LearnKernelBank(input, M, A, nsamples, arch->layer[0].nkernels_per_image, arch->layer[0].nkernels_per_marker);
	
	iftDestroyMImage(&input);
        iftDestroyLabeledSet(&M);
        sprintf(filename, "tmp/%s-kernels.npy", basename);
        iftWriteMatrix(kernelbank, filename);
	iftDestroyMatrix(&kernelbank);
        iftFree(basename);
    }

    /* Create a consensus layer (i.e., merge kernels) and save final kernel bank for layer l */

    iftMatrix *kernelbank = ConsensusKernelbank(fs_seeds, "tmp", arch->layer[0].noutput_channels, arch->stdev_factor);


    /* BIAS: read bias array */
    char * use_bias = getenv("USE_BIAS");
	float * bias = NULL;
	if (use_bias!=NULL){
        bias = iftAllocFloatArray(kernelbank->ncols);        
        for (int col=0; col < kernelbank->ncols; col++){ 
        int row = 0; 
        for (int adj=0; adj < A->n; adj++){ 
            for (int ch=0; ch < ninput_channels; ch++){ 
            iftMatrixElem(kernelbank,col,row) = 
                    iftMatrixElem(kernelbank,col,row) / stdev[ch]; 
            bias[col] -= (mean[ch]*iftMatrixElem(kernelbank,col,row)); 
            row++; 
            } 
        } 
        }
        sprintf(filename, "%s/conv%d", param_dir, layer_index);
        WriteBias(filename, bias, kernelbank->ncols); 
        iftFree(bias);
	}



    sprintf(filename, "%s/conv%d-kernels.npy", param_dir, layer_index);
    iftWriteMatrix(kernelbank, filename);
    iftDestroyMatrix(&kernelbank);
    sprintf(filename, "%s/conv%d", param_dir, layer_index);
    WriteMeanStdev(filename, mean, stdev, ninput_channels);
    // making a copy of mean and stdev for the python implementation
    sprintf(filename, "%s/m-norm%d", param_dir, layer_index);
    WriteMeanStdev(filename, mean, stdev, ninput_channels);
    iftFree(mean);
    iftFree(stdev);
    iftDestroyAdjRel(&A);

    /* Apply convolutional layer using the consensus kernel bank and the statistics from all markers */
    int pool_stride = arch->layer[0].pool_stride;
    arch->layer[0].pool_stride = 1;
    for (int i = 0; i < nimages; i++) {
        basename = iftFilename(fs_seeds->files[i]->path, "-seeds.txt");
        sprintf(filename, "tmp/%s.mimg", basename);
        input = iftReadMImage(filename);

        output = FLIMConvolutionalLayer(&input, 1, NULL, arch, 0, layer_index, atrous_factor, param_dir);
        iftDestroyMImage(&input);

        sprintf(filename, "%s/%s.mimg", output_dir, basename);
        iftWriteMImage(output[0], filename);
        iftDestroyMImage(&output[0]);
        iftFree(output);
        iftFree(basename);
    }
    arch->layer[0].pool_stride = pool_stride;

    // updating atrous factor with pooling of current layer
    if (arch->apply_intrinsic_atrous) {
        atrous_factor *= arch->layer[0].pool_stride;
        printf("Updating atrous factor for next layer\n");
        fflush(stdout);
    }
    // writing atrous factor on file
    sprintf(filename,"%s/intrinsic_atrous.txt",param_dir);
    FILE *fp = fopen(filename, "w");
    fprintf(fp, "%d", atrous_factor);
    fclose(fp);
    
    iftRemoveDir("tmp");
    iftDestroyFileSet(&fs_seeds);
    iftDestroyFileSet(&fs_activ);
}

void iftFLIMExtractFeatures(char *orig_dir, char *image_list, iftFLIMArch *arch, char *param_dir,
                            char *feat_dir, char *object_dir, int device) {
    iftFileSet *fs_images = iftLoadFileSetFromDirOrCSV(image_list, 1, 1);
    int nimages = fs_images->n;
    int batchsize = 0;
    int nbatches = 0;
    int nremaining_images = nimages;
    iftMImage *input = NULL;
    char filename[200], ext[10];
    bool batch_process = true;
    int ninput_channels = 0;
    int input_image_size = 0;
    sprintf(ext, "%s", iftFileExt(fs_images->files[0]->path));

    /* Verify if all images have the same dimension for batch processing */

    sprintf(filename, "%s/%s", orig_dir, fs_images->files[0]->path);
    if (strcmp(ext,".mimg")==0){
      input = iftReadMImage(filename);
    } else {
      input = ReadInputMImage(filename);
    }	
    int xsize = input->xsize, ysize = input->ysize, zsize = input->zsize;
    ninput_channels = input->m;
    input_image_size = input->n;
    iftDestroyMImage(&input);
    for (int i = 1; i < nimages; i++) {
        sprintf(filename, "%s/%s", orig_dir, fs_images->files[i]->path);
        if (strcmp(ext,".mimg")==0){
            input = iftReadMImage(filename);
        } else {
            input = ReadInputMImage(filename);
        }	
        if ((input->xsize != xsize) ||
            (input->ysize != ysize) ||
            (input->zsize != zsize)) {
            batch_process = false;
            iftDestroyMImage(&input);
            break;
        }
    }

    /* Select batch size for GPU/CPU processing */

  if (batch_process) {  /* process in batch */
#ifdef IFT_GPU
    int ndevices = iftNumberOfDevices();
    if (ndevices > 0 && iftStartDevice(device)){
      batchsize         = iftFLIMBatchSizeGPU(arch,input_image_size,ninput_channels,device);
      batchsize         = iftMin(batchsize,nimages);
      nbatches          = nimages/batchsize;
      nremaining_images = nremaining_images - nbatches*batchsize;
      for (int batch=0; batch < nbatches; batch++){
	int first = batch*batchsize, last  = first + batchsize - 1;
	printf("Processing batch %d of %d batches\n", batch+1, nbatches);
	fflush(stdout);
	FLIMExtractFeaturesPerBatch(fs_images, first, last, orig_dir, arch, param_dir, feat_dir, object_dir);
      }
      int first = nimages - nremaining_images, last  = nimages - 1;
      if (first <= last){
	printf("Processing remaining %d images\n", last-first+1);
	fflush(stdout);
	FLIMExtractFeaturesPerBatch(fs_images, first, last, orig_dir, arch, param_dir, feat_dir, object_dir);
      }
      iftStopDevice(device);
    } else {
      int first = 0, last  = nimages - 1;
      FLIMExtractFeaturesPerImage(fs_images, first, last, orig_dir, arch, param_dir, feat_dir, object_dir);
    }
#else
    batchsize = iftFLIMBatchSizeCPU(arch, input_image_size, ninput_channels);
    batchsize = iftMin(batchsize, nimages);
    nbatches = nimages / batchsize;
    nremaining_images = nremaining_images - nbatches * batchsize;
    for (int batch = 0; batch < nbatches; batch++) {
      int first = batch * batchsize, last = first + batchsize - 1;
      printf("Processing batch %d of %d batches\n", batch + 1, nbatches);
      fflush(stdout);
      FLIMExtractFeaturesPerBatch(fs_images, first, last, orig_dir, arch, param_dir, feat_dir, object_dir);
    }
    int first = nimages - nremaining_images, last = nimages - 1;
    if (first <= last) {
      printf("Processing remaining %d images\n", last - first + 1);
      fflush(stdout);
      FLIMExtractFeaturesPerBatch(fs_images, first, last, orig_dir, arch, param_dir, feat_dir, object_dir);
    }
#endif
  } else { /* process per image */
    int first = 0, last = nimages - 1;
    FLIMExtractFeaturesPerImage(fs_images, first, last, orig_dir, arch, param_dir, feat_dir, object_dir);
  }
  
  iftDestroyFileSet(&fs_images);
}

void iftFLIMExtractFeaturesFromLayer(char *orig_dir, char *image_list, iftFLIMArch *arch, char *param_dir, int layer_index,
                                     char *feat_dir, char *object_dir, int device) {
    iftFileSet *fs_images = iftLoadFileSetFromDirOrCSV(image_list, 1, 1);
    int nimages = fs_images->n;
    int batchsize = 0;
    int nbatches = 0;
    int nremaining_images = nimages;
    iftMImage *input = NULL;
    char filename[200], ext[10];
    bool batch_process = true;
    int ninput_channels = 0;
    int input_image_size = 0;
    
    sprintf(ext, "%s", iftFileExt(fs_images->files[0]->path));

    /* Verify if all images have the same dimension for batch processing */

    sprintf(filename, "%s/%s", orig_dir, fs_images->files[0]->path);
    if (strcmp(ext,".mimg")==0){
        input = iftReadMImage(filename);
    }else{
        input = ReadInputMImage(filename);
    }
    int xsize = input->xsize, ysize = input->ysize, zsize = input->zsize;
    ninput_channels = input->m;
    input_image_size = input->n;
    iftDestroyMImage(&input);
    for (int i = 1; i < nimages; i++) {
        sprintf(filename, "%s/%s", orig_dir, fs_images->files[i]->path);
        if (strcmp(ext,".mimg")==0){
            input = iftReadMImage(filename);
        } else {
            input = ReadInputMImage(filename);
        }
        if ((input->xsize != xsize) ||
            (input->ysize != ysize) ||
            (input->zsize != zsize)) {
            batch_process = false;
            iftDestroyMImage(&input);
            break;
        }
    }

    /* Select batch size for GPU/CPU processing */

    if (batch_process) {  /* process in batch */
#ifdef IFT_GPU
  int ndevices = iftNumberOfDevices();
  if (ndevices > 0 && iftStartDevice(device)){
    batchsize         = iftFLIMBatchSizeGPU(arch,input_image_size,ninput_channels,device);
    batchsize         = iftMin(batchsize,nimages);
    nbatches          = nimages/batchsize;
    nremaining_images = nremaining_images - nbatches*batchsize;
    for (int batch=0; batch < nbatches; batch++){
      int first = batch*batchsize, last  = first + batchsize - 1;
      printf("Processing batch %d of %d batches\n", batch+1, nbatches);
      fflush(stdout);
      FLIMExtractFeaturesFromLayerPerBatch(fs_images, first, last, orig_dir, arch, param_dir, layer_index, feat_dir, object_dir);
    }
    int first = nimages - nremaining_images, last  = nimages - 1;
    if (first <= last){
      printf("Processing remaining %d images\n", last-first+1);
      fflush(stdout);
      FLIMExtractFeaturesFromLayerPerBatch(fs_images, first, last, orig_dir, arch, param_dir, layer_index, feat_dir, object_dir);
    }
    iftStopDevice(device);
      } else {
    int first = 0, last  = nimages - 1;
    FLIMExtractFeaturesFromLayerPerImage(fs_images, first, last, orig_dir, arch, param_dir, layer_index, feat_dir, object_dir);
      }
#else
        batchsize = iftFLIMBatchSizeCPU(arch, input_image_size, ninput_channels);
        batchsize = iftMin(batchsize, nimages);
        nbatches = nimages / batchsize;
        nremaining_images = nremaining_images - nbatches * batchsize;
        for (int batch = 0; batch < nbatches; batch++) {
            int first = batch * batchsize, last = first + batchsize - 1;
            printf("Processing batch %d of %d batches\n", batch + 1, nbatches);
            fflush(stdout);
            FLIMExtractFeaturesFromLayerPerBatch(fs_images, first, last, orig_dir, arch, param_dir, layer_index, feat_dir, object_dir);
        }
        int first = nimages - nremaining_images, last = nimages - 1;
        if (first <= last) {
            printf("Processing remaining %d images\n", last - first + 1);
            fflush(stdout);
            FLIMExtractFeaturesFromLayerPerBatch(fs_images, first, last, orig_dir, arch, param_dir, layer_index, feat_dir, object_dir);
        }
#endif
    } else { /* process per image */
        int first = 0, last = nimages - 1;
        FLIMExtractFeaturesFromLayerPerImage(fs_images, first, last, orig_dir, arch, param_dir, layer_index, feat_dir, object_dir);
    }

    iftDestroyFileSet(&fs_images);
}

int iftFLIMBatchSizeCPU(iftFLIMArch *arch, int input_image_nvoxels, int input_image_nchannels) {
    float freeMemoryMb, percMemory = 0.85;

    freeMemoryMb = (float) iftGetFreePhysicalSystemMemory() / 1024.0 / 1024.0;
    printf("CPU: %.0fGB of free memory.\n", freeMemoryMb / 1024.0);

    float nMbytesPerDouble = sizeof(double) / 1024.0 / 1024.0;
    float ninput_channels = input_image_nchannels;
    float nMbytesInputImage = input_image_nvoxels * ninput_channels * nMbytesPerDouble;
    float max_requiredMemory = 0.0;
    for (int l = 0; l < arch->nlayers; l++) {
        int KS2 = 1;
        if (arch->layer[l].kernel_size[2] != 0)
            KS2 = arch->layer[l].kernel_size[2];
        float kernelsize = arch->layer[l].kernel_size[0] * arch->layer[l].kernel_size[1] * KS2;
        float nMbytesKernels = arch->layer[l].noutput_channels * kernelsize * ninput_channels * nMbytesPerDouble;
        float nMbytesOutputImage = input_image_nvoxels * arch->layer[l].noutput_channels * nMbytesPerDouble;
        float requiredMemory = nMbytesKernels + nMbytesOutputImage + nMbytesInputImage;

        if (requiredMemory > max_requiredMemory) {
            max_requiredMemory = requiredMemory;
        }
        ninput_channels = arch->layer[l].noutput_channels;
        nMbytesInputImage = nMbytesOutputImage / arch->layer[l].pool_stride;
        input_image_nvoxels = input_image_nvoxels / arch->layer[l].pool_stride;
    }

    int batchsize = (int) floor(percMemory * freeMemoryMb / max_requiredMemory);

    return (batchsize);
}


iftMImage *iftFLIMAtrousAveragePooling(iftMImage *mimg, int width, int height, int depth, int atrous_factor, int stride) {

    iftAdjRel *A;

    if (iftIs3DMImage(mimg)) {
        A = iftCuboidWithDilation(width, height, depth, atrous_factor, atrous_factor, atrous_factor);
    } else {
        A = iftRectangularWithDilation(width, height, atrous_factor, atrous_factor);
    }

    iftMImage *pool = iftCreateMImage(mimg->xsize, mimg->ysize, mimg->zsize, mimg->m);

#pragma omp parallel for
    for (int p = 0; p < mimg->n; p++) {
        iftVoxel u = iftMGetVoxelCoord(mimg, p);
        for (int i = 0; i < A->n; i++) {
            iftVoxel v = iftGetAdjacentVoxel(A, u, i);
            if (iftMValidVoxel(mimg, v)) {
                int q = iftMGetVoxelIndex(mimg, v);
                for (int b = 0; b < mimg->m; b++)
                    pool->val[p][b] += mimg->val[q][b];
            }
        }
        for (int b = 0; b < mimg->m; b++)
            pool->val[p][b] /= A->n;
    }

    iftDestroyAdjRel(&A);
    if (stride > 1) {
        iftMImage *mimgRet = iftCreateMImage(ceilf(mimg->xsize / (float) stride), ceilf(mimg->ysize / (float) stride),
                                             iftMax(ceilf(mimg->zsize / (float) stride), 1), mimg->m);
        int q = 0;
        iftVoxel u;
        for (u.z = 0; u.z < pool->zsize; u.z = u.z + stride) {
            for (u.y = 0; u.y < pool->ysize; u.y = u.y + stride) {
                for (u.x = 0; u.x < pool->xsize; u.x = u.x + stride) {
                    int p = iftMGetVoxelIndex(pool, u);
                    for (int b = 0; b < pool->m; b++)
                        mimgRet->val[q][b] = pool->val[p][b];
                    q++;
                }
            }
        }

        iftDestroyMImage(&pool);
        return mimgRet;
    }
    return (pool);
}


/*iftMImage *iftFLIMAveragePooling(iftMImage *mimg, int width, int height, int depth, int stride) {
    iftAdjRel *A;

    if (iftIs3DMImage(mimg)) {
        if ((width == height) && (height == depth))
            A = iftSpheric(width / 2.0);
        else
            A = iftCuboid(width, height, depth);
    } else {
        if (width == height)
            A = iftCircular(width / 2.0);
        else
            A = iftRectangular(width, height);
    }

    iftMImage *pool = iftCreateMImage(mimg->xsize, mimg->ysize, mimg->zsize, mimg->m);

#pragma omp parallel for
    for (int p = 0; p < mimg->n; p++) {
        iftVoxel u = iftMGetVoxelCoord(mimg, p);
        for (int i = 0; i < A->n; i++) {
            iftVoxel v = iftGetAdjacentVoxel(A, u, i);
            if (iftMValidVoxel(mimg, v)) {
                int q = iftMGetVoxelIndex(mimg, v);
                for (int b = 0; b < mimg->m; b++)
                    pool->val[p][b] += mimg->val[q][b];
            }
        }
        for (int b = 0; b < mimg->m; b++)
            pool->val[p][b] /= A->n;
    }

    iftDestroyAdjRel(&A);
    if (stride > 1) {
        iftMImage *mimgRet = iftCreateMImage(ceilf(mimg->xsize / (float) stride), ceilf(mimg->ysize / (float) stride),
                                             iftMax(ceilf(mimg->zsize / (float) stride), 1), mimg->m);
        int q = 0;
        iftVoxel u;
        for (u.z = 0; u.z < pool->zsize; u.z = u.z + stride) {
            for (u.y = 0; u.y < pool->ysize; u.y = u.y + stride) {
                for (u.x = 0; u.x < pool->xsize; u.x = u.x + stride) {
                    int p = iftMGetVoxelIndex(pool, u);
                    for (int b = 0; b < pool->m; b++)
                        mimgRet->val[q][b] = pool->val[p][b];
                    q++;
                }
            }
        }

        iftDestroyMImage(&pool);
        return mimgRet;
    }
    return (pool);
}*/

iftMImage *iftFLIMAtrousMaxPooling(iftMImage *mimg, int width, int height, int depth, int atrous_factor, int stride) {
    iftAdjRel *A;

    if (iftIs3DMImage(mimg)) {
        A = iftCuboidWithDilation(width, height, depth, atrous_factor, atrous_factor, atrous_factor);
    } else {
        A = iftRectangularWithDilation(width, height, atrous_factor, atrous_factor);
    }

    iftMImage *pool = iftCreateMImage(mimg->xsize, mimg->ysize, mimg->zsize, mimg->m);
    iftSetMImage(pool, IFT_INFINITY_FLT_NEG);

#pragma omp parallel for
    for (int p = 0; p < mimg->n; p++) {
        iftVoxel u = iftMGetVoxelCoord(mimg, p);
        for (int i = 0; i < A->n; i++) {
            iftVoxel v = iftGetAdjacentVoxel(A, u, i);
            if (iftMValidVoxel(mimg, v)) {
                int q = iftMGetVoxelIndex(mimg, v);
                for (int b = 0; b < mimg->m; b++)
                    if (mimg->val[q][b] > pool->val[p][b])
                        pool->val[p][b] = mimg->val[q][b];
            }
        }
    }

    iftDestroyAdjRel(&A);

    if (stride > 1) {
        iftMImage *mimgRet = iftCreateMImage(ceilf(mimg->xsize / (float) stride), ceilf(mimg->ysize / (float) stride),
                                             iftMax(ceilf(mimg->zsize / (float) stride), 1), mimg->m);
        int q = 0;
        iftVoxel u;
        for (u.z = 0; u.z < pool->zsize; u.z = u.z + stride) {
            for (u.y = 0; u.y < pool->ysize; u.y = u.y + stride) {
                for (u.x = 0; u.x < pool->xsize; u.x = u.x + stride) {
                    int p = iftMGetVoxelIndex(pool, u);
                    for (int b = 0; b < pool->m; b++)
                        mimgRet->val[q][b] = pool->val[p][b];
                    q++;
                }
            }
        }

        iftDestroyMImage(&pool);

        return mimgRet;
    }

    return pool;
}

/*
iftMImage *iftFLIMMaxPooling(iftMImage *mimg, int width, int height, int depth, int stride) {
    iftAdjRel *A;

    if (iftIs3DMImage(mimg)) {
        if ((width == height) && (height == depth))
            A = iftSpheric(width / 2.0);
        else
            A = iftCuboid(width, height, depth);
    } else {
        if (width == height)
            A = iftCircular(width / 2.0);
        else
            A = iftRectangular(width, height);
    }

    iftMImage *pool = iftCreateMImage(mimg->xsize, mimg->ysize, mimg->zsize, mimg->m);
    iftSetMImage(pool, IFT_INFINITY_FLT_NEG);

#pragma omp parallel for
    for (int p = 0; p < mimg->n; p++) {
        iftVoxel u = iftMGetVoxelCoord(mimg, p);
        for (int i = 0; i < A->n; i++) {
            iftVoxel v = iftGetAdjacentVoxel(A, u, i);
            if (iftMValidVoxel(mimg, v)) {
                int q = iftMGetVoxelIndex(mimg, v);
                for (int b = 0; b < mimg->m; b++)
                    if (mimg->val[q][b] > pool->val[p][b])
                        pool->val[p][b] = mimg->val[q][b];
            }
        }
    }

    iftDestroyAdjRel(&A);

    if (stride > 1) {
        iftMImage *mimgRet = iftCreateMImage(ceilf(mimg->xsize / (float) stride), ceilf(mimg->ysize / (float) stride),
                                             iftMax(ceilf(mimg->zsize / (float) stride), 1), mimg->m);
        int q = 0;
        iftVoxel u;
        for (u.z = 0; u.z < pool->zsize; u.z = u.z + stride) {
            for (u.y = 0; u.y < pool->ysize; u.y = u.y + stride) {
                for (u.x = 0; u.x < pool->xsize; u.x = u.x + stride) {
                    int p = iftMGetVoxelIndex(pool, u);
                    for (int b = 0; b < pool->m; b++)
                        mimgRet->val[q][b] = pool->val[p][b];
                    q++;
                }
            }
        }

        iftDestroyMImage(&pool);

        return mimgRet;
    }

    return pool;
}*/

iftMatrix *iftFLIMSelectKernelsManual(char *kernel_bank_path, char *selected_kernels_path)
{
    iftMatrix *input_kernels   = iftReadMatrix(kernel_bank_path);
    iftDict    *json           = iftReadJson(selected_kernels_path);
    iftIntArray *selKernels    = iftGetIntArrayFromDict("selected_kernels", json);
    int        nKernels        = selKernels->n;
    iftMatrix *output_kernels  = iftCreateMatrix(nKernels,input_kernels->nrows);
    char *use_bias             = getenv("USE_BIAS");

    if (use_bias!=NULL){
      float *bias      = NULL;
      char *model_path = iftDirname(kernel_bank_path);
      char *prefix     = iftFilename(kernel_bank_path, "-kernels.npy");
      char basename[300];
      sprintf(basename,"%s/%s",model_path,prefix);
      printf("%s \n",basename);
      bias             = ReadBias(basename);
      float *newbias   = iftAllocFloatArray(nKernels);
      /* perform kernel and bias selection */
      for(int c = 0; c < nKernels; c++) {
        int k      = selKernels->val[c];
	printf("k %d\n",k);
	newbias[c] = bias[abs(k)];
        for (int r=0; r < input_kernels->nrows; r++){
	  iftMatrixElem(output_kernels, c, r) = iftSign(k)*iftMatrixElem(input_kernels, abs(k), r);
        }
      }
      WriteBias(basename, newbias, nKernels); 
      iftFree(bias);
      iftFree(newbias);
      iftFree(model_path);
      iftFree(prefix);
    } else {
      /* perform kernel selection */
      for(int c = 0; c < nKernels; c++) {
        int k = selKernels->val[c];
        for (int r=0; r < input_kernels->nrows; r++){
	  iftMatrixElem(output_kernels, c, r) = iftSign(k)*iftMatrixElem(input_kernels, abs(k), r);
        }
      }
    }
    
    iftDestroyMatrix(&input_kernels);
    return output_kernels;
}

void iftFLIMConvertModel2BIAS(iftFLIMArch *arch, int ninput_channels, char * param_dir, char * output_dir){
    
    iftMakeDir(output_dir);
    
    for(int l=0; l<arch->nlayers; l++){
        int layer_index = l+1;

        //read kernel, mean and std
        char filename[200];
        sprintf(filename, "%s/conv%d-kernels.npy", param_dir, layer_index);
        iftMatrix *kernelbank = iftReadMatrix(filename);
        float *mean  = iftAllocFloatArray(ninput_channels);
        float *stdev = iftAllocFloatArray(ninput_channels);
        sprintf(filename, "%s/conv%d", param_dir, layer_index);
        ReadMeanStdev(filename, mean, stdev, ninput_channels);

        //convert parameters
        float *bias = iftAllocFloatArray(kernelbank->ncols);        
        for (int col=0; col < kernelbank->ncols; col++){ 
            int row = 0; 
            int adj_size = kernelbank->nrows/ninput_channels;
            for (int adj=0; adj < adj_size; adj++){ 
                for (int ch=0; ch < ninput_channels; ch++){ 
                iftMatrixElem(kernelbank,col,row) = 
                        iftMatrixElem(kernelbank,col,row) / stdev[ch]; 
                bias[col] -= (mean[ch]*iftMatrixElem(kernelbank,col,row)); 
                row++; 
                } 
            } 
        }

        //save parameters to dir
        sprintf(filename, "%s/conv%d", output_dir, l+1);
        WriteBias(filename, bias, kernelbank->ncols);

        sprintf(filename, "%s/conv%d-kernels.npy", output_dir, l+1);
        iftWriteMatrix(kernelbank, filename);

        ninput_channels = kernelbank->ncols;

        iftFree(bias);
        iftFree(mean);
        iftFree(stdev);
        iftDestroyMatrix(&kernelbank);
        
    }

}
