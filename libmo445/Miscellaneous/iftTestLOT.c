#include "ift.h"

/* Use the distance between voxels of same intensity in order to match the closest ones first. The distance beween images may be the total distance of the matching operation (this is not implemented yet) */

double iftImageDistance(iftImage *img1, iftImage *img2)
{
  int p; 
  double dist=0.0;

  for (p=0; p < img1->n; p++) 
    dist += (img1->val[p]-img2->val[p])*(img1->val[p]-img2->val[p]);

  return(sqrt(dist));
}

iftImage *iftNormalizeImageEnergy(iftImage *img, int K)
{
  iftImage *nimg=iftCreateImage(img->xsize,img->ysize,img->zsize);
  double energy=0.0;
  int p;
  
  K = K * img->n;

  for (p=0; p < img->n; p++) {
    energy += img->val[p];
  }

  for (p=0; p < img->n; p++) {
    nimg->val[p] = (int)(K*((double)img->val[p]/energy));
  }

  return(nimg);
}

iftImage *iftLOT(iftImage *ref, iftImage *def)
{
  iftGQueue *Q;
  int *refvoxel, *defvoxel, p, q, i;
  iftImage *reg;


  reg = iftCreateImage(ref->xsize,ref->ysize,ref->zsize);

  refvoxel = iftAllocIntArray(ref->n);
  defvoxel = iftAllocIntArray(def->n);

  Q = iftCreateGQueue(iftMaximumValue(ref)+1,ref->n,ref->val);
  for (p=0; p < ref->n; p++) 
    iftInsertGQueue(&Q,p);
  q = 0;
  while (!iftEmptyGQueue(Q)){
    p = iftRemoveGQueue(Q);
    refvoxel[q]=p; q++;
  }
  iftDestroyGQueue(&Q);

  Q = iftCreateGQueue(iftMaximumValue(def)+1,def->n,def->val);
  for (p=0; p < def->n; p++) 
    iftInsertGQueue(&Q,p);
  q = 0;
  while (!iftEmptyGQueue(Q)){
    p = iftRemoveGQueue(Q);
    defvoxel[q]=p; q++;
  }
  iftDestroyGQueue(&Q);

  for (i=0; i < ref->n; i++) {
    p = refvoxel[i];
    q = defvoxel[i];
    reg->val[p] = def->val[q];
  }

  free(refvoxel);
  free(defvoxel);
  
  return(reg);
}

int main(int argc, char *argv[]) 
{
  iftImage *ref=NULL, *def=NULL, *reg=NULL;
  iftImage *nref=NULL, *ndef=NULL;
  char      ext[10],*pos,command[200],*filename,pathname[200];
  FILE     *fp;
  int       i, nfiles;

  /*--------------------------------------------------------*/

  int MemDinInicial, MemDinFinal;
  MemDinInicial = iftMemoryUsed();

  /*--------------------------------------------------------*/

  if (argc!=4)
      iftError("Usage: iftTestLOT <reference.pgm> <origpath> <destpath>", "main");

  pos = strrchr(argv[1],'.') + 1;
  sscanf(pos,"%s",ext);
  if (strcmp(ext,"pgm")==0){  
    ref   = iftReadImageP5(argv[1]); 
  } else {
      iftError("Usage: iftTestLOT <reference.pgm> <origpath> <destpath>", "main");
  }
  nref = iftNormalizeImageEnergy(ref,255);
  iftDestroyImage(&ref);

  sprintf(command,"ls -v %s/*.%s > temp.txt",argv[2],ext);
  system(command);
  fp = fopen("temp.txt","r");
  nfiles = 0;
  while (!feof(fp)){
    fscanf(fp,"%s",pathname);
    nfiles++;
  }
  fclose(fp);
  nfiles--;

  fp = fopen("temp.txt","r");

  timer *t1=iftTic();

  for (i=0; i < nfiles; i++) {
    fscanf(fp,"%s",pathname);
    def   = iftReadImageP5(pathname); 
    filename  = iftSplitStringAt(pathname,"/",-1);
      
    /* Normalize both images */
    
    ndef = iftNormalizeImageEnergy(def,255);    
    iftDestroyImage(&def);

    /* Compute LOT */

    reg  = iftLOT(nref,ndef);
    
    /* Write output file */

    sprintf(pathname,"%s/%s",argv[3],filename);
    iftWriteImageP5(reg,pathname);

    printf("%s %s %lf\n",argv[1],filename,iftImageDistance(nref,reg));

    iftDestroyImage(&ndef);
    iftDestroyImage(&reg);
    free(filename);
  }
  
  fclose(fp);

  timer *t2=iftToc();

  system("rm -f temp.txt");
  printf("LOT tested in %f ms\n",iftCompTime(t1,t2));

  iftDestroyImage(&nref);

  /* ---------------------------------------------------------- */

  MemDinFinal = iftMemoryUsed();
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}

