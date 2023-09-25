#include "ift.h"

/* Exemplifies how to use the sorting algorithms */

typedef struct _objectlist {
  char name[20];
  int   value;
  float fvalue;
} ObjectList;

int main(int argc, char *argv[]) 
{
  ObjectList obj[100];
  int value[100], index[100], i;
  float fvalue[100];
  char  **name;
  timer          *t1=NULL,*t2=NULL;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  iftRandomSeed(IFT_RANDOM_SEED);

  if (argc!=1)
    iftError("Usage must be: iftSortObjects","main");
  
  name = (char **) calloc(100,sizeof(char *));
  for (i=0; i < 100; i++) {
    obj[i].value=value[i]  =i;
    obj[i].fvalue=fvalue[i]=i;
    index[i] = i;
    sprintf(obj[i].name,"Object%d",i+1);
    name[i] = iftAllocCharArray(20);
    sprintf(name[i],"Object%d",i+1);
  }

  printf("Before sorting\n");
  for (i=0; i < 100; i++) {
    printf("%s has value %f (%d)\n",obj[index[i]].name,obj[index[i]].fvalue,obj[index[i]].value);
  }
  
  t1     = iftTic();

  //iftBucketSort(value, index, 100, DECREASING);
  //iftQuickSort(value, index, 0, 99, DECREASING); 
  //iftFHeapSort(fvalue, index, 100, DECREASING);
  //iftFQuickSort(fvalue, index, 0, 99, DECREASING); 
  iftSQuickSort(name, index, 0, 99, DECREASING, 20);

  t2     = iftToc();
  fprintf(stdout,"sorting in %f ms\n",iftCompTime(t1,t2));

  printf("After sorting\n");
  for (i=0; i < 100; i++) {
    printf("%s has value %f (%d)\n",obj[index[i]].name,obj[index[i]].fvalue,obj[index[i]].value);
  }

  /*
  printf("Sorted values\n");
  for (i=0; i < 100; i++) {
    printf("value %f (%d)\n",fvalue[i],value[i]);
  }
  */

  for (i=0; i < 100; i++) 
    free(name[i]);
  free(name);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);

}
