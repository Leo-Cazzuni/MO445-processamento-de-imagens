#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ift.h"

int main(int argc, char ** argv) {
  iftDataSet* ZOPF;
  if (argc != 3) {
    char msg[300];
    sprintf(msg,"usage: %s <OPFDataSet.dat> <output>\n",argv[0]);
    iftError(msg,"iftOPFDataSetToTxt");
  }

  char input[100] ; strcpy(input ,argv[1]);
  char output[100]; strcpy(output,argv[2]);

  ZOPF  = iftReadOPFDataSet(input); // Read trainning dataset Z_train
  
  FILE* pF=fopen(output,"wt");
  if (pF == NULL) {
    char msg[300]; 
    sprintf(msg,"usage: %s <OPFDataSet.dat> <output>\n",argv[0]);
    iftError(msg,"iftOPFDataSetToTxt");
  }

  fprintf(pF,"%d %d %d\n",ZOPF->nsamples,ZOPF->nclasses,ZOPF->nfeats);
  for(int s=0;s<ZOPF->nsamples;s++) {
    fprintf(pF,"%d %d",s,ZOPF->sample[s].class);
    for(int ff=0;ff<ZOPF->nfeats;ff++)
      fprintf(pF," %f",ZOPF->sample[s].feat[ff]);
    fprintf(pF,"\n");
  }

  fclose(pF);

  iftDestroyDataSet(&ZOPF);

  return 0;
}
