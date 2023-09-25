#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftImage        *group,*label;
  iftLabeledSet   *seed=NULL, *extended_seed=NULL;
  size_t           mem_start, mem_end;
  timer           *t1, *t2;

  mem_start = iftMemoryUsed();

  if (argc != 4){
    printf("Usage: iftExtendSeedSet <input-groups.*> <input-seeds.txt> <output-extended-seeds.txt>\n");
    exit(1);
  }
  
  t1 = iftTic();

  iftAdjRel *A;
  iftImage *aux = iftReadImageByExt(argv[1]);
  if (iftIs3DImage(aux))
    A=iftSpheric(1.0);
  else
    A=iftCircular(1.0);
  group = iftRelabelRegions(aux,A);
  iftDestroyAdjRel(&A);
  iftDestroyImage(&aux);
  label = iftCreateImageFromImage(group);
  seed  = iftReadSeeds(argv[2],label);
  iftLabeledSetToImage(seed,label,true);
  
  int  nlabels           = iftMaximumValue(label);
  int  ngroups           = iftMaximumValue(group);
  int *group_with_seeds  = iftAllocIntArray(ngroups);
  float *purity          = iftAllocFloatArray(ngroups);

  while (seed != NULL){
    int lb;
    int p = iftRemoveLabeledSet(&seed,&lb);
    group_with_seeds[group->val[p]-1]=lb+1;
  }
  
  int **hist  = (int **)calloc(ngroups,sizeof(int *));
  for (int i=0; i < ngroups; i++)
    hist[i] = (int *)calloc(nlabels,sizeof(int));

  for (int p=0; p < label->n; p++){
    if (label->val[p]>0)
      hist[group->val[p]-1][label->val[p]-1]++;
  }

  for (int i=0; i < ngroups; i++){
    if (group_with_seeds[i]>0){
      int maxlabel = 0, sum = hist[i][0];
      for (int j=1; j < nlabels; j++){
  	sum += hist[i][j];
  	if (hist[i][maxlabel] < hist[i][j])
  	  maxlabel = j;
      }
      purity[i] = (float)hist[i][maxlabel]/(float)sum;
    }
  }

  /* These seeds are suitable for object saliency estimation (using
     iftTrainImageClassifierByOPF and iftClassifyImageByOPF). If you
     include the input seeds, then you will have seeds for delineation
     as well. */
  
  for (int p=0; p < label->n; p++){
    if (purity[group->val[p]-1]==1.0){
      iftInsertLabeledSet(&extended_seed,p,group_with_seeds[group->val[p]-1]-1);
    }
  }
  
  iftWriteSeeds(argv[3],extended_seed,label);
  
  for (int i=0; i < ngroups; i++)
    iftFree(hist[i]);
  iftFree(hist);

  iftFree(group_with_seeds);
  iftFree(purity);
  if (extended_seed!=NULL)
    iftDestroyLabeledSet(&extended_seed);
  iftDestroyImage(&label);
  iftDestroyImage(&group);
  
  mem_end = iftMemoryUsed();
  iftVerifyMemory(mem_start,mem_end);
    
  t2 = iftToc();
  puts(iftFormattedTime(iftCompTime(t1,t2)));

  return(0);
}
