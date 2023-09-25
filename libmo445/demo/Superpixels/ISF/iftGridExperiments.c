#include "ift.h"

iftImage *iftExactGridSampling(iftMImage *img, iftImage *mask1, int nsamples)
{
  iftImage  *mask2 = iftCreateImage(img->xsize,img->ysize,img->zsize);
  iftImage  *prob  = iftBorderProbImage(img);
  int        p, xsize,ysize,zsize;
  float      xspacing,yspacing,zspacing,deltax,deltay,deltaz;
  iftAdjRel *A;
  iftVoxel   u, m, uo, uf;

  /* Compute the extreme voxels that define the region of interest in
     mask1, and then compute the xsize, ysize, and zsize of that
     ROI. */
  iftBoundingBox mbb = iftMinBoundingBox(mask1, NULL);
  uo = mbb.begin;
  uf = mbb.end;
  xsize = uf.x - uo.x + 1;
  ysize = uf.y - uo.y + 1;
  zsize = uf.z - uo.z + 1;

  if (iftIs3DMImage(img)){

    A  = iftSpheric(sqrtf(3.0));
    /* finds displacements along each axis */
    /* uncomment the next 4 lines to use same number o superpixels per axis */
    /*
    float nsamples_per_axis = (float) pow((double)nsamples,1.0/3.0);
    xspacing          = (xsize/nsamples_per_axis);
    yspacing          = (ysize/nsamples_per_axis);
    zspacing          = (zsize/nsamples_per_axis);
    */
    /* uncomment the next 5 lines to obtain equally spaced seeds in every axis */
    float superpixelsize = 0.5+(float)(img->n)/(float)nsamples;
    float step = (float) pow((double)superpixelsize,1.0/3.0)+0.5;
    xspacing = step;
    yspacing = step;
    zspacing = step;

    deltax            = xspacing/2.0;
    deltay            = yspacing/2.0;
    deltaz            = zspacing/2.0;

    if ((deltax < 1.0)||(deltay < 1.0)||(deltaz < 1.0))
      iftError("The number of samples is too high","iftGridSampling");

    for (m.z=(int)deltaz; m.z < (zsize-deltaz); m.z = (int)(m.z + zspacing)) {
      for (m.y=(int)deltay; m.y < (ysize-deltay); m.y = (int)(m.y + yspacing)) {
  for (m.x=(int)deltax; m.x < (xsize-deltax); m.x = (int)(m.x + xspacing)) {
    u.z = uo.z + m.z; u.y = uo.y + m.y; u.x = uo.x + m.x;
    p = iftGetVoxelIndex(mask1,u);
    if (mask1->val[p]!=0){
      mask2->val[p]=1;
    }
  }
      }
    }
  }else{
    A   = iftCircular(sqrtf(2.0));

    /* finds displacements along each axis  */
    /* uncomment the next 3 lines to use same number o superpixels per axis */
    /*
    nsamples_per_axis = (float) sqrt((double)nsamples);
    xspacing          = (xsize/nsamples_per_axis);
    yspacing          = (ysize/nsamples_per_axis);
    */
    /* uncomment the next 4 lines to obtain equally spaced seeds in every axis */
    float superpixelsize = 0.5+(float)(img->n)/(float)nsamples;
    float step = sqrt((float)superpixelsize)+0.5;
    xspacing = step;
    yspacing = step;


    deltax            = xspacing/2.0;
    deltay            = yspacing/2.0;

    if ((deltax < 1.0)||(deltay < 1.0))
      iftError("The number of samples is too high","iftGridSampling");

    u.z = 0;
    for (m.y=(int)deltay; m.y < ysize; m.y = (int)(m.y + yspacing)){
      for (m.x=(int)deltax; m.x < xsize; m.x = (int)(m.x + xspacing)) {
  u.y = uo.y + m.y; u.x = uo.x + m.x;
  p = iftGetVoxelIndex(mask1,u);
  if (mask1->val[p]!=0){
    mask2->val[p]=1;
  }
      }
    }
  }

  //printf("nseeds: %d\n",iftNumberOfElements(mask2));

  iftDestroyImage(&prob);
  iftDestroyAdjRel(&A);

  return(mask2);
}

iftImage *iftExtract2DGridSuperpixels(iftImage *img, int nsuperpixels, float alpha, float beta, int niters, int smooth_niters, int *nseeds_param, int *finalniters) {
  iftImage  *mask1, *seeds, *label;
  iftMImage *mimg;
  int nseeds, dist, p;
  iftVoxel center, u_ini, u_end, u;
  int *centers, *dist_vec;
  float superpixelsize, step;

  /* Compute GRID superpixels */

  if (iftIs3DImage(img)){
    superpixelsize = 0.5+(float)(img->n)/(float)nsuperpixels;
    step = (float) pow((double)superpixelsize,1.0/3.0)+0.5;
  } else {
    superpixelsize = 0.5+(float)(img->n)/(float)nsuperpixels;
    step = sqrt((float)superpixelsize)+0.5;
  }

  if (iftIsColorImage(img)){
    mimg   = iftImageToMImage(img,LABNorm_CSPACE);
  } else {
    mimg   = iftImageToMImage(img,GRAY_CSPACE);
  }

  mask1    = iftSelectImageDomain(mimg->xsize,mimg->ysize,mimg->zsize);
  seeds    = iftExactGridSampling(mimg,mask1,nsuperpixels);
  label    = iftCreateImage(img->xsize, img->ysize, img->zsize);

  nseeds   = iftNumberOfElements(seeds);
  *nseeds_param = nseeds;
  centers  = iftAllocIntArray(nseeds);
  dist_vec = iftAllocIntArray(img->n);
  for (int i = 0; i < img->n; ++i)
    dist_vec[i] = IFT_INFINITY_INT;

  int index_seed = 0;
  for (p = 0; p < seeds->n; ++p) {
    if (seeds->val[p] != 0) {
      centers[index_seed] = p;
      index_seed++;
    }
  }

  int large_step = (int)(2 * step);

  for (int i = 0; i < nseeds; ++i) {
    center = iftGetVoxelCoord(seeds, centers[i]);
    u_ini.x = iftMax(0, center.x - large_step);
    u_end.x = iftMin(img->xsize, center.x + large_step);
    u_ini.y = iftMax(0, center.y - large_step);
    u_end.y = iftMin(img->ysize, center.y + large_step);
    u_ini.z = iftMax(0, center.z - large_step);
    u_end.z = iftMin(img->zsize, center.z + large_step);
    for (u.x = u_ini.x; u.x < u_end.x; u.x++) {
      for (u.y = u_ini.y; u.y < u_end.y; u.y++) {
        for (u.z = u_ini.z; u.z < u_end.z; u.z++) {
          dist = (u.x - center.x)*(u.x - center.x) + (u.y - center.y)*(u.y - center.y) + (u.z - center.z)*(u.z - center.z);
          p = iftGetVoxelIndex(seeds, u);
          if (dist < dist_vec[p]) {
            dist_vec[p] = dist;
            label->val[p] = i+1;
          }
        }
      }
    }
  }

  printf("Maximum val %d\n", iftMaximumValue(label));

  iftDestroyMImage(&mimg);
  iftDestroyImage(&mask1);
  iftDestroyImage(&seeds);
  free(centers);
  free(dist_vec);
  return label;
}


int main(int argc, char *argv[]) 
{
  if(argc!=9)
    {
        iftError("Usage: <dataset path> <nsamples> <alpha (e.g., [0.005-0.2])> <beta (e.g., 12)> <niters (e.g., 10)> <smooth niters (e.g., 2)> <output path> <output csv nseeds file>", "ISFExperiments");
        return 1;
    }

  FILE *f1;
  iftImage *superpixels, *img;
  iftDir* datasetfiles = iftLoadFilesFromDirBySuffix(argv[1], "ppm");
  int i, niters, mean_niters, nseeds;
  float mean_nseeds;
  int nimages = datasetfiles->nfiles;
  char filename[256];
  f1 = fopen(argv[8], "w");

  mean_nseeds = 0.0;
  mean_niters = 0.0;

  for (i = 0; i < nimages; i++) {

    // Extract superpixels
    printf("Processing: %s\n", datasetfiles->files[i]->path);
    img = iftReadImageByExt(datasetfiles->files[i]->path);
    superpixels = iftExtract2DGridSuperpixels(img, atoi(argv[2]), atof(argv[3]), atof(argv[4]), atoi(argv[5]), atoi(argv[6]), &(nseeds), &niters);
    // Get filename without extension
    strcpy(filename, iftFilename(datasetfiles->files[i]->path, ".ppm"));
    // Write superpixels image
    iftWriteImageP2(superpixels, "%s/%s.pgm", argv[7], filename);

    mean_niters += niters;
    mean_nseeds += nseeds;
    fprintf(f1,"%f, %f\n", (float)niters, (float)nseeds);

    iftDestroyImage(&img);
    iftDestroyImage(&superpixels);
  }
  mean_niters /= nimages;
  mean_nseeds /= nimages;

  printf("Avg iters: %d \n", mean_niters);
  printf("MeanSeeds: %f \n", mean_nseeds);

  fprintf(f1,"%f, %f\n", (float)mean_niters, (float)mean_nseeds);

  fclose(f1);
  iftDestroyDir(&datasetfiles);
  
  return(0);
}
