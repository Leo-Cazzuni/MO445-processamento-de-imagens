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
            iftError("Empty region", "iftSupervoxelsToLabColorMeanDataset");

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

// Select superpixels for training
iftIntArray *iftSelectSupervoxelsForTraining(iftImage *label, iftDataSet *dataset, iftImage *gt_img, iftIntArray *nquery_perclass) {
  iftIntArray *train, *aux_train, *marked_sup;
  int p, idx_sup, nreal_selsup;
  int nobjs = nquery_perclass->n;
  int nsups = iftMaximumValue(label);
  int ntotal_query = 0;
  marked_sup = iftCreateIntArray(nsups);
  for (int i = 0; i < nobjs; ++i)
    ntotal_query += nquery_perclass->val[i];

  aux_train = iftCreateIntArray(ntotal_query);
  int idx_selsup = 0;
  for (int i = 0; i < nobjs; ++i) { 
    int obj_val = i + 1;
    int nselected = 0;
    int niters = 0;
    while(niters < label->n && nselected < nquery_perclass->val[i]) {
      p = iftRandomInteger(0, label->n-1);
      idx_sup = label->val[p] - 1;
      //if (marked_sup->val[idx_sup] == 0 && gt_img->val[p] == obj_val) {
      //if (gt_img->val[p] == obj_val) {
      if (marked_sup->val[idx_sup] == 0 && dataset->sample[idx_sup].truelabel == obj_val) {
        //printf("p %d\n", p);
        aux_train->val[idx_selsup] = idx_sup;
        marked_sup->val[idx_sup] = 1;
        idx_selsup++;
        nselected++;
      }
      niters++;
    }
    printf("obj %d nselected %d\n", i+1, nselected);
  }
  nreal_selsup = idx_selsup;
  train = iftCreateIntArray(nreal_selsup);
  for (int i = 0; i < nreal_selsup; ++i)
    train->val[i] = aux_train->val[i];

  iftDestroyIntArray(&aux_train);
  iftDestroyIntArray(&marked_sup);
  return train;
}

iftIntArray *iftSelectSupervoxelsForTrainingUnbalanced(iftImage *label, iftDataSet *dataset, iftImage *gt_img, float perc_train) {
  iftIntArray *train, *marked_sup, *aux_train;
  int p, idx_sup;
  assert(perc_train <= dataset->nsamples);
  int nqueried_samples = dataset->nsamples;
  if (perc_train == 1.0) {
    train = iftCreateIntArray(nqueried_samples);
    for (int i = 0; i < nqueried_samples; ++i)
      train->val[i] = i;
  } else {
    if (perc_train <= 1)
      nqueried_samples = (int)(dataset->nsamples * perc_train);
    else
      nqueried_samples = (int)perc_train;
    
    marked_sup = iftCreateIntArray(dataset->nsamples);
    aux_train = iftCreateIntArray(nqueried_samples);
    int nselected = 0;
    int niters = 0;
    int idx_selsup = 0;
    while(niters < 2*label->n && nselected < nqueried_samples) {
      p = iftRandomInteger(0, label->n-1);
      idx_sup = label->val[p] - 1;
      if (marked_sup->val[idx_sup] == 0) {
        //printf("p %d\n", p);
        aux_train->val[idx_selsup] = idx_sup;
        marked_sup->val[idx_sup] = 1;
        idx_selsup++;
        nselected++;
      }
    }
    train = iftCreateIntArray(nselected);
    for (int i = 0; i < nselected; ++i) {
      train->val[i] = aux_train->val[i];
    }
    iftDestroyIntArray(&aux_train);
    iftDestroyIntArray(&marked_sup);
  }

  return train;
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

void iftClassifySupervoxlesUsingSelectedTrainSamples(iftDataSet *dataset, iftIntArray *trainsamples) {
  iftSetStatus(dataset, IFT_TEST);
  dataset->ntrainsamples = 0;
  for (int i = 0; i < trainsamples->n; ++i) {
    int idx_sup = trainsamples->val[i];
    dataset->sample[idx_sup].status = IFT_TRAIN;
    dataset->sample[idx_sup].label = dataset->sample[idx_sup].truelabel;
    dataset->ntrainsamples++;
  }
  printf("ntrainsamples %d\n", dataset->ntrainsamples);
  // test classifier 
  iftCplGraph *graph = iftCreateCplGraph(dataset);
  iftSupTrain(graph); // Training
  iftClassify(graph, dataset);  // Testing
  iftDestroyCplGraph(&graph);
}

iftIntArray *iftGetNumberOfTrainSamples(iftDataSet *dataset, float perc_train) {
  iftIntArray *nsamples_perclass, *nquery_perclass;
  int nobjs;
  nobjs = dataset->nclasses;
  // obtain number of samples per class to be selected
  nsamples_perclass = iftCreateIntArray(nobjs);
  for (int s = 0; s < dataset->nsamples; ++s)
    nsamples_perclass->val[dataset->sample[s].truelabel - 1] += 1;
  // compute number of samples for training  
  int total_query_samples;
  if (perc_train <= 1)
    total_query_samples = (int)(dataset->nsamples * perc_train); // use percentage of samples
  else
    total_query_samples = (int)perc_train;  // use number of samples
  
  nquery_perclass = iftCreateIntArray(nobjs);
  for (int i = 0; i < nobjs; ++i) {
    nquery_perclass->val[i] = total_query_samples / nobjs;  // balanced
    if (nquery_perclass->val[i] < 1) {
      nquery_perclass->val[i] = 1;
    }
    printf("class %d nsamples %d\n", i+1, nsamples_perclass->val[i]);
  }
  iftDestroyIntArray(&nsamples_perclass);

  return nquery_perclass;
}

void iftCopySamplesToDataset(iftDataSet * combined_dataset, iftDataSet *dataset, iftIntArray *trainsamples, int index_offset) {
  int index_comb_dataset = index_offset;
  for (int i = 0; i < trainsamples->n; ++i) {
    int index_sample = trainsamples->val[i];
    for (int j = 0; j < dataset->nfeats; ++j) {
      combined_dataset->sample[index_comb_dataset].feat[j] = dataset->sample[index_sample].feat[j];
    }
    combined_dataset->sample[index_comb_dataset].truelabel = dataset->sample[index_sample].truelabel;
    combined_dataset->sample[index_comb_dataset].id = dataset->sample[index_sample].id;
    index_comb_dataset++;
  }
}

iftIntArray *iftSelectSupervoxelsForTrainingFromLabeledSet(iftImage *label, iftLabeledSet *seeds) {
  iftLabeledSet *s;
  iftIntArray *marked_sup, *trainsamples, *aux_trainsamples;
  int p, idx_sup, count_samples;
  int nsups = iftMaximumValue(label);
  marked_sup = iftCreateIntArray(nsups);
  aux_trainsamples = iftCreateIntArray(nsups);
  s = seeds;
  count_samples = 0;
  while(s != NULL){
    p = s->elem;
    //obj = s->label + 1; // for binary classification labels 1 and 2
    idx_sup = label->val[p] - 1;
    if (marked_sup->val[idx_sup] == 0) {
      aux_trainsamples->val[count_samples] = idx_sup;
      count_samples++;
      marked_sup->val[idx_sup] = 1;
    }
    s = s->next;
  }
  trainsamples = iftCreateIntArray(count_samples);
  for (int i = 0; i < count_samples; ++i) {
    trainsamples->val[i] = aux_trainsamples->val[i];
  }
  iftDestroyIntArray(&aux_trainsamples);
  iftDestroyIntArray(&marked_sup);
  return trainsamples;
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
  iftImage  *gt_img, *label, *img;
  iftDataSet *dataset, *combined_dataset; 
  float perc_train;
  iftIntArray *trainsamples;
  int nimages, index_comb_dataset, descriptor;
  /*--------------------------------------------------------*/

  int MemDinInicial, MemDinFinal;
  MemDinInicial = iftMemoryUsed();

  /*--------------------------------------------------------*/
  
  if (argc<7 || argc>8)
      iftError(
              "Usage: iftTrainSupervoxelsClassifierFromDir <image_dir> <gt_image_dir> <label_dir> <descriptor (0:MC, 1:BIC, 2:MC-CONTEXTUAL, 3:BIC-CONTEXTUAL)> <perc_train or num_trainsaples_perimage> <output_classifier> <OPTIONAL markers_dir>",
              "main");
    
  iftDir* markerfiles = NULL;
  iftDir* imagefiles = iftLoadFilesFromDirBySuffix(argv[1], "ppm");
  iftDir* gtfiles = iftLoadFilesFromDirBySuffix(argv[2], "pgm");
  iftDir* labelfiles = iftLoadFilesFromDirBySuffix(argv[3], "pgm");
  descriptor = atoi(argv[4]);
  perc_train = atof(argv[5]);
  nimages = gtfiles->nfiles;

  if (argc == 8)
    markerfiles = iftLoadFilesFromDirBySuffix(argv[7], "txt");

  // compute total number of train samples
  int total_ntrainsamples = 0;
  int nfeats;
  if (argc == 8) { // trainsamples from markers
    for (int i = 0; i < nimages; i++) {
      label = iftReadImageByExt(labelfiles->files[i]->path);
      iftLabeledSet *seeds = iftReadSeeds2D(markerfiles->files[i]->path, label);
      trainsamples = iftSelectSupervoxelsForTrainingFromLabeledSet(label, seeds);
      total_ntrainsamples += trainsamples->n;
      img = iftReadImageByExt(imagefiles->files[0]->path);
      dataset = iftExtractDescriptorFromSupervoxelDataSet(img, label, descriptor);
      nfeats = dataset->nfeats;
      iftDestroyLabeledSet(&seeds);
      iftDestroyIntArray(&trainsamples);
      iftDestroyImage(&label);
      iftDestroyImage(&img);
    }
  } else if (perc_train <= 1) {
    for (int i = 0; i < nimages; i++) {
      img = iftReadImageByExt(imagefiles->files[i]->path);
      label = iftReadImageByExt(labelfiles->files[i]->path);
      dataset = iftExtractDescriptorFromSupervoxelDataSet(img, label, descriptor);
      nfeats = dataset->nfeats;
      total_ntrainsamples += (int)(dataset->nsamples * perc_train);
      iftDestroyDataSet(&dataset);
      iftDestroyImage(&img);
      iftDestroyImage(&label);
    }
  } else {
    total_ntrainsamples = ((int)perc_train) * nimages;
    img = iftReadImageByExt(imagefiles->files[0]->path);
    label = iftReadImageByExt(labelfiles->files[0]->path);
    dataset = iftExtractDescriptorFromSupervoxelDataSet(img, label, descriptor);
    nfeats = dataset->nfeats;
    iftDestroyDataSet(&dataset);
    iftDestroyImage(&img);
    iftDestroyImage(&label);
  }
  
  combined_dataset = iftCreateDataSet(total_ntrainsamples, nfeats);
  printf("total_ntrainsamples %d nfeats %d\n", total_ntrainsamples, nfeats);
  index_comb_dataset = 0;

  for (int i = 0; i < nimages; i++) {
    img = iftReadImageByExt(imagefiles->files[i]->path);
    gt_img = iftReadImageByExt(gtfiles->files[i]->path);
    label = iftReadImageByExt(labelfiles->files[i]->path);
    dataset = iftExtractDescriptorFromSupervoxelDataSet(img, label, descriptor);
    iftRelabelObjectLabelsForBinaryClassification(gt_img); // relabel 0 -> 1 and 1 or 255 -> 2
    iftSetSupervoxelObjectLabels(label, gt_img, dataset);
    // obtain train samples
    if (argc == 8) { // train samples from markers
      printf("Reading markers... %s\n", markerfiles->files[i]->path);
      iftLabeledSet *seeds = iftReadSeeds2D(markerfiles->files[i]->path, label);
      printf("train samples ok\n");
      trainsamples = iftSelectSupervoxelsForTrainingFromLabeledSet(label, seeds);
      iftDestroyLabeledSet(&seeds);
      // add to train samples
      iftCopySamplesToDataset(combined_dataset, dataset, trainsamples, index_comb_dataset);      
      index_comb_dataset += trainsamples->n;

      iftDestroyIntArray(&trainsamples);
    } else { // randomly selected train samples
      iftRandomSeed(IFT_RANDOM_SEED);
      // get number of train samples
      //iftIntArray *nquery_perclass
      //nquery_perclass = iftGetNumberOfTrainSamples(dataset, perc_train);
      //trainsamples = iftSelectSupervoxelsForTraining(label, dataset, gt_img, nquery_perclass);
      //iftDestroyIntArray(&nquery_perclass);
      trainsamples = iftSelectSupervoxelsForTrainingUnbalanced(label, dataset, gt_img, perc_train);
      printf("ntrainsamples per img %d\n", (int)trainsamples->n);
      
      iftCopySamplesToDataset(combined_dataset, dataset, trainsamples, index_comb_dataset);
      index_comb_dataset += trainsamples->n;
      iftDestroyIntArray(&trainsamples);
    }

    // Free
    iftDestroyImage(&gt_img);
    iftDestroyImage(&label);
    iftDestroyImage(&img);
    iftDestroyDataSet(&dataset);
  }
  
  iftSetStatus(combined_dataset, IFT_TRAIN);
  combined_dataset->ntrainsamples = combined_dataset->nsamples;
  printf("ntrainsamples %d\n", combined_dataset->ntrainsamples);

  // Train classifier
  iftCplGraph *graph = iftCreateCplGraph(combined_dataset);
  iftSupTrain(graph); // Training
  iftWriteCplGraph(graph, argv[6]); // Write classifier
  iftDestroyCplGraph(&graph);

  // Free
  iftDestroyDataSet(&combined_dataset);
  iftDestroyDir(&gtfiles);
  iftDestroyDir(&labelfiles);
  iftDestroyDir(&imagefiles);
  if (markerfiles != NULL)
    iftDestroyDir(&markerfiles);

  /* ---------------------------------------------------------- */

  MemDinFinal = iftMemoryUsed();
  iftVerifyMemory(MemDinInicial, MemDinFinal);

  return(0);
}
