#include "ift.h"


int main(int argc, const char **argv) {
  iftImage *img = NULL, *roi = NULL;
  iftBoundingBox bb;

  if (argc != 9) {
    iftError("iftRegionOfInterest <input-image.scn> <xmin> <ymin> <zmin> <xmax> <ymax> <zmax> <output-roi.scn>","iftRegionOfInterest");
  }

  img = iftReadImageByExt(argv[1]);
  bb.begin.x = atoi(argv[2]); 
  bb.begin.y = atoi(argv[3]);
  bb.begin.z = atoi(argv[4]);
  bb.end.x   = atoi(argv[5]);
  bb.end.y   = atoi(argv[6]);
  bb.end.z   = atoi(argv[7]);
  roi = iftExtractROI(img,bb); 

  iftWriteImage(roi,argv[8]);

  iftDestroyImage(&img);
  iftDestroyImage(&roi);

  return 0;
}
