#include "ift.h"

int main(int argc, char *argv[]){

    if (argc != 4)
        iftError("iftImageSet2Scene <image folder> <prefix> <out.scn>","iftImageSet2Scene");

    char *image_dir = argv[1];
    char *prefix = argv[2];
    char *out_path = argv[3];
    char regex[100];
    iftImage *referenceDomain, *img, *out;
    int n,p,z;
    timer *t1, *t2;

    puts("\n--------------------------------------------");
    puts("Note: The files must have a prefix followed by a number representing its slide in"\
                 "the volume. Also, the list of images must be sorted by the slice number");
    puts("For example: IM0001.png, the prefix is IM and the slice number is 0001");
    puts("--------------------------------------------\n\n");

    t1 = iftTic();
    puts("- Loading files from Dir");
    sprintf(regex,".%s.*\\.png",prefix);
    iftFileSet *fileSet = iftLoadFileSetFromDirByRegex(image_dir,regex);
    if (fileSet == NULL){
        iftError("Unable to load files.","iftImageSet2Scene");
    }

    referenceDomain = iftReadImageByExt(fileSet->files[0]->path);
    out = iftCreateImage(referenceDomain->xsize,referenceDomain->ysize,fileSet->n);

    puts("- Creating Volume");
    z = 0;
    for (n=fileSet->n-1; n > 0; n--) {
        printf("--- %s\n",fileSet->files[n]->path);
        img = iftReadImageByExt(fileSet->files[n]->path);
        if (!(iftIsDomainEqual(referenceDomain,img)))
            iftError("Images must have the same domain","iftImageSet2Scene");
        for (p = 0; p < img->n; p++){
            iftVoxel u,v;
            u = iftGetVoxelCoord(img,p);
            if (iftValidVoxel(img,u)){
                v.x = u.x;
                v.y = u.y;
                v.z = z;
                int q = iftGetVoxelIndex(out,v);
                out->val[q] = img->val[p];
            }
        }
        z++;
        iftDestroyImage(&img);
    }

    iftDestroyImage(&referenceDomain);

    iftImage *norm = iftNormalizeWithNoOutliers(out,0,4095,0.98);
    iftDestroyImage(&out);

    iftWriteImage(norm,out_path);
    iftDestroyImage(&norm);

    t2 = iftToc();
    fprintf(stdout, "\nTime elapsed %f sec\n", iftCompTime(t1, t2) / 1000);

    return 0;
}