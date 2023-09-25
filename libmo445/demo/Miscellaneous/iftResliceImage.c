#include "ift.h"

void iftGetInputParameters(int argc, const char *argv[], char **input_img_file, char **output_img_file, int *dx, int *dy, int *dz, iftAxisOrder *axis_order)
{
  // This array of options, n_opts, and the program description could be defined out of the main as a global variables
  // or, we could create a function in this program file to perform the command line parser setup
  iftCmdLineOpt cmd_line_opts[] = {
    {.short_name = "-i", .long_name = "--input-img", .has_arg=true, .required=true, .arg_type=IFT_STR_TYPE, .help="Input image for reslicing"},
    {.short_name = "-x", .long_name = "--inc_x", .has_arg=true, .arg_type=IFT_LONG_TYPE, .required=true, .help="Increment in {-1,1} along axis x"},
    {.short_name = "-y", .long_name = "--inc_y", .has_arg=true, .arg_type=IFT_LONG_TYPE, .required=true, .help="Increment in {-1,1} along axis y"},
    {.short_name = "-z", .long_name = "--inc_z", .has_arg=true, .arg_type=IFT_LONG_TYPE, .required=true, .help="Increment in {-1,1} along axis z"},
    {.short_name = "-a", .long_name = "--axis_order", .has_arg=true, .arg_type=IFT_STR_TYPE, .required=true, .help="Voxel access order in {XYZ, XZY, YXZ, YZX, ZXY, ZYX} among axes"},
    {.short_name = "-o", .long_name = "--output-img", .has_arg=true, .arg_type=IFT_STR_TYPE, .required=true, .help="Output image"},
  };
  int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);
  char program_description[2048] = "It reslices the input image by following a given voxel access order \n and with increments dx, dy, and dz along the main axes x, y, and z."; 
  
  // Parser Setup
  iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
  iftDict *args = iftParseCmdLine(argc, argv, parser); // getting the
						       // passed
						       // options/arguments
  
  *input_img_file  = iftGetStrValFromDict("-i", args); 
  *output_img_file = iftGetStrValFromDict("-o", args); 
  *dx              = iftGetLongValFromDict("-x",args);
  *dy              = iftGetLongValFromDict("-y",args);
  *dz              = iftGetLongValFromDict("-z",args);
  char *value      = iftGetStrValFromDict("-a", args);

  if (abs((*dx)*(*dy)*(*dz))!=1)
    iftPrintUsage(parser);

  if (iftCompareStrings(value,"XYZ")){
    *axis_order = XYZ;
  }else{
    if (iftCompareStrings(value,"XZY")){
      *axis_order = XZY;
    }else{
      if (iftCompareStrings(value,"YXZ")){
	*axis_order = YXZ;
      }else
	if (iftCompareStrings(value,"YZX")){
	  *axis_order = YZX;
	}else{
	  if (iftCompareStrings(value,"ZXY")){
	    *axis_order = ZXY;
	  }else{
	    if (iftCompareStrings(value,"ZYX")){
	      *axis_order = ZYX;
	    }else{
	      iftPrintUsage(parser);
	    }
	  }
	}
    }
  }

  iftDestroyCmdLineParser(&parser);
  iftDestroyDict(&args);
  free(value);

}


int main(int argc, const char *argv[]) 
{
  iftImage    *img=NULL, *rimg=NULL;
  char        *input_img_file,*output_img_file;
  int          dx, dy, dz;
  iftAxisOrder axis_order;
  size_t       MemDinInitial, MemDinFinal;
  timer       *t1=NULL,*t2=NULL;
  
  /* ------------------------------------------------------- */

  MemDinInitial = iftMemoryUsed();

  /* ------------------------------------------------------- */

  iftGetInputParameters(argc, argv, &input_img_file, &output_img_file,&dx,&dy,&dz,&axis_order);

  t1 = iftTic(); 
  
  img  = iftReadImageByExt(input_img_file);
  rimg = iftResliceImageSimple(img,dx,dy,dz,axis_order);
  iftDestroyImage(&img);

  t2 = iftToc();

  iftWriteImageByExt(rimg,output_img_file);
  iftDestroyImage(&rimg);
  free(input_img_file);
  free(output_img_file);

  fprintf(stdout,"Image resliced in %f ms\n",iftCompTime(t1,t2));

  /* ------------------------------------------------------ */

  MemDinFinal = iftMemoryUsed();
  
  iftVerifyMemory(MemDinInitial,MemDinFinal);

  /* ------------------------------------------------------- */

  return(0);
}
