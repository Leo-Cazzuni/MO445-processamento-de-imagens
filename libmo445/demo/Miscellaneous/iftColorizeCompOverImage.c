#include "ift.h"


int main(int argc, const char *argv[]) {

  if (argc != 4){
    iftError("iftColorizeCompOverImage image.* label.* result.*","main");
  }

  iftImage *image = iftReadImageByExt(argv[1]);
  iftImage *label = iftReadImageByExt(argv[2]);
  iftImage *res   = iftColorizeCompOverImage(image,label);

  iftWriteImageByExt(res,argv[3]);

  iftDestroyImage(&image);
  iftDestroyImage(&label);
  iftDestroyImage(&res);
		    
  return 0;
}

