#include "ift.h"

//Features (3 or 1): (normalized) mean L, mean a, mean b or mean value for grayscale images
iftDataSet* iftSupervoxelsToLabColorMeanDataset(iftImage* image, iftImage* label_image){

    iftDataSet* dataset;
    int normalization_value = iftNormalizationValue(iftMaximumValue(image));
    
    float l_norm, a_norm, b_norm;
    iftVerifyImageDomains(image, label_image,"iftSupervoxelsToLabColorMeanDataset"); 

    int nregions = iftMaximumValue(label_image);
    int *size = iftAllocIntArray(nregions);
    
    if(!iftIsColorImage(image))
      dataset = iftCreateDataSet(nregions, 1);
    else
      dataset = iftCreateDataSet(nregions, 3);

    int p;
    for(p = 0; p < image->n; p++){
        int r = label_image->val[p] - 1;
        if(r < 0)
            continue;

        size[r]++;

        if(!iftIsColorImage(image)) {
          dataset->sample[r].feat[0] += ((float)image->val[p] / (float)normalization_value);
        } else {
          iftColor color;
          color.val[0] = image->val[p];
          color.val[1] = image->Cb[p];
          color.val[2] = image->Cr[p];

          color = iftYCbCrtoRGB(color,normalization_value);
          iftFColor fcolor = iftRGBtoLab(color,normalization_value);

          // L in [0, 99.998337]
          // a in [-86.182236, 98.258614]
          // b in [-107.867744, 94.481682]
          l_norm = (fcolor.val[0] - 0.)/(99.998337 - 0.);
          a_norm = (fcolor.val[1] + 86.182236)/(98.258614 + 86.182236);
          b_norm = (fcolor.val[2] + 107.867744)/(94.481682 + 107.867744);

          dataset->sample[r].feat[0] += l_norm;
          dataset->sample[r].feat[1] += a_norm;
          dataset->sample[r].feat[2] += b_norm;
      }
    }

    int r;
    for(r = 0; r < nregions; r++){
        if(size[r] == 0)
            iftError("Empty region","iftSupervoxelsToLabColorMeanDataset");

        dataset->sample[r].id = r+1;
        //Means
        if(!iftIsColorImage(image)) {
          dataset->sample[r].feat[0] /= size[r];
        } else {
          dataset->sample[r].feat[0] /= size[r];
          dataset->sample[r].feat[1] /= size[r];
          dataset->sample[r].feat[2] /= size[r];
        }
    }
    iftSetDistanceFunction(dataset, 1);
    dataset->ref_data      = (void*)label_image;
    dataset->ref_data_type = IFT_REF_DATA_IMAGE;

    free(size);
    return dataset;
}

// Get number of pixel inside the superpixels (for contextual descriptor computation)
iftIntArray *iftGetNumberOfPixelsInsideSuperpixels(iftImage *label) {
  iftIntArray *npixels_by_sup;
  int nsuperpixels;
  nsuperpixels = iftMaximumValue(label);
  npixels_by_sup = iftCreateIntArray(nsuperpixels);

  for (int p = 0; p < label->n; ++p) {
    int index_sup = label->val[p] - 1;
    npixels_by_sup->val[index_sup] += 1;
  }
  return npixels_by_sup;
}

iftDataSet *iftContextualDescriptorWith3levels(iftDataSet *input_dataset, iftImage *label) {
  iftDataSet *dataset;
  iftSet *adj_set;
  iftRegionGraph *region_graph;
  iftAdjRel *A;
  iftIntArray *queue, *dist, *npixels_by_sup, *total_npixels_by_level;
  int nsamples, back_queue, front_queue, nfeats_input, max_level, nblocks;
  nsamples = input_dataset->nsamples;
  nfeats_input = input_dataset->nfeats;
  max_level = 3;
  nblocks = 3;

  A = iftCircular(1.0);
  region_graph = iftRegionGraphFromLabelImage(label, input_dataset, A);
  dataset = iftCreateDataSet(nsamples, nblocks*nfeats_input);
  queue = iftCreateIntArray(nsamples);
  dist = iftCreateIntArray(nsamples);
  npixels_by_sup = iftGetNumberOfPixelsInsideSuperpixels(label);

  // clean queue
  for (int i = 0; i < nsamples; ++i) {
    queue->val[i] = -1;
    dist->val[i] = IFT_INFINITY_INT;
  }
  
  for (int s = 0; s < nsamples; ++s) {
    total_npixels_by_level = iftCreateIntArray(nblocks);

    dataset->sample[s].id = input_dataset->sample[s].id;
    // copy data from sample s (contextual level = 0)
    for (int j = 0; j < nfeats_input; ++j) 
      dataset->sample[s].feat[j] = input_dataset->sample[s].feat[j];

    front_queue = 0;
    back_queue = 0;
    queue->val[back_queue] = s;
    dist->val[s] = 0;
    back_queue++;
    while (front_queue < back_queue) {
      int u = queue->val[front_queue];
      front_queue++;
      // iterate over neighbours
      adj_set = region_graph->node[u].adjacent;
      while(adj_set){
        int v = adj_set->elem; // neighbour index
        if (dist->val[v] != IFT_INFINITY_INT) {
          adj_set = adj_set->next;
          continue;
        }
        dist->val[v] = dist->val[u] + 1;

        int index_block = dist->val[v];
        if (dist->val[v] == max_level) // special case
          index_block = max_level - 1;
        // accumulate weighted values in feature vector
        for (int j = 0; j < nfeats_input; ++j)
          dataset->sample[s].feat[index_block*nfeats_input  + j] += (input_dataset->sample[v].feat[j] * (float)npixels_by_sup->val[v]);

        total_npixels_by_level->val[index_block] += npixels_by_sup->val[v];

        if (dist->val[v] < max_level) {
          queue->val[back_queue] = v;
          back_queue++;
        }
        adj_set = adj_set->next;
      }
      iftDestroySet(&adj_set);
    }
    // Normalize
    for (int b = 1; b < nblocks; ++b)
      for (int j = 0; j < nfeats_input; ++j) 
        dataset->sample[s].feat[b*nfeats_input + j] /= (float)total_npixels_by_level->val[b];
    // clear queue
    for (int i = 0; i < back_queue; ++i) 
      queue->val[i] = -1;
    for (int i = 0; i < nsamples; ++i)
      dist->val[i] = IFT_INFINITY_INT;

    iftDestroyIntArray(&total_npixels_by_level);
  }
  
  // Free
  iftDestroyIntArray(&queue);
  iftDestroyIntArray(&dist);
  iftDestroyIntArray(&npixels_by_sup);
  iftDestroyRegionGraph(&region_graph);
  iftDestroyAdjRel(&A);
  
  iftSetDistanceFunction(dataset, 1);
  dataset->ref_data      = input_dataset->ref_data;
  dataset->ref_data_type = input_dataset->ref_data_type;

  return dataset;
}

void iftRelabelObjectLabelsForBinaryClassification(iftImage *gt_img) {
  // Fix gt (for binary classification)
  int max_val = iftMaximumValue(gt_img);
  if (max_val == 255 || max_val == 1) {
    for (int p = 0; p < gt_img->n; ++p) {
      if (gt_img->val[p] == 0)
        gt_img->val[p] = 1;
      else
        gt_img->val[p] = 2;
    }
  }
}

void iftSetSupervoxelObjectLabels(iftImage *label, iftImage *gt_img, iftDataSet *dataset) {
  iftMatrix *mat_sup_obj;
  int nsups, nobjs;
  // Assign object labels to supervoxels
  nsups = iftMaximumValue(label);
  nobjs = iftMaximumValue(gt_img);
  dataset->nclasses = nobjs;
  mat_sup_obj = iftCreateMatrix(nobjs, nsups);
  for (int p = 0; p < label->n; ++p) {
    int gt_val = gt_img->val[p];
    int label_val = label->val[p];
    mat_sup_obj->val[iftGetMatrixIndex(mat_sup_obj, gt_val-1, label_val-1)] += 1;
  }
  for (int s = 0; s < nsups; ++s) {
    int best_obj = -1;
    int best_npix = 0;
    for (int j = 0; j < nobjs; ++j) {
      if (mat_sup_obj->val[iftGetMatrixIndex(mat_sup_obj, j, s)] > best_npix) {
        best_npix = mat_sup_obj->val[iftGetMatrixIndex(mat_sup_obj, j, s)];
        best_obj = j;
      }
    }
    dataset->sample[s].truelabel = best_obj + 1;
  }
  iftDestroyMatrix(&mat_sup_obj);
}

int main(int argc, char *argv[]) 
{
  iftImage  *img, *label, *gt_img;
  iftDataSet *dataset; 
  int descriptor;
  char filename[256];
  int nimages;
  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/
  
  if (argc!=6)
    iftError("Usage: iftSupervoxelDataSetFromDir <image_dir> <label_dir> <gt_image_dir> <descriptor (0:MC, 1:BIC, 2:MC-CONTEXTUAL, 3:BIC-CONTEXTUAL)> <output_dataset_dir>","main");
  
  iftDir* imagefiles = iftLoadFilesFromDirBySuffix(argv[1], "ppm");
  nimages = imagefiles->nfiles;
  iftDir* labelfiles = iftLoadFilesFromDirBySuffix(argv[2], "pgm");
  iftDir* gtfiles = iftLoadFilesFromDirBySuffix(argv[3], "pgm");
  descriptor = atoi(argv[4]);

  for (int i = 0; i < nimages; i++) {
    img  = iftReadImageByExt(imagefiles->files[i]->path);
    label  = iftReadImageByExt(labelfiles->files[i]->path);
    gt_img  = iftReadImageByExt(gtfiles->files[i]->path);

    if (descriptor == 0) {
      dataset = iftSupervoxelsToLabColorMeanDataset(img, label);
    } else if (descriptor == 1) {
      dataset = iftSupervoxelsToBICDataset(img, label, 8, 0, NULL);
    } else if (descriptor == 2) {
      iftDataSet *mc_dataset = iftSupervoxelsToLabColorMeanDataset(img, label);
      dataset = iftContextualDescriptorWith3levels(mc_dataset, label);
      iftDestroyDataSet(&mc_dataset);
    } else {
      iftDataSet *bic_dataset = iftSupervoxelsToBICDataset(img, label, 8, 0, NULL);
      dataset = iftContextualDescriptorWith3levels(bic_dataset, label);
      iftDestroyDataSet(&bic_dataset);
    }
    
    printf("Num. samples %d, Num. features %d\n", dataset->nsamples, dataset->nfeats);
    
    iftRelabelObjectLabelsForBinaryClassification(gt_img); // relabel 0 -> 1 and 1 or 255 -> 2
    iftSetSupervoxelObjectLabels(label, gt_img, dataset);

    sprintf(filename, "%s/%s.zip", argv[5] ,iftFilename(imagefiles->files[i]->path, ".ppm"));
    printf("%s\n", filename);
    // write dataset
    iftWriteOPFDataSet(dataset, filename);
    
    iftDestroyImage(&img);
    iftDestroyImage(&gt_img);
    iftDestroyImage(&label);
    iftDestroyDataSet(&dataset);
  }

  iftDestroyDir(&imagefiles);
  iftDestroyDir(&labelfiles);
  iftDestroyDir(&gtfiles);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
     MemDinInicial,MemDinFinal);   

  return(0);
}
