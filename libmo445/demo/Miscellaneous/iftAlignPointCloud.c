#include "ift.h"

typedef struct ift_kinect_point {
  float x, y, z;
} iftKPoint;

typedef struct ift_kinect_points {
  iftKPoint **P;
  int npoints, nframes;
} iftKinectPoints;

iftKinectPoints *iftReadKinectPoints(char *filename)
{
  iftKinectPoints *pt = (iftKinectPoints *) calloc(1,sizeof(iftKinectPoints));
  FILE *fp;
  int s,l;

  fp = fopen(filename,"r");
  fscanf(fp,"%d %d",&pt->nframes,&pt->npoints);
  pt->P = (iftKPoint **) calloc(pt->nframes,sizeof(iftKPoint *));
  for (s=0; s < pt->nframes; s++) 
    pt->P[s] = (iftKPoint *) calloc(pt->npoints,sizeof(iftKPoint));
 
  for (s=0; s < pt->nframes; s++) 
    for (l=0; l < pt->npoints; l++) {
      fscanf(fp,"%f %f %f",&pt->P[s][l].x,&pt->P[s][l].y,&pt->P[s][l].z);
    }

  fclose(fp);

  return(pt);
}

void iftDestroyKinectPoints(iftKinectPoints **pt)
{
  int s;
  iftKinectPoints *aux = *pt;

  for (s=0; s < aux->nframes; s++) 
    free(aux->P[s]);
  free(aux->P);
  free(aux);
  *pt = NULL;
}

void iftCorrectSymmetryErrors(iftKinectPoints *pt)
{
  int l, s;
  int sign[3];

  for (l=0; l < pt->npoints; l++) {

    sign[0]=sign[1]=sign[2]=0;

    for (s=0; s < pt->nframes; s++) {
      if (pt->P[s][l].x > 0.0) 
	sign[0]++;
      else
	if (pt->P[s][l].x < 0.0) 
	  sign[0]--;
      if (pt->P[s][l].y > 0.0) 
	sign[1]++;
      else
	if (pt->P[s][l].y < 0.0) 
	  sign[1]--;
      if (pt->P[s][l].z > 0.0) 
	sign[2]++;
      else
	if (pt->P[s][l].z < 0.0) 
	  sign[2]--;
    }

    for (s=0; s < pt->nframes; s++) {
      if (pt->P[s][l].x*sign[0] < 0) 
	pt->P[s][l].x = -pt->P[s][l].x;
      if (pt->P[s][l].y*sign[1] < 0) 
	pt->P[s][l].y = -pt->P[s][l].y;
      if (pt->P[s][l].z*sign[2] < 0) 
	pt->P[s][l].z = -pt->P[s][l].z;
    }
  }
}

iftImage *iftPointCloudScene(iftKinectPoints *pt, int size, int number)
{
  iftImage *label; 
  int s,l,p;
  iftVoxel u;
  float xmin,xmax,ymin,ymax,zmin,zmax;

  xmin = ymin = zmin =  INFINITY_FLT;
  xmax = ymax = zmax = -INFINITY_FLT;
  for (s=0; s < pt->nframes; s++) {
    for (l=0; l < pt->npoints; l++) {
      if (pt->P[s][l].x < xmin)
	xmin = pt->P[s][l].x;
      if (pt->P[s][l].y < ymin)
	ymin = pt->P[s][l].y;
      if (pt->P[s][l].z < zmin)
	zmin = pt->P[s][l].z;
      if (pt->P[s][l].x > xmax)
	xmax = pt->P[s][l].x;
      if (pt->P[s][l].y > ymax)
	ymax = pt->P[s][l].y;
      if (pt->P[s][l].z > zmax)
	zmax = pt->P[s][l].z;
    }
  }

  printf("%f %f %f %f %f %f\n",xmin,xmax,ymin,ymax,zmin,zmax);

  label = iftCreateImage(size,size,size);

  for (s=0; s < pt->nframes; s++) {
    for (l=0; l < pt->npoints; l++) {
      u.x = size*(pt->P[s][l].x + 0.1)/0.2;
      u.y = size*(pt->P[s][l].y + 0.1)/0.2;
      u.z = size*(pt->P[s][l].z + 0.1)/0.2;

      p   = iftGetVoxelIndex(label,u);
      label->val[p] = number; // l+1;
    }
  }

  return(label);
}

void iftWritePointCloud(iftKinectPoints *pt, char *filename)
{
  FILE *fp;
  int s,l;

  fp = fopen(filename,"w");
  //  fprintf(fp,"%d %d\n",pt->nframes,pt->npoints);
  for (s=0; s < pt->nframes; s++) {
    for (l=0; l < pt->npoints; l++) 
      fprintf(fp,"%f %f %f ",pt->P[s][l].x,pt->P[s][l].y,pt->P[s][l].z);
    fprintf(fp,"\n");
  }

  fclose(fp);
}



int main(int argc, char **argv) 
{
  iftDataSet      *Z1,*Z2;
  iftImage        *label;
  iftKinectPoints *pt;
  int              s,t;
  char             filename[200];

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc!=4)
    iftError("Usage: iftAlignPointCloud <kinectpoints.txt> <individuo> <number>","main");

  pt = iftReadKinectPoints(argv[1]);

  /* Align points by PCA */

  for (s=0; s < pt->nframes; s++) {
    Z1  = iftCreateDataSet(pt->npoints,3);
    for (t=0; t < Z1->nsamples; t++) {
      Z1->sample[t].feat[0]   = pt->P[s][t].x;
      Z1->sample[t].feat[1]   = pt->P[s][t].y;
      Z1->sample[t].feat[2]   = pt->P[s][t].z;
      Z1->sample[t].truelabel = t+1;
    }
    Z2 = iftAlignDataSetByPCA(Z1);
    for (t=0; t < Z2->nsamples; t++) {
      pt->P[s][t].x = Z2->sample[t].feat[0];
      pt->P[s][t].y = Z2->sample[t].feat[1];
      pt->P[s][t].z = Z2->sample[t].feat[2];
    }
    iftDestroyDataSet(&Z1);
    iftDestroyDataSet(&Z2);
  }

  /* Correct symmetry errors in PCA by coordinate reflection */

  iftCorrectSymmetryErrors(pt);

  /* Create and Save Point Cloud Scene */

  label = iftPointCloudScene(pt, 500, atoi(argv[3]));
  sprintf(filename,"%s%d.scn",argv[2],atoi(argv[3]));
  iftWriteImage(label,filename);
  iftDestroyImage(&label);

  /* Write and Destroy Point Cloud */

  sprintf(filename,"%s%d.txt",argv[2],atoi(argv[3]));
  iftWritePointCloud(pt,filename);
  iftDestroyKinectPoints(&pt);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}

