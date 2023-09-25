#include "ift.h"

/************* HEADERS ****************/
void ValidateInputs(iftFile *out_file);
/**************************************/




/************* MAIN ****************/
int main(int argc, char **argv) {
    if ((argc < 3) || (argc > 4))
        iftError("To run: iftDicom2Scene <dicom_directory> <dir/filename.[scn, scn.gz, zscn]> <OPTIONAL - voxel size (e.g., d=1.25 => dx=dy=dz=1.25)>\n\n" \
            "- It works for <dicom_directory> with or without / in the end\n" \
            "- If the parent_dir <dir> from the filename.scn does not exist,\n" \
            " then the program creates it.\n" \
            "- REQUIRED: libgdcm-tools", "main");


    char dir_pathname[512]; strcpy(dir_pathname, argv[1]);
    char out_filename[512]; strcpy(out_filename, argv[2]);
    timer *t1, *t2;

    iftFile *out_file = iftCreateFile(out_filename);
    ValidateInputs(out_file);

    t1 = iftTic();
    //The function checks if the dir exists.
    printf("- Reading Dicom slices (2D images)\n");
    iftDicomImage * dicom = iftReadDicomImage(dir_pathname); 
    //iftPrintDicomInfo(dicom);
    //printf("dicom dx %f, dy %f, dz %f\n",dicom->dx,dicom->dy,dicom->dz);
    
    puts("--> Converting Dicom to Scene");
    iftImage *img = iftConvertDicom2Scene(dicom);
    printf("img dx %f, dy %f, dz %f\n",img->dx,img->dy,img->dz);
    

   //Transform the obtained image to the correct coordinate system,
   //so that the images originated from the dicom format can always be 
   //visualized the same way, and stores the results in "new_img".
   //iftImage *new_img = img;

   iftImage *new_img = iftConvertDicomCoordinates(img, dicom);
   iftDestroyImage(&img);
   iftDestroyDicomImage(&dicom);

   if (argc == 4) {
       float d = atof(argv[3]);
       printf("new_img dx %f, dy %f, dz %f\n",new_img->dx,new_img->dy,new_img->dz);
       printf("d = %f\n",d);
       if (d > 0.0) {
           img = iftInterp(new_img, new_img->dx / d, new_img->dy / d, new_img->dz / d);
           iftWriteImageByExt(img, out_file->path);
           iftDestroyImage(&img);
       } else {
           iftWriteImageByExt(new_img, out_file->path);
       }
   } else if (new_img->dz > 0){
       printf("new_img dx %f, dy %f, dz %f\n",new_img->dx,new_img->dy,new_img->dz);
       float d = new_img->dx;
       if (d > new_img->dy)
           d = new_img->dy;
       if (d > new_img->dz)
           d = new_img->dz;
       img = iftInterp(new_img, new_img->dx / d, new_img->dy / d, new_img->dz / d);
       iftWriteImageByExt(img, out_file->path);
       iftDestroyImage(&img);
   } else
       iftWriteImageByExt(new_img, out_file->path);

   iftDestroyImage(&new_img);
   iftDestroyFile(&out_file);
   
   t2 = iftToc();
   fprintf(stdout, "\nTime elapsed %f sec\n", iftCompTime(t1, t2) / 1000);
    
    puts("Done...");
	return 0;
}
/**************************************/






/************* BODIES ****************/
void ValidateInputs(iftFile *out_file) {
	//char msg[512];

    /*
    // Check if the output filename has the extension .scn
	if (!iftEndsWith(out_file->path, "scn")) {
		sprintf(msg, "-> Extension is not .scn - \"%s\"", out_file->path);
		iftError(msg, "ValidateInputs");
    }
     */

    // Check if the Output Directory exists
    char *parent_dir = iftParentDir(out_file->path);

    if (!iftDirExists(parent_dir)) {
        printf("--> Creating the Output Directory: \"%s\"\n", parent_dir);
        iftMakeDir(parent_dir);
    }
}
/**************************************/








