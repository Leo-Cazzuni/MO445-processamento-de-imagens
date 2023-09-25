#include "ift.h"

#define PCA  0
#define LDA  1
#define tSNE 2

/* Reduce dimensionality of training and testing sets, and save new
   sets for future use (by A.X. Falcao, March 20th 2018) */

int main(int argc, char *argv[])
{
  iftDataSet      *Ztrain[2], *Ztest[2];
  timer           *tstart=NULL;
  int              MemDinInicial, MemDinFinal;
  int              method, new_dim=2, nfiles;
  char             filename[200];

  if ((argc != 5)&&(argc != 6)){
    iftExit("Usage: iftDimReduction <input-basename> <input-number-of-files> <output-basename> <input-method (0-PCA, 1-LDA, 2-t-SNE)> <input-new-dimension (optional for PCA and t-SNE)>", "main");
  }

  sprintf(filename,"%s_train_001.zip",argv[1]);    
  Ztrain[0] = iftReadDataSet(filename); 
  
  method = atoi(argv[4]);
  if ((method==PCA)||(method==tSNE)){
    new_dim = atoi(argv[5]);
    if ((new_dim <= 0)||(new_dim >= Ztrain[0]->nfeats))
      iftExit("Invalid new dimension","main");
  }

  if (method==LDA){
    if (Ztrain[0]->nclasses <= 1)
      iftExit("LDA requires a supervised dataset","main");
  }

  iftDestroyDataSet(&Ztrain[0]);
  nfiles = atoi(argv[2]);
  
  MemDinInicial = iftMemoryUsed(1);
   
  tstart = iftTic();

  switch (method) {
  case PCA:

    break;
  case LDA:

    break;
  case tSNE:
    for (int i=1; i <= nfiles; i++) {
      sprintf(filename,"%s_train_%03d.zip",argv[1],i);    
      Ztrain[0] = iftReadDataSet(filename); 
      sprintf(filename,"%s_test_%03d.zip",argv[1],i);    
      Ztest[0] = iftReadDataSet(filename); 
      iftDataSet *Z    = iftMergeDataSets(Ztrain[0], Ztest[0]);
      iftDataSet *Zlow = iftDimReductionByTSNE(Z, new_dim, 40, 1000);
      iftDestroyDataSet(&Z);
      Ztrain[1] = iftCopyDataSet(Ztrain[0],false);
      Ztest[1]  = iftCopyDataSet(Ztest[0],false);
      iftDestroyDataSet(&Ztrain[0]);
      iftDestroyDataSet(&Ztest[0]);
      Ztrain[1]->data = iftCreateMatrix(Zlow->nfeats,Ztrain[1]->nsamples);
      Ztest[1]->data  = iftCreateMatrix(Zlow->nfeats,Ztest[1]->nsamples);
      Ztrain[1]->nfeats = Zlow->nfeats;
      Ztest[1]->nfeats  = Zlow->nfeats;
      for (int s=0; s < Ztrain[1]->nsamples; s++){
	Ztrain[1]->sample[s].feat = &(Ztrain[1]->data->val[s*Zlow->nfeats]);
	for (int f=0; f < Zlow->nfeats; f++)
	  iftMatrixElem(Ztrain[1]->data,f,s) = iftMatrixElem(Zlow->data,f,s);
      }
      for (int s=Ztrain[1]->nsamples; s < Zlow->nsamples; s++){
	Ztest[1]->sample[s-Ztrain[1]->nsamples].feat = &(Ztest[1]->data->val[(s-Ztrain[1]->nsamples)*Zlow->nfeats]);
	for (int f=0; f < Zlow->nfeats; f++)
	  iftMatrixElem(Ztest[1]->data,f,s-Ztrain[1]->nsamples) = iftMatrixElem(Zlow->data,f,s);
      }
      iftDestroyDataSet(&Zlow);
      sprintf(filename,"%s_train_%03d.zip",argv[3],i);    
      iftWriteDataSet(Ztrain[1],filename); 
      sprintf(filename,"%s_test_%03d.zip",argv[3],i);    
      iftWriteDataSet(Ztest[1],filename); 
    }	
    break;
  default:
    iftExit("Usage: iftDimReduction <input-basename> <input-number-of-files> <output-basename> <input-method (0-PCA, 1-LDA, 2-t-SNE)> <input-new-dimension (optional for PCA and t-SNE)>", "main");
  }
  
  puts("\nDone...");
  puts(iftFormattedTime(iftCompTime(tstart, iftToc())));

  MemDinFinal = iftMemoryUsed();
  iftVerifyMemory(MemDinInicial, MemDinFinal);

  return(0);
}
