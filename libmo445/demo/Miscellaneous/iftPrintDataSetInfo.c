//
// Created by Cesar Castelo on Mar 26, 2018
//

#include <ift.h>

int main(int argc, char** argv)  {

    if(argc != 3) {
        iftError("\nUsage: iftPrintDataSetInfo <...>\n"
                         "[1] dataset: input dataset\n"
                         "[2] type 1 to print data statistics or 0 to omit it\n",
                 "iftPrintDataSetInfo.c");
    }

    iftDataSet *Z  = iftReadDataSet(argv[1]);

    iftPrintDataSetInfo(Z, (bool)atoi(argv[2]));
  
    
    iftDestroyDataSet(&Z);

    return 0;
}
