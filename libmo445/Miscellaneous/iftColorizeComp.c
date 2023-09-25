#include "ift.h"


int main(int argc, const char *argv[]) {

  if (argc != 3){
    iftError("iftColorizeComp label.* result.*","main");
  }

  iftImage *label = iftReadImageByExt(argv[1]);
  iftImage *res   = iftColorizeComp(label);

  iftWriteImageByExt(res,argv[2]);
    
  iftDestroyImage(&label);
  iftDestroyImage(&res);
		    
  return 0;
}

