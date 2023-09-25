#include "ift.h"

/* Author: Alexandre Xavier Falcão (September 10th, 2023)

   Description: Detects objects (e.g., parasite eggs) from the decoded
   saliency maps.

*/

iftImage *DrawBoxes(iftImage *orig, iftImage *comp, iftColor YCbCr, float scale)
{
  iftImage  *box     = iftCopyImage(orig);

  if (iftMaximumValue(comp)==0)
    return(box);

  iftAdjRel *A       = iftCircular(1.5);
  iftImage  *label   = iftFastLabelComp(comp, A);
  int        nlabels = iftMaximumValue(label);
  
  for (int l=1; l <= nlabels; l++) {
    iftImage *bin     = iftExtractObject(label,l);
    iftVoxel center;
    iftBoundingBox bb = iftMinBoundingBox(bin, &center);
    iftDestroyImage(&bin);
    int xsize = (bb.end.x - bb.begin.x)*scale;
    int ysize=(bb.end.y - bb.begin.y)*scale;
    if (xsize*ysize > 0){
      iftAdjRel *B = iftRectangular(xsize,ysize);
      iftAdjRel *C = iftAdjacencyBoundaries(B,A);    
      iftDestroyAdjRel(&B);
      for (int i=1; i < C->n; i++) {
	iftVoxel v = iftGetAdjacentVoxel(C,center,i);
	if (iftValidVoxel(box,v)){
	  int q       = iftGetVoxelIndex(box,v);
	  box->val[q] = YCbCr.val[0];
	  box->Cb[q]  = YCbCr.val[1];
	  box->Cr[q]  = YCbCr.val[2];
	}
      }
      iftDestroyAdjRel(&C);
    }
  }
  iftDestroyAdjRel(&A);
  iftDestroyImage(&label);
  
  return(box);
}

int main(int argc, char *argv[])
{

  /* Example: detection salie 1 boxes */
  
  if (argc!=4){ 
    iftError("Usage: detection <P1> <P2> <P3>\n"
	     "[1] folder with the salience maps\n"
	     "[2] layer (1,2,...) to create the results\n"
	     "[3] output folder with the resulting bounding boxes\n",	 
	     "main");
  }
  
  timer *tstart = iftTic();

  char *filename     = iftAllocCharArray(512);
  int layer          = atoi(argv[2]);
  char suffix[12];
  sprintf(suffix,"_layer%d.png",layer);
  iftFileSet *fs     = iftLoadFileSetFromDirBySuffix(argv[1], suffix, true);
  char *output_dir   = argv[3];
  iftMakeDir(output_dir);
  iftColor RGB, YCbCr;

  for(int i = 0; i < fs->n; i++) {
    printf("Processing image %d of %ld\r", i + 1, fs->n);
    char *basename1   = iftFilename(fs->files[i]->path,suffix);      
    char *basename2   = iftFilename(fs->files[i]->path,".png");      
    iftImage *salie   = iftReadImageByExt(fs->files[i]->path);
    sprintf(filename,"./truelabels/%s.png",basename1);
    iftImage *gt      = iftReadImageByExt(filename);
    sprintf(filename,"./images/%s.png",basename1);
    iftImage *orig    = iftReadImageByExt(filename);
    int Imax          = iftNormalizationValue(iftMaximumValue(orig));
 
    /* Detect parasite */

    iftAdjRel *A   = iftCircular(1.5);
    iftImage *bin  = iftThreshold(salie,iftOtsu(salie),IFT_INFINITY_INT,255);
    iftImage *comp = iftSelectCompInAreaInterval(bin, A, 400, 4000);

    RGB.val[0]     = Imax;
    RGB.val[1]     = 0;
    RGB.val[2]     = Imax;
    YCbCr          = iftRGBtoYCbCr(RGB,Imax);
    iftImage *img1 = DrawBoxes(orig, comp, YCbCr, 1.5);
    RGB.val[0]     = 0;
    RGB.val[1]     = Imax;
    RGB.val[2]     = Imax; 
    YCbCr          = iftRGBtoYCbCr(RGB,Imax);
    iftImage *img2 = DrawBoxes(img1, gt, YCbCr, 1.5);

    iftDestroyImage(&bin);
    iftDestroyImage(&comp);
    iftDestroyAdjRel(&A);
    iftDestroyImage(&salie);
    iftDestroyImage(&gt);
    iftDestroyImage(&orig);
    iftDestroyImage(&img1);

    /* save resulting image */

    sprintf(filename,"%s/%s.png",output_dir,basename2);
    iftWriteImageByExt(img2,filename);

    iftDestroyImage(&img2);
    iftFree(basename1);
    iftFree(basename2);
  }
  
  iftFree(filename);
  iftDestroyFileSet(&fs);
  
  printf("\nDone ... %s\n", iftFormattedTime(iftCompTime(tstart, iftToc())));
  
  return (0);
}
