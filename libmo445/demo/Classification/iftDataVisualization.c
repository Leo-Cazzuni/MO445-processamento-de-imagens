#include "ift.h"

int main(int argc, char *argv[])
{
    iftDataSet      *Z,*Z2d;
    iftImage        *img;

    if (argc != 6)
        iftError("Usage: iftDataVisualization <input-dataset.zip> <perplexity> <max_iter> <input-option (0-WEIGHT, 1-LABEL, 2-CLASS, 3-GROUP, 4-POINT)> <output-image.[pgn,ppm]>", "main");

    Z    = iftReadDataSet(argv[1]);

    
    if ((Z->nclasses == 0)&&((atoi(argv[4])==1)||(atoi(argv[4])==2)))
      iftError("Dataset has no classes","iftDataVisualization"); 
    
    if ((Z->ngroups == 0)&&(atoi(argv[4])==3))
      iftError("Dataset has no groups","iftDataVisualization"); 

    printf("Total number of samples  %d\n",Z->nsamples);
    printf("Total number of features %d\n",Z->nfeats);
    printf("Total number of classes  %d\n",Z->nclasses);
    printf("Total number of groups  %d\n",Z->ngroups);

    double perplexity = atof(argv[2]);
    int max_iter = atoi(argv[3]);

    if(Z->nfeats == 2)
        Z2d = iftCopyDataSet(Z, true);
    else
        Z2d  = iftDimReductionByTSNE(Z, 2, perplexity, max_iter);

   
    img  = iftDraw2DFeatureSpace(Z2d,atoi(argv[4]),IFT_ALL);
    iftWriteImageByExt(img,argv[5]);
    iftDestroyImage(&img);
    iftDestroyDataSet(&Z2d);
    iftDestroyDataSet(&Z);

    return(0);
}
