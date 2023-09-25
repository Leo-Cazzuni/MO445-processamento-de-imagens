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

// Obtain classification map from dataset (including train and test samples)
iftImage* iftGetClassificationMap(iftImage *label, iftDataSet *dataset) {
  iftImage *class_map;
  class_map = iftCreateImage(label->xsize, label->ysize, label->zsize);
  for (int p = 0; p < label->n; ++p) {
    int index_sup = label->val[p] - 1;
    class_map->val[p] = dataset->sample[index_sup].label;
  }
  return class_map;
}

// Compute accuracy from classification map
float iftComputeAccuracyFromClassificationMap(iftImage *class_map, iftImage *gt_img) {
  float accuracy = 0;
  int ncorrect = 0;
  assert(class_map->n == gt_img->n);
  for (int p = 0; p < class_map->n; ++p)
    if (class_map->val[p] == gt_img->val[p])
      ncorrect++;
  accuracy = (float) ncorrect / (float) class_map->n;
  return accuracy;
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

void iftRelabelBinaryClassificationMapToZeroOneValues(iftImage *class_map) {
  int max_val = iftMaximumValue(class_map);
  if (max_val == 2) {
    for (int p = 0; p < class_map->n; ++p) {
      if (class_map->val[p] == 1)
        class_map->val[p] = 0;
      else
        class_map->val[p] = 1;
    }
  }
}

// Aassume label values start from 1
iftImage *iftExtractLargerConnectedComponentFromBinaryImage(iftImage *label) {
  printf("Extract larger component\n");
  int *area, *component_type, nregions, index_maxarea, max_area;
  iftImage *relabel, *result;
  iftAdjRel *adj = iftCircular(1.0);
  printf("label max %d min %d\n", iftMaximumValue(label), iftMinimumValue(label));
  relabel = iftRelabelRegions(label, adj);
  nregions = iftMaximumValue(relabel);
  printf("nregions %d min %d\n", nregions, iftMinimumValue(relabel));
  area = iftAllocIntArray(nregions);
  component_type = iftAllocIntArray(nregions);
  for (int p = 0; p < label->n; ++p) {
    int index_region = relabel->val[p] - 1;
    area[index_region]++;    
    component_type[index_region] = label->val[p];
  }
  index_maxarea = -1;
  max_area = 0;
  for (int i = 0; i < nregions; ++i) {
    if(component_type[i] == 2 && area[i] > max_area) {
      index_maxarea = i;
      max_area = area[i];
    }
  }

  result = iftCreateImage(label->xsize, label->ysize, label->zsize);
  for (int p = 0; p < label->n; ++p) {
    int index_region = relabel->val[p] - 1;
    if (index_region == index_maxarea) {
      result->val[p] = 1;
    }
  }
  printf("best index_region %d\n", index_maxarea);
  iftDestroyImage(&relabel);
  iftDestroyAdjRel(&adj);
  free(area);
  free(component_type);
  printf("extraction ok\n");
  return result;
}

iftDataSet *iftExtractDescriptorFromSupervoxelDataSet(iftImage *img, iftImage *label, int descriptor) {
  iftDataSet *dataset;
  // Extract descriptor dataset
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
  return dataset;
}

int main(int argc, char *argv[]) 
{
  iftImage  *gt_img, *label, *class_map, *orig_class_map, *img;
  iftDataSet *dataset, *train_dataset; 
  iftCplGraph *graph = NULL;
  float type_metric;
  int nimages, descriptor;
  char filename_classmap[256];
  FILE *f1;
  /*--------------------------------------------------------*/

  void *trash = malloc(1);
  struct mallinfo info;
  int MemDinInicial, MemDinFinal;
  free(trash);
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/
  if (argc != 9)
    iftError("Usage: iftClassifySupervoxelsUsingPreTrainedClassifier <image_dir> <label_dir> <descriptor (0:MC, 1:BIC, 2:MC-CONTEXTUAL, 3:BIC-CONTEXTUAL)> <input_classifier> <output_dir> <gt_image_dir> <output_csv> <metric (0: ACC or Fscore beta value)>","main");

  iftDir* imagefiles = iftLoadFilesFromDirBySuffix(argv[1], "ppm");
  iftDir* labelfiles = iftLoadFilesFromDirBySuffix(argv[2], "pgm");
  descriptor = atoi(argv[3]);
  graph = iftReadCplGraph(argv[4]);
  train_dataset = graph->Z;

  for (int j = 0; j < train_dataset->nfeats; ++j) {
    printf("%f\n", train_dataset->sample[3].feat[j]);
  }

  printf("read ok\n");

  iftDir* gtfiles = iftLoadFilesFromDirBySuffix(argv[6], "pgm");
  type_metric = atof(argv[8]);
  nimages = gtfiles->nfiles;
  f1 = fopen(argv[7], "w");
  float total_mean_acc = 0;
  for (int i = 0; i < nimages; i++) {
    img  = iftReadImageByExt(imagefiles->files[i]->path);
    gt_img  = iftReadImageByExt(gtfiles->files[i]->path);
    label  = iftReadImageByExt(labelfiles->files[i]->path);
    printf("%s\n", imagefiles->files[i]->path);
    
    dataset = iftExtractDescriptorFromSupervoxelDataSet(img, label, descriptor);    
    iftRelabelObjectLabelsForBinaryClassification(gt_img);

    // classify images
    float acc = 0;
    // Classify samples
    iftSetStatus(dataset, IFT_TEST);
    iftClassify(graph, dataset);  // Testing
    // compute overall accuracy
    orig_class_map = iftGetClassificationMap(label, dataset);   
    printf("Maximum value classification orig_map %d\n", iftMaximumValue(orig_class_map));
    if (iftMaximumValue(orig_class_map) == 0) {
      class_map = orig_class_map;
      orig_class_map = NULL;
    } else {
      class_map = iftExtractLargerConnectedComponentFromBinaryImage(orig_class_map);
      iftDestroyImage(&orig_class_map);
      printf("extract largtest component\n");
    }

    iftRelabelBinaryClassificationMapToZeroOneValues(gt_img);
    if (type_metric == 0) {
      acc = iftComputeAccuracyFromClassificationMap(class_map, gt_img);
    } else {
      acc = iftFBetaScore(class_map, gt_img, type_metric); // beta = type_metric
    }
    // write classification map
    sprintf(filename_classmap, "%s/%s.pgm", argv[5] ,iftFilename(gtfiles->files[i]->path, ".pgm"));
    printf("%s\n", filename_classmap);
    iftWriteImageP2(class_map, filename_classmap);
    iftDestroyImage(&class_map);
    
    total_mean_acc += acc;
    printf("Accuracy of classification map %f\n", acc);

    fprintf(f1,"%f\n", acc);

    // Free
    iftDestroyImage(&img);
    iftDestroyImage(&gt_img);
    iftDestroyImage(&label);
    iftDestroyDataSet(&dataset);
  }
  total_mean_acc /= (float) nimages;
  fprintf(f1,"%f\n", total_mean_acc);  

  printf("Mean metric value %f\n", total_mean_acc);

  fclose(f1);

  iftDestroyDir(&imagefiles);
  iftDestroyDir(&gtfiles);
  iftDestroyDir(&labelfiles);
  iftDestroyCplGraph(&graph);
  iftDestroyDataSet(&train_dataset);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
     MemDinInicial,MemDinFinal);   

  return(0);
}
