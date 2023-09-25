#include "ift.h"

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
        printf("p %d\n", p);
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
  /*
  int total_query_samples;
  if (perc_train <= 1)
    total_query_samples = (int)(dataset->nsamples * perc_train); // use percentage of samples
  else
    total_query_samples = (int)perc_train;  // use number of samples
  */
  nquery_perclass = iftCreateIntArray(nobjs);
  for (int i = 0; i < nobjs; ++i) {
    //nquery_perclass->val[i] = total_query_samples / nobjs;  // balance
    /*
    if (nobjs == 2) {
      if (i == 0)
        nquery_perclass->val[0] = (int)(total_query_samples * (2/3.0));
      else
        nquery_perclass->val[1] = (int)(total_query_samples * (1/3.0));
    }
    */
    nquery_perclass->val[i] = (int)(nsamples_perclass->val[i] * perc_train); // no balance
    if (nquery_perclass->val[i] < 1) {
      nquery_perclass->val[i] = 1;
    }
    printf("class %d nsamples %d\n", i+1, nsamples_perclass->val[i]);
  }
  iftDestroyIntArray(&nsamples_perclass);

  return nquery_perclass;
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

int main(int argc, char *argv[]) 
{
  iftImage  *gt_img, *label, *class_map;
  iftDataSet *dataset; 
  float perc_train;
  iftIntArray *nquery_perclass, *trainsamples;
  int nimages;
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
  
  if (argc<7 || argc>8)
    iftError("Usage: iftClassifySupervoxelsFromDir <gt_image_dir> <label_dir> <dataset_dir> <perc_train or num_train> <output_dir> <output_csv> <OPTIONAL markers_dir>","main");
  
  iftDir* markerfiles = NULL;
  iftDir* gtfiles = iftLoadFilesFromDirBySuffix(argv[1], "pgm");
  iftDir* labelfiles = iftLoadFilesFromDirBySuffix(argv[2], "pgm");
  iftDir* datasetfiles = iftLoadFilesFromDirBySuffix(argv[3], "zip");
  if (argc == 8)
    markerfiles = iftLoadFilesFromDirBySuffix(argv[7], "txt");

  nimages = gtfiles->nfiles;
  f1 = fopen(argv[6], "w");
  float mean_acc = 0;
  for (int i = 0; i < nimages; i++) {
    gt_img  = iftReadImageByExt(gtfiles->files[i]->path);
    label  = iftReadImageByExt(labelfiles->files[i]->path);
    dataset = iftReadOPFDataSet(datasetfiles->files[i]->path);
    perc_train = atof(argv[4]);
    iftRelabelObjectLabelsForBinaryClassification(gt_img);
    // get number of train samples
    nquery_perclass = iftGetNumberOfTrainSamples(dataset, perc_train);
    // obtain train samples
    if (argc == 8) {
      printf("Reading markers... %s\n", markerfiles->files[i]->path);
      iftLabeledSet *seeds = iftReadSeeds2D(markerfiles->files[i]->path, label);
      printf("train samples ok\n");
      trainsamples = iftSelectSupervoxelsForTrainingFromLabeledSet(label, seeds);
      iftDestroyLabeledSet(&seeds);
    } else {
      trainsamples = iftSelectSupervoxelsForTraining(label, dataset, gt_img, nquery_perclass);
    }
    // Classify samples
    iftClassifySupervoxlesUsingSelectedTrainSamples(dataset, trainsamples);
    printf("classifier ok\n");
    // compute overall accuracy
    class_map = iftGetClassificationMap(label, dataset);
    float acc = iftComputeAccuracyFromClassificationMap(class_map, gt_img);
    mean_acc += acc;
    printf("Accuracy of classification map %f\n", acc);
    // write classification map
    iftRelabelBinaryClassificationMapToZeroOneValues(class_map);
    sprintf(filename_classmap, "%s/%s.pgm", argv[5] ,iftFilename(gtfiles->files[i]->path, ".pgm"));
    printf("%s\n", filename_classmap);
    iftWriteImageP2(class_map, filename_classmap);

    fprintf(f1,"%f\n", acc);

    // Free
    iftDestroyIntArray(&trainsamples);
    iftDestroyIntArray(&nquery_perclass);
    iftDestroyImage(&gt_img);
    iftDestroyImage(&label);
    iftDestroyImage(&class_map);
    iftDestroyDataSet(&dataset);
  }
  mean_acc /= (float) nimages;
  fprintf(f1,"%f\n", mean_acc);  

  fclose(f1);

  iftDestroyDir(&gtfiles);
  iftDestroyDir(&labelfiles);
  iftDestroyDir(&datasetfiles);
  if (argc == 8)
    iftDestroyDir(&markerfiles);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
     MemDinInicial,MemDinFinal);   

  return(0);
}
