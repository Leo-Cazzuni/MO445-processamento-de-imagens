#include "ift.h"

#define truelabel_thres 0.90

void SaveResults(char *dir, char *basename, const char *ext, iftImage *img);

char *RemovePrefixOrig(char *basename)
{
  iftSList *list = iftSplitString(basename,"_");
  iftSNode *L    = list->tail;
  return(iftCopyString(L->elem));
}

void SaveResults(char *dir, char *basename, const char *ext, iftImage *img)
{
    char filename[200];
        
    sprintf(filename,"%s/%s%s",dir,basename,ext);
    iftWriteImageByExt(img,filename);
}

int main(int argc, char *argv[]) 
{
    timer *tstart = iftTic();

    if (argc!=6){
      printf("Usage: tiledataset ...\n");
      printf("[P1]:  input csv file with 2D images\n");
      printf("[P2]:  stride in x and y: ''stx sty'' \n");
      printf("[P3]:  cropped region size: ''xsize ysize'' \n");
      printf("[P4]:  output region size: ''xsize ysize''\n");
      printf("[P5]:  output directory with the tile dataset\n");
      exit(-1);
    }

    iftFileSet *fs_input = iftLoadFileSetFromCSV(argv[1], false); 
    int stx, sty;
    sscanf(argv[2],"%d %d",&stx,&sty);
    int xsize, ysize;
    sscanf(argv[3],"%d %d",&xsize,&ysize);
    int out_xsize, out_ysize;
    sscanf(argv[4],"%d %d",&out_xsize,&out_ysize);
    
    int nimages = fs_input->n;    
    iftMakeDir(argv[5]);
    
    for (int i=0; i < nimages; i++) {
      
      const char *ext = iftFileExt(fs_input->files[i]->path);
      char *basename  = iftFilename(fs_input->files[i]->path,ext);
      char *basename_aux = RemovePrefixOrig(basename);
      iftFree(basename);
      basename = basename_aux;

      printf("Processing file %s: %d of %d files\r", basename, i + 1, nimages);
      fflush(stdout);

      
      iftImage *input  = iftReadImageByExt(fs_input->files[i]->path);
      
      /* Extract tile at (x,y) */
      for (int y=ysize/2; y < input->ysize-ysize/2-1; y += sty){
	for (int x=xsize/2; x < input->xsize-xsize/2-1; x += stx){
	  iftBoundingBox  bb; 
	  iftImage       *roi, *tile;

	  bb.begin.x  = x - xsize/2; bb.begin.y  = y - ysize/2; bb.begin.z  = 0;
	  bb.end.x    = x + xsize/2 - 1; bb.end.y    = y + ysize/2 - 1; bb.end.z    = 0;
	  
	  roi         = iftExtractROI(input, bb);
	  
	  if ((out_xsize != xsize)||(out_ysize != ysize)){
	    float sx    = ((float) out_xsize) / (float)roi->xsize;
	    float sy    = ((float) out_ysize) / (float)roi->ysize;
	    tile        = iftInterp2D(roi,sx,sy);
	    iftDestroyImage(&roi);
	  }else{
	    tile = roi;
	  }
	  char out_basename[200];
	  sprintf(out_basename,"%s_%03d_%03d",basename,x,y);
	  SaveResults(argv[5], out_basename, ext, tile);
	  iftDestroyImage(&tile);
	}
      }
      iftFree(basename);
      iftDestroyImage(&input);
    }

    iftDestroyFileSet(&fs_input);
    
    printf("Done ... %s\n", iftFormattedTime(iftCompTime(tstart, iftToc())));

    return(0);
}



