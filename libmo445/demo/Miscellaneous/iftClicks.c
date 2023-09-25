#include "ift.h"

int *iftFindClicks(iftMatrix *S, float simil_thres)
{
  int  l = 1, elem=0; 
  int *L = iftAllocIntArray(S->ncols);

  if (S->ncols != S->nrows) 
    iftError("Similarity matrix must be squared","iftFindClicks"); 

  while (elem < S->ncols) {
    if (L[elem]==0){  /* elem defines a new group */
      L[elem]=l; l++;
      for (int i=0; i < S->ncols; i++) {
	if (L[i]==0){ /* an element searching for a group */
	  if (S->val[iftGetMatrixIndex(S,i,elem)] >= simil_thres){
	    int same_group_as_elem = 1; /* verify if i belongs to the
					   same group of elem */
	    for (int j=0; j < S->ncols; j++) {
	      if ( (L[j] == L[elem]) && 
		   (S->val[iftGetMatrixIndex(S,i,j)] < simil_thres) ){
		same_group_as_elem = 0;	   /* it does not belong */ 
		break;
	      }
	    }
	    if (same_group_as_elem) { /* it belongs */
	      L[i]=L[elem];
	    }
	  }
	}
      }
    }
    elem++; /* new elem */
  }
	  
  return(L);
}

int main(int argc, char *argv[]) 
{
  iftMatrix *S;
  FILE *fp;  
  int   *L, nelems, max = 0, count; 
  char  names[100][100];

  if (argc!=3)
    iftError("Usage: iftClicks <similarity_table.txt> <threshold>","main");

  fp = fopen(argv[1],"r");
  fscanf(fp,"%d\n",&nelems); 

  S = iftCreateMatrix(nelems,nelems);

  for (int i = 0; i < nelems; i++)
  {
     fscanf(fp,"%s", names[i]); 
  }

  for (int i = 0; i < nelems; i++)  { 
    for (int j = 0; j < nelems; j++) 
      fscanf(fp,"%lf",&S->val[iftGetMatrixIndex(S,j,i)]);
    fscanf(fp,"\n");
  }
  fclose(fp); 

  L = iftFindClicks(S, atof(argv[2]));

  for (int i = 0; i < nelems; i++)
     max = MAX(max, L[i]);

  for (int i = 1; i <= max; i++)  {     
     count = 0; 
     for (int j = 0; j < nelems; j++)  {     
        if (L[j] == i)
           count++; 
     }
     printf("Group %d (%d elems): ", i, count);
     for (int j = 0; j < nelems; j++)  {     
        if (L[j] == i)
           printf("%s ", names[j]); 
     }
     printf("\n");
  }

  return(0);
}
