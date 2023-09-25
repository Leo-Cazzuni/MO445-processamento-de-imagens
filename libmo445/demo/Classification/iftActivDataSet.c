#include "ift.h"

iftMImage *CropMImageBorder(iftMImage *img, int border_size)
{
  iftVoxel u;
  iftMImage *crop;
  
  if (iftIs3DMImage(img)){  
    crop  = iftCreateMImage(img->xsize-2*border_size,img->ysize-2*border_size,img->zsize-2*border_size, img->m);
    int q = 0;
    for (u.z = border_size; u.z < img->zsize-border_size; u.z++) 
      for (u.y = border_size; u.y < img->ysize-border_size; u.y++) 
	for (u.x = border_size; u.x < img->xsize-border_size; u.x++) {
	  int p = iftMGetVoxelIndex(img,u);
	  for (int b=0; b < img->m; b++)	    
	    crop->val[q][b] = img->val[p][b];
	  q++;
	}
  }else { 
    crop  = iftCreateMImage(img->xsize-2*border_size,img->ysize-2*border_size,img->zsize, img->m);
    int q = 0;
    u.z   = 0;
    for (u.y = border_size; u.y < img->ysize-border_size; u.y++) 
      for (u.x = border_size; u.x < img->xsize-border_size; u.x++) {
	int p = iftMGetVoxelIndex(img,u);
	for (int b=0; b < img->m; b++)	    
	  crop->val[q][b] = img->val[p][b];
	q++;
      }
  }

  return(crop);
}


void iftCreateImgRefData(char *origImgsDir, char *imgBasename, char *origImgExt, iftFileSet *fsRefData, int fileId)
{
    char filename[2048];
    sprintf(filename, "%s/%s%s", origImgsDir, imgBasename, origImgExt);
    fsRefData->files[fileId] = iftCreateFile(filename);
}

void iftWriteKernelDataSetWithFileSet(iftDataSet *Z, iftFileSet *fs, iftSampleStatus status, char *outBasename)
{
    iftSetRefData(Z, fs, IFT_REF_DATA_FILESET);
    iftSetStatus(Z, status);
    if (Z->nclasses > 0) 
      iftAddStatus(Z, IFT_SUPERVISED);
    char filename[2048];
    sprintf(filename, "%s.zip", outBasename);
    iftWriteDataSet(Z, filename);
    printf("File '%s' created\n", filename);
}

int main(int argc, char *argv[])
{
    timer *tstart = NULL;

    if (argc != 5)
        iftError("Usage: iftActivDataSet <...>\n"
                 "[1] input_images: Input directory with activation mimages\n"
                 "[2] path of the original images\n"
                 "[3] output_status: Status for the output dataset (1: Train, 2: Test)\n"
                 "[4] border size to be cropped (e.g., 5, 10)\n",
                 "main");

    tstart = iftTic();

    iftFileSet *fs            = iftLoadFileSetFromDirBySuffix(argv[1],".mimg", 1);
    iftSampleStatus outStatus = atoi(argv[3]) == 1 ? IFT_TRAIN : IFT_TEST;
    int border_size           = atoi(argv[4]);

    char *origImgsDir = NULL, *origImgExt = NULL;
    iftFileSet *fsAux = iftLoadFileSetFromDirOrCSV(argv[2], 1, 1);
    origImgsDir       = iftDirname(fsAux->files[0]->path);
    origImgExt        = iftCopyString(iftFileExt(fsAux->files[0]->path));
    printf("%s %s\n",origImgsDir,origImgExt);
    
    iftDestroyFileSet(&fsAux);

    int first    = 0;
    int last     = fs->n - 1;
    int nclasses = iftFileSetLabelsNumber(fs);
    char *outBasename = iftBasename(argv[1]);
    iftRightTrim(outBasename, '/');

    iftMImage *img0 = iftReadMImage(fs->files[0]->path);
    int nvoxels = 0;
    if (iftIs3DMImage(img0))
      nvoxels = (img0->xsize-2*border_size)*(img0->ysize-2*border_size)*(img0->zsize-2*border_size);
    else
      nvoxels = (img0->xsize-2*border_size)*(img0->ysize-2*border_size);      
    int nfeats      = (nvoxels * img0->m);
    iftDestroyMImage(&img0);

    iftDataSet *Z         = iftCreateDataSet(fs->n, nfeats);
    iftFileSet *fsRefData = iftCreateFileSet(fs->n);
    Z->nclasses = nclasses;

    char filename[200];
    sprintf(filename,"%s_id_image.txt",argv[1]);
    FILE *fp = fopen(filename,"w");
    for (int i = first; i <= last; i++) {
        fprintf(fp,"id: %d image: %s\n", i, fs->files[i]->path);
    }
    fclose(fp);
    
    for (int i = first; i <= last; i++) {
        char *basename = iftFilename(fs->files[i]->path, ".mimg");
        printf("Processing file %s: %d of %ld files\r", basename, i + 1, fs->n); fflush(stdout);
        iftMImage *mimg = iftReadMImage(fs->files[i]->path);
	
	if (border_size != 0){
	  iftMImage *crop = CropMImageBorder(mimg,border_size);
	  iftDestroyMImage(&mimg);
	  mimg = crop;
	}
	
	Z->sample[i].id = i;
        Z->sample[i].truelabel = fs->files[i]->label;
	int j = 0;
        for (int p = 0; p < mimg->n; p++) {
            for (int b = 0; b < mimg->m; b++) {
                Z->sample[i].feat[j] = mimg->val[p][b];
                j++;
            }
        }
        iftCreateImgRefData(origImgsDir, basename, origImgExt, fsRefData, i);
        iftDestroyMImage(&mimg);
    }
    printf("\n");

    /* write the dataset and create the projection */
    iftWriteKernelDataSetWithFileSet(Z, fsRefData, outStatus, outBasename);

    iftDestroyFileSet(&fs);
    iftDestroyFileSet(&fsRefData);
    iftDestroyDataSet(&Z);

    printf("Done ... %s\n", iftFormattedTime(iftCompTime(tstart, iftToc())));

    return (0);
}
