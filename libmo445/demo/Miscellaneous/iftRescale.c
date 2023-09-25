#include "ift.h"

void MSaveResults(char *dir, char *basename, iftMImage *mimg)
{
    char filename[200];

    sprintf(filename, "%s/%s.mimg", dir, basename);
    iftWriteMImage(mimg, filename);
}

void SaveResults(char *dir, char *basename, iftImage *img, const char *ext)
{
    char filename[200];

    sprintf(filename, "%s/%s%s", dir, basename, ext);
    iftWriteImageByExt(img, filename);
}

int main(int argc, char *argv[])
{
    timer *tstart = NULL;

    if (argc != 6)
        iftError("Usage: iftRescale <...>\n"
                "[1] input: folder/csv file with images (.mimg, .png., .nii.gz)\n"
		 "[2] new size in x\n"
		 "[3] new size in y\n"
		 "[4] new size in z\n"
                "[5] output: Output folder with the rescaled images\n",
                "main");

    tstart = iftTic();

    iftFileSet *fs = iftLoadFileSetFromDirOrCSV(argv[1],1,1);
    float xsize = atof(argv[2]), sx;
    float ysize = atof(argv[3]), sy;
    float zsize = atof(argv[4]), sz;
    
    iftMakeDir(argv[5]);

    int first = 0;
    int last = fs->n - 1;

    for (int i = first; i <= last; i++)
    {
      const char *ext = iftFileExt(fs->files[i]->path);
      char *basename  = iftFilename(fs->files[i]->path,ext);
      printf("Processing file %s: %d of %ld files\r", basename, i + 1, fs->n);
      fflush(stdout);
      if (iftCompareStrings(ext,".mimg")){
	iftMImage *mimg   = iftReadMImage(fs->files[i]->path);
	iftMImage *interp = NULL;
	sx = xsize/mimg->xsize;
	sy = ysize/mimg->ysize;
	if (iftIs3DMImage(mimg)){
	  sz     = zsize/mimg->zsize;
	  interp = iftMInterp(mimg,sx,sy,sz);
	}else{
	  interp = iftMInterp2D(mimg,sx,sy);
	}
	MSaveResults(argv[5], basename, interp);
	iftDestroyMImage(&mimg);
	iftDestroyMImage(&interp);
      } else {
	iftImage *img    = iftReadImageByExt(fs->files[i]->path);
	iftImage *interp = NULL;
	sx = xsize/img->xsize;
	sy = ysize/img->ysize;
	if (iftIs3DImage(img)){
	  sz     = zsize/img->zsize;
	  interp = iftInterp(img,sx,sy,sz);
	}else{
	  interp = iftInterp2D(img,sx,sy);
	}
	SaveResults(argv[5], basename, interp, ext);
	iftDestroyImage(&img);
	iftDestroyImage(&interp);
      }
      
      iftFree(basename);
    }
    
    printf("\n");
    printf("Done ... %s\n", iftFormattedTime(iftCompTime(tstart, iftToc())));

    return (0);
}
