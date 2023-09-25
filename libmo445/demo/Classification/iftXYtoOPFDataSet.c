#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftDataSet      *Z=NULL, *Zaux=NULL;
  timer           *t1=NULL,*t2=NULL;

  /*--------------------------------------------------------*/

  int MemDinInicial, MemDinFinal;
  MemDinInicial = iftMemoryUsed();

  /*--------------------------------------------------------*/


  if (argc!=3)
    iftError("Usage: iftXYtoOPFDataSet <input.txt> <output.dat>","main");

  t1     = iftTic();


  Z  = iftReadXYDataSet(argv[1]); 
  printf("Total number of samples  %d\n",Z->nsamples);
  printf("Total number of features %d\n",Z->nfeats);
  printf("Remove ambiguous samples\n");
  Zaux  = iftEliminateAmbiguousSamples(Z);
  iftDestroyDataSet(&Z);
  iftWriteOPFDataSet(Zaux,argv[2]);
  t2     = iftToc();

  printf("Total number of samples  %d\n",Zaux->nsamples);
  printf("Total number of features %d\n",Zaux->nfeats);

  fprintf(stdout,"DataSet converted in %f ms\n",iftCompTime(t1,t2));

  iftDestroyDataSet(&Z);
  iftDestroyDataSet(&Zaux);
  
  /* ---------------------------------------------------------- */

  MemDinFinal = iftMemoryUsed();
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}




