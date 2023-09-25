iftColorTable *iftCreateColorTable(int n_colors, iftColorSpace cspace) {
    if (n_colors <= 0)
        iftExit("Invalid num of colors: %d <= 0", "iftCreateColorTable", n_colors);

    const int max_val = 255;

    iftColorTable *ctb = (iftColorTable*) iftAlloc(1, sizeof(iftColorTable));
    ctb->ncolors       = n_colors;
    ctb->color         = (iftColor*) iftAlloc(n_colors, sizeof(iftColor));
    if (ctb->color == NULL)
        iftExit("Cannot allocate Color Table", "iftCreateColorTable");

    // gets the RED color 
    if (n_colors == 1) {
        ctb->color[0].val[0] = 255;
        ctb->color[0].val[1] = 0;
        ctb->color[0].val[2] = 0;
    }
    else {
        // number of tickets including the 0-tick
        int n_ticks      = ceil(powf(n_colors, 0.33333)); // cubic root
        int n_max_colors = pow(n_ticks, 3) - 1; // excluding the (0,0,0)
        if (n_max_colors < n_colors) {
            n_ticks++;
            n_max_colors = pow(n_ticks, 3) - 1; // excluding the (0,0,0)
        }
        int step = (int) max_val / (n_ticks-1); 

        iftIntArray *ticks = iftCreateIntArray(n_ticks-1); // excluding the 0-tick
        ticks->val[0] = 255;

        int *inter_ticks = iftAllocIntArray(n_ticks-2);

        // gets the remaining ticks by stepping - it excludes the 0-tick and 255-tick
        for (int i = 1; i < (n_ticks-1); i++)
            inter_ticks[i-1] = i * step;

        // in order to spread the intermediate ticks in a good order to generate different colors,
        // it arranges the intermediate color ticks by following the BFS order
        iftBFSColorTicks(inter_ticks, n_ticks-2, ticks);
        iftFree(inter_ticks);
        
        // all possible colors using <n_ticks>
        iftColor *all_colors = iftGetAllColors(n_max_colors, ticks);

        // generates the color table
        for (int c = 0; c < ctb->ncolors; c++)
            ctb->color[c] = all_colors[c];

        // iftFree(colors);
        iftDestroyIntArray(&ticks);
        iftFree(all_colors);
    }

    switch (cspace) {
        case RGB_CSPACE:
            break;
        case YCbCr_CSPACE:
            for (int c = 0; c < ctb->ncolors; c++) {
                iftColor YCbCr = iftRGBtoYCbCr(ctb->color[c], max_val);
                ctb->color[c].val[0] = YCbCr.val[0];
                ctb->color[c].val[1] = YCbCr.val[1];
                ctb->color[c].val[2] = YCbCr.val[2];
            }
            break;
        default:
            iftExit("Color Space not supported yet", "iftCreateColorTable");
    }

    return ctb;
}



iftCurve *iftSkelSaliences(iftImage *skel) 
{  
  iftImage  *dist=NULL,*root=NULL,*size=NULL;
  iftGQueue *Q=NULL;
  int        i,p,q,tmp,npts;
  iftVoxel   u,v,r;
  iftAdjRel *A=NULL;
  iftCurve  *curve=NULL;
  float      salie_thres; /* In 3D, the size of the influence zone of
	     a point is a volume and, in 2D, it is an area. In 2D,
	     area = (theta x radius^2)/2, and in 3D, volume = (theta x
	     2 x radius^3)/3, where 0 <= theta <= 2xPI is the aperture
	     angle. We are seeking salience points with angle above 70
	     degrees, theta >= (70.0xPI)/180.0, with a radius=10. This
	     gives us a salience threshold of (70xPI)/180 x 50 in 2D
	     and (70xPI)/180 x (2000/3) in 3D. */

  /* Propagate the skeleton points up to a small squared distance of
     100 (10 voxels) from the skeleton */

  if (iftIs3DImage(skel)){ // 3D
    A = iftSpheric(sqrtf(3.0));
    salie_thres = (70.0*PI/180.0)*(2000.0/3.0);
  }else{ // 2D
    A = iftCircular(sqrtf(2.0));
    salie_thres = (70.0*PI/180.0)*50.0;
  }

  dist   = iftCreateImage(skel->xsize,skel->ysize,skel->zsize);
  root   = iftCreateImage(skel->xsize,skel->ysize,skel->zsize);
  size   = iftCreateImage(skel->xsize,skel->ysize,skel->zsize);
  Q      = iftCreateGQueue(QSIZE,skel->n,dist->val);

  for (p=0; p < skel->n; p++) {
    dist->val[p]=INFINITY_INT;
    if (skel->val[p] != 0){ /* skeleton point */
      dist->val[p]=0;
      root->val[p]=p;
      size->val[p]=0;
      iftInsertGQueue(&Q,p);
    }
  }

  /* Image Foresting Transform for a limited propagation of the
     influence zones of the skeleton points */

  while(!iftEmptyGQueue(Q)) {
    p=iftRemoveGQueue(Q);

    if (dist->val[p] <= 100){

      size->val[root->val[p]] += 1;

      u = iftGetVoxelCoord(skel,p);
      r = iftGetVoxelCoord(skel,root->val[p]);
      
      for (i=1; i < A->n; i++){
	v = iftGetAdjacentVoxel(A,u,i);
	if (iftValidVoxel(skel,v)){
	  q = iftGetVoxelIndex(skel,v);
	  if (dist->val[q] > dist->val[p]){
	    tmp = iftSquaredVoxelDistance(v,r);
	    if (tmp < dist->val[q]){
	      if (dist->val[q]!=INFINITY_INT)
		iftRemoveGQueueElem(Q, q);
	      dist->val[q]  = tmp;
	      root->val[q]  = root->val[p];
	      iftInsertGQueue(&Q, q);
	    }
	  }
	}
      }
    }
  }

  /* Detect salience points above 70 degrees of aperture angle */

  npts=0; 
  for (p=0; p < skel->n; p++) 
    if (size->val[p]>=salie_thres) 
      npts++;

  curve = iftCreateCurve(npts);
  i     = 0; 
  for (p=0; p < skel->n; p++) 
    if (size->val[p]>=salie_thres){
      u = iftGetVoxelCoord(skel,p);
      curve->pt[i].x = u.x;
      curve->pt[i].y = u.y;
      curve->pt[i].z = u.z;
      i++;
    }

  iftDestroyAdjRel(&A);
  iftDestroyGQueue(&Q);
  iftDestroyImage(&root);
  iftDestroyImage(&dist);
  iftDestroyImage(&size);

  return(curve);
}

float iftUnderSegmentationSLIC2(iftImage *gt_image, iftImage *label_image, float tol_per)
{
  iftVerifyImageDomains(gt_image, label_image, "iftUnderSegmentationSLIC2");
  int i,j,p,numObj,numRegions;
  numObj = iftMaximumValue(gt_image) + 1;
  numRegions = iftMaximumValue(label_image);
  
  int *pixGT = iftAllocIntArray(numObj);
  for(p = 0; p < gt_image->n; p++)
    pixGT[gt_image->val[p]]++;

	int *pixTotal = iftAllocIntArray(numRegions);
	for(p = 0; p < label_image->n; p++)
		pixTotal[label_image->val[p] - 1]++;

	int *pixObj = iftAllocIntArray(numRegions);

	int sumObj = 0;
	for(j = 0; j < numObj; j++){
		for(p = 0; p < label_image->n; p++){
			if(gt_image->val[p] == j){
				pixObj[label_image->val[p]-1]++;
			}
		}
		for(i = 0; i < numRegions; i++){
			if(pixObj[i] > 0) {
				float per = (float)pixTotal[i]/(float)pixGT[j];

				if(per >= tol_per){
					sumObj = sumObj + pixTotal[i];
				}
			}
			pixObj[i] = 0;
		}
	}

	float totalErr = ((float)(sumObj - label_image->n)/(float)label_image->n);

	iftFree(pixGT);
	iftFree(pixTotal);
	iftFree(pixObj);

	return totalErr;
}

float iftUnderSegmentationTurbovoxel(iftImage *gt_image, iftImage *label_image)
{
  iftVerifyImageDomains(gt_image, label_image, "iftUnderSegmentationTurbovoxel");
  int i,j,p,numObj,numRegions;
  numObj = iftMaximumValue(gt_image) + 1;
  numRegions = iftMaximumValue(label_image);
  
  int *pixGT = iftAllocIntArray(numObj);
  for(p = 0; p < gt_image->n; p++)
    pixGT[gt_image->val[p]]++;

  int *pixTotal = iftAllocIntArray(numRegions);
  for(p = 0; p < label_image->n; p++)
    pixTotal[label_image->val[p] - 1]++;
  
  int *pixObj = iftAllocIntArray(numRegions);
  
  int sumErr = 0;
  for(j = 0; j < numObj; j++){
    for(p = 0; p < label_image->n; p++){
      if(gt_image->val[p] == j){
	pixObj[label_image->val[p]-1]++;
      }
    }
    int sumCovering = 0;
    for(i = 0; i < numRegions; i++){
      if(pixObj[i] > 0) {
	sumCovering = sumCovering + pixTotal[i];
      }
      pixObj[i] = 0;
    }
    sumErr = sumErr + (float)(sumCovering - pixGT[j])/(float)pixGT[j];
  }
  
  float totalErr = ((float)sumErr/(float)numObj);
  
  iftFree(pixGT);
  iftFree(pixTotal);
  iftFree(pixObj);

  return totalErr;
}

float iftUnderSegmentationMin(iftImage *gt_image, iftImage *label_image)
{
  iftVerifyImageDomains(gt_image, label_image, "iftUnderSegmentationMin");
	int i,j,p,numObj,numRegions;
	numObj = iftMaximumValue(gt_image) + 1;
	numRegions = iftMaximumValue(label_image);

	int *pixTotal = iftAllocIntArray(numRegions);
	for(p = 0; p < label_image->n; p++)
		pixTotal[label_image->val[p] - 1]++;

	int *pixObj = iftAllocIntArray(numRegions);

	int sumErr = 0;
	for(j = 0; j < numObj; j++){
		for(p = 0; p < label_image->n; p++){
			if(gt_image->val[p] == j){
				pixObj[label_image->val[p]-1]++;
			}
		}
		for(i = 0; i < numRegions; i++){
			float area =  pixObj[i];
			if((pixTotal[i] - pixObj[i]) < area){
				area = (pixTotal[i] - pixObj[i]);
			}
			sumErr = sumErr + area;
			pixObj[i] = 0;
		}
	}

	float totalErr = sumErr/(float)label_image->n;

	iftFree(pixTotal);
	iftFree(pixObj);

	return totalErr;
}




typedef struct ift_manifold_node {
  int     voxel;
  iftSet *adj; 
  float  *feat;
} iftManifoldNode;

typedef struct ift_Object_Manifold {
  iftManifoldNode *node;
  int              nfeats, nnodes;
  iftImage        *index;   
} iftObjectManifold;

iftObjectManifold *iftCreateObjectManifold(iftImage *label, int nfeats)
{
  iftAdjRel *A, *B;
  iftImage  *border;
  int        p, q, i, s;
  iftVoxel   u, v;
  iftObjectManifold *om = (iftObjectManifold *) iftAlloc(1,sizeof(iftObjectManifold));

  om->index  = iftCreateImage(label->xsize,label->ysize,label->zsize);
  om->nfeats = nfeats;

  if (iftIs3DImage(label)){ 
    A = iftSpheric(sqrtf(3.0));
    B = iftSpheric(1.0);
  }else{ 
    A = iftCircular(sqrtf(2.0));
    B = iftCircular(1.0);
  }
  
  border = iftObjectBorders(label,B);
  iftDestroyAdjRel(&B);

  /* count number of object border voxels */

  om->nnodes = 0;
  for (p=0; p < border->n; p++) 
    if (border->val[p]!=0)
      om->nnodes ++;

  /* alloc memory for the object manifold, while storing boundary
     voxels and setting index array */
  
  om->node = (iftManifoldNode *) iftAlloc(om->nnodes,sizeof(iftManifoldNode));
  for (p=0,s=0; p < border->n; p++) {
    om->index->val[p] = NIL;
    if (border->val[p]!=0){
      om->node[s].voxel = p;
      om->node[s].feat  = (float *)iftAlloc(nfeats,sizeof(float));
      om->node[s].adj   = NULL;
      om->index->val[p] = s;
      s++;
    }
  }

  /* Insert adjacent nodes by using the index array */

  for (s=0; s < om->nnodes; s++) {
    p = om->node[s].voxel;
    u = iftGetVoxelCoord(om->index,p);
    for (i=1; i < A->n; i++) {
      v = iftGetAdjacentVoxel(A,u,i);
      if (iftValidVoxel(om->index,v)){
	q = iftGetVoxelIndex(om->index,v);
	if (om->index->val[q]!=NIL){
	  iftInsertSet(&(om->node[s].adj),om->index->val[q]);
	}
      }
    }
  }
  
  iftDestroyImage(&border);
  iftDestroyAdjRel(&A);

  return(om);
}

Obsolete, since iftObjectBorderSet is now more general and doing
   the same

iftSet *iftLabelBordersSet(iftImage *label, iftAdjRel *A)
{
  iftSet *border=NULL;
  int i,p,q;
  iftVoxel u,v;

  for (u.z=0; u.z < label->zsize; u.z++)
    for (u.y=0; u.y < label->ysize; u.y++)
      for (u.x=0; u.x < label->xsize; u.x++){
	p = iftGetVoxelIndex(label,u);
	if (label->val[p]!=0){
	  for (i=1; i < A->n; i++) {
	    v.x = u.x + A->dx[i];
	    v.y = u.y + A->dy[i];
	    v.z = u.z + A->dz[i];
	    if (iftValidVoxel(label,v)){
	      q = iftGetVoxelIndex(label,v);
	      if (label->val[q]!=label->val[p]){
		iftInsertSet(&border,p);
		break;
	      }
	    }else{
	      iftInsertSet(&border,p);
	      break;
	    }
	  }
	}
      }

  return(border);
}

iftBMap *iftPropagationFrontier (iftImageForest *fst, iftBMap *propagation)
{
    int p=0, i, q;
    iftVoxel u, v;
    iftBMap *output = iftCreateBMap(propagation->n);
    for (p = 0; p < propagation->n; p++)
    {
        u  = iftGetVoxelCoord(fst->img, p);
        if (iftBMapValue(propagation, p))
        {
            for (i = 1; i < fst->A->n; i++)
            {
                v = iftGetAdjacentVoxel(fst->A, u, i);
                if (iftValidVoxel(fst->img, v))
                {
                    q = iftGetVoxelIndex(fst->img, v);
                    if (fst->label->val[p] != fst->label->val[q])
                    {
                      iftBMapSet1(output, p);
                    }

                }
            }
        }
    }
    return output;
}

/* Code of Andre with reduction and interpolation at the end */
iftImage *iftRecProjWithNoMetal(iftImage *img, iftImage *metal)
{
  iftAdjRel *A=iftCircular(sqrtf(2.0));  
  iftImage  *pathval=NULL,*root=NULL,*rec=iftCopyImage(img);
  iftGQueue *Q=NULL;
  int        i,p,q,tmp;
  iftVoxel   u,v,r;
  float      val, sum, *weight;

  pathval  = iftCreateImage(img->xsize,img->ysize,img->zsize);
  root     = iftCreateImage(img->xsize,img->ysize,img->zsize);
  Q        = iftCreateGQueue(QSIZE,img->n,pathval->val);
  iftSetRemovalPolicy(Q,MINVALUE);

  /* Initialize path values and compute seeds for the IFT */

  for (p=0; p < img->n; p++) {
    if (metal->val[p]!=0){
      pathval->val[p] = INFINITY_INT;
    } else {
      u = iftGetVoxelCoord(img,p);
      for (i=1; i < A->n; i++){
	v = iftGetAdjacentVoxel(A,u,i);
	if (iftValidVoxel(img,v)){	
	  q = iftGetVoxelIndex(img,v);
	  if (metal->val[q]!=0){
	    pathval->val[p]=0;
	    root->val[p]   =p;
	    iftInsertGQueue(&Q,p);
	    break;
	  }
	}
      }
    }
  }

  /* Image Foresting Transform */

  while(!iftEmptyGQueue(Q)) {

    p = iftRemoveGQueue(Q);
    u = iftGetVoxelCoord(img,p);
    r = iftGetVoxelCoord(img,root->val[p]);
  
    for (i=1; i < A->n; i++){
      v = iftGetAdjacentVoxel(A,u,i);
	
      if (iftValidVoxel(img,v)){	
	q = iftGetVoxelIndex(img,v);

	if ((Q->L.elem[q].color != BLACK)&&
	    (metal->val[q]!=0)) {

	  tmp = iftVoxelSquareDistance(r,v);

	  if (tmp < pathval->val[q]){ 
	    if (Q->L.elem[q].color == GRAY)
	      iftRemoveGQueueElem(Q,q);
	    pathval->val[q] = tmp;
	    root->val[q]    = root->val[p];
	    img->val[q]     = MIN(img->val[p],img->val[q]);
	    iftInsertGQueue(&Q, q);
	  }
	}
      }
    }
  }

  /* Filter image */

  iftDestroyAdjRel(&A);
  A = iftSpheric(3.0);
  sum = 0.0; 
  weight = iftAllocFloatArray(A->n);
  for (i=0; i < A->n; i++){
    weight[i]=exp(-(A->dx[i]*A->dx[i]+A->dy[i]*A->dy[i]+A->dz[i]*A->dz[i])/20.0);
    sum += weight[i];
  }
  for (i=0; i < A->n; i++){
    weight[i] /= sum;
  }

  for (p=0; p < img->n; p++) {
    if (metal->val[p]!=0){
      u      = iftGetVoxelCoord(img,p);
      val    = img->val[p]*weight[0];
      for (i=1; i < A->n; i++){
	v = iftGetAdjacentVoxel(A,u,i);
	if (iftValidVoxel(img,v)){	
	  q    = iftGetVoxelIndex(img,v);
	  val += img->val[q]*weight[i];
	}
      }    
      rec->val[p] = (int)(val); 
    }
  }

  iftFree(weight);

  iftDestroyGQueue(&Q);
  iftDestroyAdjRel(&A);
  iftDestroyImage(&pathval);
  iftDestroyImage(&root);

  return(rec);
}


int main(int argc, char *argv[]) 
{
  iftImage  *orig, *body, *interp[2], *roi,  *fixed, *mask[2];
  iftImage  *inner_markers, *bin, *outer_markers;
  char       filename[200]; 
  iftAdjRel *A=NULL;
  FILE      *fp;
  timer     *t1=NULL,*t2=NULL;

  /* parameters.txt */

  iftVoxel   uo, uf; /* ROI */
  float      scale, sx=0, sy=0;  /* interpolation */
  int        Imax,Imin; /* minimum and maximum linear stretching for
			   metal enhacement */
  
  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  iftFree(trash);
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc!=4)
    iftError("Usage: iftSegmMMetals <proj-original.scn> <proj-mask> <parameters.txt>","main");
  
  orig  = iftReadImage(argv[1]);

  fp=fopen(argv[3],"r");
  fscanf(fp,"%d %d %d",&uo.x,&uo.y,&uo.z);
  fscanf(fp,"%d %d %d",&uf.x,&uf.y,&uf.z);
  fscanf(fp,"%f",&scale);
  fscanf(fp,"%d %d",&Imin,&Imax);
  fclose(fp);
  
  /* Segment body region by Otsu's thresholding */

  fprintf(stdout,"Segmenting body region\n");
  t1     = iftTic();
  bin   = iftThreshold(orig,iftOtsu(orig),INFINITY_INT,1);
  body  = iftMask(orig,bin);
  iftDestroyImage(&bin);
  t2     = iftToc();
  fprintf(stdout,"Body segmented in %f ms\n",iftCompTime(t1,t2));
  
  if ((uo.x==0)&&(uo.y==0)&&(uo.z==0)&&(uf.x==orig->xsize-1)&&
      (uf.y==orig->ysize-1)&&(uo.z==orig->zsize-1) )
    roi = iftCopyImage(body);
  else{
    fprintf(stdout,"Extracting ROI\n");
    t1     = iftTic();
    printf("%d %d %d\n",uo.x,uo.y,uo.z);
    printf("%d %d %d\n",uf.x,uf.y,uf.z);
    roi    = iftExtractROI(body,uo,uf);
    t2     = iftToc();
    fprintf(stdout,"ROI extracted in %f ms\n",iftCompTime(t1,t2));
  }

  if (scale != 1.0){
    fprintf(stdout,"Reducing image size\n");
    t1     = iftTic();
    sx = ROUND(scale*(float)roi->xsize)/(float)roi->xsize;
    sy = ROUND(scale*(float)roi->ysize)/(float)roi->ysize;
    interp[0] = iftInterp(roi,sx,sy,1.0);
    t2     = iftToc();
    fprintf(stdout,"Image size reduced in %f ms\n",iftCompTime(t1,t2));
  }else{
    interp[0] = iftCopyImage(roi);
  }

  iftDestroyImage(&roi);

  if ((Imin!=0)||(Imax!=4095)){
    fprintf(stdout,"Applying linear stretching\n");
    t1     = iftTic();
    fixed = iftLinearStretch(interp[0],Imin,Imax,0,4095);
    t2     = iftToc();
    fprintf(stdout,"Image stretched in %f ms\n",iftCompTime(t1,t2));
  }else {
    fixed = iftCopyImage(interp[0]);
  }

  iftDestroyImage(&interp[0]);

  fprintf(stdout,"Extracting inner and outer markers\n");
  t1     = iftTic();
  inner_markers = iftSelectInnerMarkers(fixed);
  outer_markers = iftSelectOuterMarkers(fixed);
  t2     = iftToc();
  fprintf(stdout,"Markers extracted in %f ms\n",iftCompTime(t1,t2));

  iftWriteImage(fixed,"fixed.scn");

  fprintf(stdout,"Segmenting metals\n");
  t1     = iftTic();
  A       = iftSpheric(sqrtf(3.0));
  bin     = iftSegmMMetals(fixed,A,inner_markers,outer_markers);
  iftDestroyAdjRel(&A);
  t2     = iftToc();
  fprintf(stdout,"Metals extracted in %f ms\n",iftCompTime(t1,t2));

  iftWriteImage(bin,"proj-mask.scn");
  exit(0); 

  iftDestroyImage(&body);
  iftDestroyImage(&fixed);
  iftDestroyImage(&inner_markers);
  iftDestroyImage(&outer_markers);

  /* Post-processing */

  fprintf(stdout,"Returning metal mask to its original size\n");
  t1      = iftTic();
  if (scale != 1.0)
    mask[0] = iftShapeBasedInterp(bin,1.0/sx,1.0/sy,1.0);
  else
    mask[0] = iftCopyImage(bin);
  iftDestroyImage(&bin);
  t2      = iftToc();
  fprintf(stdout,"Interpolation done in %f ms\n",iftCompTime(t1,t2));

  fprintf(stdout,"Improving mask \n");

  A = iftCircular(sqrtf(2.0));
  t1      = iftTic();
#pragma omp parallel for shared(A,orig,bin)
  for (int z=0; z < orig->zsize; z++) {
    iftImage *bin_slice = iftGetXYSlice(bin,z);
    iftSet *S=NULL;
    iftImage *dil  = iftDilateBin(bin_slice,&S,sqrtf(2.0));
    iftDestroySet(&S);
    iftDestroyImage(&bin_slice);
    iftImage  *gray_slice = iftGetXYSlice(orig,z);
    iftImage  *borders    = iftImageBasins(gray_slice,A);
    iftFImage *weight     = iftSmoothWeightImage(borders,0.5);
    iftDestroyImage(&borders);
    bin_slice   = iftFastSmoothObjects(dil,weight,10);
    iftDestroyImage(&gray_slice);
    iftDestroyImage(&dil);
    iftDestroyFImage(&weight);
    iftPutXYSlice(bin,bin_slice,z);
    iftDestroyImage(&bin_slice); 
  }
  t2      = iftToc();
  fprintf(stdout,"Mask improved in %f ms\n",iftCompTime(t1,t2));
  sprintf(filename,"%s.scn",argv[2]);


float iftMaxGeodesicMeasure(iftGLen *gl, int *R, int nroots)
{
  int         i;
  float       auxgl,maxgl=0;

  nroots = 4; /* We only need to compute paths for three out of the
		 six neighbors */

  for (i=1; i < nroots; i++) {      
    
    if (gl->cost[R[i]]==INFINITY_FLT){
      auxgl = iftDistance26(iftGetVoxelCoord(gl->root,R[0]),iftGetVoxelCoord(gl->root,R[i]));
      if (auxgl == INFINITY_FLT)
    	auxgl = iftGeodesicLength(gl, R[i]);

    }else{
      auxgl = gl->cost[R[i]];
    }

    if (auxgl > maxgl){
      maxgl = auxgl;
    }

  }


  return(maxgl);   
}

iftFImage *iftMSSkel1(iftImage *bin){

  iftFImage *skel;
  char       skiz=0;
  int        p;
  int        nroots, R[7]; /* number of extended roots and extended root set */
  iftImage  *root,*label,*pred;
  iftAdjRel *B=iftSpheric(1.0), *A=iftSpheric(sqrtf(3.0)); 
  iftGLen   *gl;

  iftLabelRootPropagation(bin,A,&root,&label,&pred);
  iftDestroyImage(&pred);
  iftDestroyAdjRel(&A);

  gl        = iftCreateGLenDataStruct(root,label);   
  skel      = iftCreateFImage(root->xsize,root->ysize,root->zsize);

  for (p=0; p < root->n; p++) {

    if ((label->val[p]!=0)&&(root->val[p]!=p)){
      
      /* reset data structure for geodesic computations */

      iftResetGLenDataStruct(gl);
    
      /* initialize data structure for a new geodesic computation */
    
      gl->cost[root->val[p]] = 0.0;
      iftInsertSet(&gl->visited_voxel,root->val[p]);
      iftInsertFHeap(gl->Q,root->val[p]);

      /* repeat skeleton computation along each optimum path */
	
      nroots = iftIdentifyRootsForSkelAndSkizComp(root, label, B, p, R, &skiz);

      /* compute the maximum geodesic length between roots */


      if (!skiz) {
	skel->val[p] = iftMaxGeodesicMeasure(gl,R,nroots);	    
      }else{
	skel->val[p] = NIL;
      }
    }
  }

  iftDestroyGLenDataStruct(&gl);
  
  skel->maxval = iftFMaximumValue(skel);
  printf("%f \n",skel->maxval);

  for (p=0; p < skel->n; p++){
    if (skel->val[p]==NIL){
      skel->val[p] = skel->maxval+1;
    }  
  }
  
  iftDestroyImage(&root);
  iftDestroyImage(&label);
  iftDestroyAdjRel(&B);
  
  skel->dx = bin->dx;
  skel->dy = bin->dy;
  skel->dz = bin->dz;
  
  return(skel);
}

float iftGeodesicLength(iftGLen *gl, int end)
{
  int         p,q,i;
  iftVoxel    u,v;
  float       tmp;

  /* Geodesic Image Foresting Transform up to the end point */

  while(!iftEmptyFHeap(gl->Q)) {

    p = iftRemoveFHeap(gl->Q);
    u = iftGetVoxelCoord(gl->root,p);

    for (i=1; i < gl->A->n; i++) { 

      v = iftGetAdjacentVoxel(gl->A,u,i);
      if (iftValidVoxel(gl->root,v)){
	q = iftGetVoxelIndex(gl->root,v);

	/* must be a boundary voxel and p should be able to improve
	   its geodesic value */
	
	if ((gl->root->val[q]==q)&&(gl->cost[p] < gl->cost[q])){

	  tmp = gl->cost[p] + gl->arcw[i];

	  if (tmp < gl->cost[q]){
	    gl->cost[q] = tmp;
	    if(gl->Q->color[q] == WHITE){
	      iftInsertFHeap(gl->Q, q);
	      iftInsertSet(&gl->visited_voxel,q);
	    }else{
	      iftGoUpFHeap(gl->Q, gl->Q->pos[q]);
	    }
	  }
	}

      }
    }

    if (p==end) {
      return(gl->cost[p]);
    }
 
  }

  return(INFINITY_FLT);
}

float iftDistance26(iftVoxel u, iftVoxel v)
{
  float d[3];

  d[0] = v.x - u.x; 
  d[1] = v.y - u.y; 
  d[2] = v.z - u.z; 

  if ( (d[0]>1) || (d[1]>1) || (d[2]>1) )
    return(INFINITY_FLT);
  else{
    return(sqrtf(d[0]*d[0]+d[1]*d[1]+d[2]*d[2]));
  }
}


iftFImage *iftCurvSkel(iftImage *surf_skel)
{
  int   p, q, i, n; 
  iftAdjRel *A = iftSpheric(sqrtf(3.0)); 
  iftVoxel u, v; 
  iftFImage *curv_skel = iftCreateFImage(surf_skel->xsize,surf_skel->ysize,surf_skel->zsize); 
  iftVector vec[A->n-1];
  
  for (p=0; p < surf_skel->n; p++) {
    if (surf_skel->val[p]) { 
      u = iftGetVoxelCoord(surf_skel,p);
      n = 0;

      /* get all vectors on the surface skeleton with center at p */
      for (i=1; i < A->n; i++){
	v = iftGetAdjacentVoxel(A,u,i);
	if (iftValidVoxel(surf_skel,v)){
	  q = iftGetVoxelIndex(surf_skel,v);
	  if (surf_skel->val[q]){
	    vec[n].x = v.x - u.x;
	    vec[n].y = v.y - u.y;
	    vec[n].z = v.z - u.z;
	    float m = iftVectorMagnitude(vec[n]);	    
	    vec[n].x = vec[n].x / m;
	    vec[n].y = vec[n].y / m;
	    vec[n].z = vec[n].z / m;
	    n++;
	  }
	}
      }
      /* Compute the sum of their cross products multiplied by 100.0 */

      for (i=0; i < n; i++){
	vec[i] = iftCrossProduct(vec[i],vec[(i+1)%n]);
      }

      for (i=0; i < n; i++){
	float iprod = iftInnerProduct(vec[i],vec[(i+1)%n]);
	if (iprod < 0) iprod = -iprod;
	curv_skel->val[p] += MAX(curv_skel->val[p],1.0-iprod); 
      }
    }
  }
  
  iftDestroyAdjRel(&A);

  return(curv_skel);
}

/* iftFImage *iftMSSkel2D(iftImage *bin, iftAdjRel *A, char side) */
/* { */
/*   iftFImage *skel; */
/*   iftVoxel   u,v; */
/*   int        p,q,s,i,j,k; */
/*   float      maxgl,arcw=0.0,gl; */
/*   iftSFGraph  *graph=NULL; */
/*   int        nroots, r[5], R[5]; // extended root sets */
/*   iftImage   *root,*label; */

/*   iftLabelRootPropagation(bin,A,side,&root,&label); */
/*   graph = iftSurfaceGraph(root,A); */
/*   skel  = iftCreateFImage(root->xsize,root->ysize,root->zsize); */

/*   u.z = v.z = 0; */
/*   for (u.y=0; u.y < label->ysize; u.y++) */
/*     for (u.x=0; u.x < label->xsize; u.x++){ */

/*       p = iftGetVoxelIndex(label,u); */
	
/*       if ((label->val[p]!=0)&&(root->val[p]!=p)){ */
	  
/* 	// Identify distinct roots for skeletonization and skiz */
	
/* 	maxgl=0.0; */
/* 	R[0]=root->val[p]; j=1; */
/* 	for (i=1; i < 5; i++) { */
/* 	  v.x = u.x + A->dx[i]; */
/* 	  v.y = u.y + A->dy[i]; */
/* 	  if (iftValidVoxel(label,v)){ */
/* 	    q = iftGetVoxelIndex(label,v); */
/* 	    if (label->val[p]<label->val[q]){ // skiz */
/* 	      maxgl = NIL; */
/* 	      break; */
/* 	    }else{ */
/* 	      if (label->val[p]==label->val[q]){ // skeleton */
/* 		if (root->val[p]!=root->val[q]){ */
/* 		  // Compute union between root and root list */
		    
/* 		  for (k=0; k < j; k++) */
/* 		    if (R[k]==root->val[q]) */
/* 		      break; */
/* 		  if (k==j){ */
/* 		    R[j]=root->val[q]; j++; */
/* 		  } */
/* 		} */
/* 	      } */
/* 	    } */
/* 	  } */
/* 	} */
/* 	nroots=j; */

/* 	if ((maxgl==0.0)&&(nroots >= 2)){ // compute maximum geodesic length */
 
	  
/* 	  for (s=0; s < graph->nnodes; s++) { */
/* 	    for (i=0; i < nroots; i++) */
/* 	      if (graph->node[s].voxel==R[i]) */
/* 		r[i]=s; */
/* 	  } */
	
/* 	  for (j=1; j < nroots; j++) { */
/* 	    if(!iftAdjacentSFNodes(graph,r[0],r[j],&arcw)){ */
/* 	      gl = iftGeodesicLength(graph,r[0],r[j]); */
/* 	    }else */
/* 	      { */
/* 		gl=arcw; */
/* 	      } */
	    
/* 	    if (gl > maxgl){ */
/* 	      maxgl = gl; */
/* 	    } */
/* 	  } */
/* 	} */

/* 	skel->val[p]=maxgl; */
/*       } */
/*     } */

/*   skel->maxval = iftMaximumFValue(skel); */
/*   for (p=0; p < skel->n; p++) { */
/*     if (skel->val[p]==NIL) */
/*       skel->val[p]=100.0; */
/*     else */
/*     skel->val[p]=100.0*(skel->val[p]/skel->maxval); */
/*   } */
  
/*   iftDestroySFGraph(&graph); */
/*   iftDestroyImage(&root); */
/*   iftDestroyImage(&label); */
/*   return(skel); */
/* } */

iftImage *iftArea(iftImage *bin)
{
  int p;
  iftImage *label,*area;
  iftHistogram *hist;
  iftAdjRel *A;
  
  if (bin->zsize != 1) 
    iftError("Image must be 2D","iftArea");

  A     = iftCircular(sqrtf(2.0));
  label = iftLabelComp(bin,A);
  area  = iftCreateImage(label->xsize,label->ysize,label->zsize);
  hist  = iftGrayHistogram(label,iftMaximumValue(label)+1,0);

  for (p=0; p < label->n; p++)
    if (label->val[p] > 0)
      area->val[p] = hist->val[label->val[p]];

  iftDestroyHistogram(&hist);
  iftDestroyImage(&label);
  iftDestroyAdjRel(&A);

  return(area);
}

iftImage *iftVolume(iftImage *bin)
{
  int p;
  iftImage *label,*volume;
  iftHistogram *hist;
  iftAdjRel *A;
  
  if (bin->zsize == 1) 
    iftError("Image must be 3D","iftVolume");

  A      = iftSpheric(sqrtf(3.0));
  label  = iftLabelComp(bin,A);
  volume = iftCreateImage(label->xsize,label->ysize,label->zsize);
  hist   = iftGrayHistogram(label,iftMaximumValue(label)+1,0);

  for (p=0; p < label->n; p++)
    if (label->val[p] > 0)
      volume->val[p] = (int)hist->val[label->val[p]];

  iftDestroyHistogram(&hist);
  iftDestroyImage(&label);
  iftDestroyAdjRel(&A);

  return(volume);
}

    iftLabeledSet *iftRemBkgSeedsForEnhancement(iftImage *img, iftLabeledSet *seed)
    {
        iftDataSet    *Z, *Zobj, *Zbkg;
        iftKnnGraph   *graph;
        iftLabeledSet *enhaseed=NULL;
        int   s,kmax,noutliers;
        float train_perc;

        Z      =  iftImageSeedsToDataSet(img,seed);
        iftSetDistanceFunction(Z, 1);
        if (Z->nfeats==3){ // for color images only
            Z->alpha[0] = 0.20;
            Z->alpha[1] = 1.00;
            Z->alpha[2] = 1.00;
        }

        Zobj       = iftExtractObjectClasses(Z);
        Zbkg       = iftExtractClass(Z,1);

        if (Zobj->nsamples > 200)
            train_perc = (float) 200.0/Zobj->nsamples;
        else
            train_perc = 1.0;

        iftSelectUnsupTrainSamples(Zobj,train_perc);
        kmax  = (int)(0.05*Zobj->ntrainsamples);
        graph = iftCreateKnnGraph(Zobj,kmax);
        iftUnsupTrain(graph,iftNormalizedCut);
        noutliers=iftUnsupClassify(graph,Zbkg);
        iftDestroyKnnGraph(&graph);

        if (noutliers == 0){
            iftDestroyDataSet(&Z);
            iftDestroyDataSet(&Zobj);
            iftDestroyDataSet(&Zbkg);
            return(seed);
        }

        for (s=0; s < Zbkg->nsamples; s++)
            if (Zbkg->sample[s].status == OUTLIER){
                iftInsertLabeledSet(&enhaseed,Zbkg->sample[s].id,1);
            }

        for (s=0; s < Zobj->nsamples; s++)
            iftInsertLabeledSet(&enhaseed,Zobj->sample[s].id,2);

        iftDestroyDataSet(&Z);
        iftDestroyDataSet(&Zobj);
        iftDestroyDataSet(&Zbkg);

        return(enhaseed);
    }

iftImage *iftEnhanceObjects(iftImage *img, iftLabeledSet *seed)
{
  float          train_perc;
  iftImage      *objmap=NULL;
  iftDataSet    *Z=NULL,*T=NULL;
  iftCplGraph   *graph=NULL;
  iftLabeledSet *enhaseed=NULL; 

  enhaseed = iftRemBkgSeedsForEnhancement(img,seed);

  if (enhaseed == seed) {
    return(NULL);
  }

  /* Compute object enhancement */

  T  = iftImageSeedsToDataSet(img, enhaseed);
  iftDestroyLabeledSet(&enhaseed);
    
  if (T->nsamples > 1000)
    train_perc   = (float)1000/T->nsamples;
  else
    train_perc   = 1.0;
    
  graph = iftSupLearn(T,train_perc);
  Z     = iftImageToDataSet(img);
  iftFuzzyClassify(graph,Z);
  iftDestroyCplGraph(&graph);
  objmap = iftDataSetWeight(Z,"iftImage");
  iftDestroyDataSet(&Z);
  iftDestroyDataSet(&T);

  iftCopyVoxelSize(img,objmap);

  return(objmap);
}

    iftLabeledSet *iftRemObjSeedsForEnhancement(iftImage *img, iftLabeledSet *seed)
    {
        iftDataSet    *Z, *Zobj, *Zbkg;
        iftKnnGraph   *graph;
        iftLabeledSet *enhaseed=NULL;
        int   s,kmax,noutliers;
        float train_perc;

        Z          =  iftImageSeedsToDataSet(img,seed);
        iftSetDistanceFunction(Z, 1);
        if (Z->nfeats==3){ // for color images only
            Z->alpha[0] = 0.20;
            Z->alpha[1] = 1.00;
            Z->alpha[2] = 1.00;
        }


        Zobj       = iftExtractObjectClasses(Z);
        Zbkg       = iftExtractClass(Z,1);

        if (Zbkg->nsamples > 200)
            train_perc = (float) 200.0/Zbkg->nsamples;
        else
            train_perc = 1.0;

        iftSelectUnsupTrainSamples(Zbkg,train_perc);
        kmax  = (int)(0.05*Zbkg->ntrainsamples);
        graph = iftCreateKnnGraph(Zbkg,kmax);
        iftUnsupTrain(graph,iftNormalizedCut);
        noutliers=iftUnsupClassify(graph,Zobj);
        iftDestroyKnnGraph(&graph);


        if (noutliers == 0){
            iftDestroyDataSet(&Z);
            iftDestroyDataSet(&Zobj);
            iftDestroyDataSet(&Zbkg);
            return(seed);
        }

        for (s=0; s < Zobj->nsamples; s++)
            if (Zobj->sample[s].status == OUTLIER){
                iftInsertLabeledSet(&enhaseed,Zobj->sample[s].id,1);
            }

        for (s=0; s < Zbkg->nsamples; s++)
            iftInsertLabeledSet(&enhaseed,Zbkg->sample[s].id,0);

        iftDestroyDataSet(&Z);
        iftDestroyDataSet(&Zobj);
        iftDestroyDataSet(&Zbkg);

        return(enhaseed);
    }

iftFeatures *iftObjectEigenValues(iftImage *bin)
{
  iftMatrix  *U,*S,*Vt,*A;
  iftDataSet *Z1, *Z2;
  iftFeatures *feat;

  Z1   = iftObjectToDataSet(bin);
  iftSetStatus(Z1,TRAIN);
  Z2 = iftCentralizeDataSet(Z1);
  iftDestroyDataSet(&Z1);
  //  A   = iftCovarianceMatrix(Z2);
  A = iftDataSetToFeatureMatrix(Z2);
  iftDestroyDataSet(&Z2);
  iftSingleValueDecomp(A,&U,&S,&Vt);

  if (bin->zsize > 1) {
    feat = iftCreateFeatures(3);
    feat->val[0] = S->val[0];
    feat->val[1] = S->val[1];
    feat->val[2] = S->val[2];
  }else{
    feat = iftCreateFeatures(2);
    feat->val[0] = S->val[0];
    feat->val[1] = S->val[1];
  }
  iftDestroyMatrix(&U);
  iftDestroyMatrix(&S);
  iftDestroyMatrix(&Vt);
  iftDestroyMatrix(&A);

  return(feat);
}

void         iftPDFByAbrangency(iftDataSet *Z, iftAdjRel *A, float feat_radius);

void iftPDFByAbrangency(iftDataSet *Z, iftAdjRel *A, float feat_radius)
{
  int   s,t,nelems;
  float dist, max_weight;
 
  if (feat_radius <= 0.0)
    iftError("Invalid radius in the feature space","iftPDFByAbrangency");

  /* Compute the pdf by abrangency */
  
  if ((Z->ref_data != NULL)&&(A != NULL)&&(Z->nsamples==Z->img->n)) {
    /* PDF estimation using both feature space and image domain */
    iftVoxel   u,v;
    int        i;

    max_weight = INFINITY_FLT_NEG;
    for (s=0; s < Z->nsamples; s++) {      
      Z->sample[s].weight = 0.0; nelems=0;
      u = iftGetVoxelCoord(Z->img,s);
      for (i=1; i < A->n; i++) {
	v = iftGetAdjacentVoxel(A,u,i);
	if (iftValidVoxel(Z->img,v)){
	  t    = iftGetVoxelIndex(Z->img,v);
	  if (iftDist == NULL)
	    dist = Z->iftArcWeight(Z->sample[s].feat,Z->sample[t].feat,Z->alpha,Z->nfeats);
	  else
	    dist = iftDist->distance_table[Z->sample[s].id][Z->sample[t].id];
	  if (dist <= feat_radius){
	    Z->sample[s].weight += dist;
	    nelems++;
	  }
	}
      }
      if (nelems > 0){
	Z->sample[s].weight /= nelems;
      	if (Z->sample[s].weight > max_weight) 
	  max_weight = Z->sample[s].weight;
      }
    }
  }else{ /* PDF estimation in the feature space only */
    max_weight = INFINITY_FLT_NEG;
    for (s=0; s < Z->nsamples; s++) {      
      Z->sample[s].weight = 0.0; nelems=0;
      for (t=0; t < Z->nsamples; t++) {
	if ((s != t)&&(Z->sample[t].status==TRAIN)) {
	  if (iftDist == NULL)
	    dist = Z->iftArcWeight(Z->sample[s].feat,Z->sample[t].feat,Z->alpha,Z->nfeats);
	  else
	    dist = iftDist->distance_table[Z->sample[s].id][Z->sample[t].id];
	  if (dist <= feat_radius){
	    Z->sample[s].weight += dist;
	    nelems++;
	  }
	}
	if (nelems > 0){
	  Z->sample[s].weight /= nelems;
	  if (Z->sample[s].weight > max_weight) 
	    max_weight = Z->sample[s].weight;
	}
      }
    }
  }

  /* normalize weights (pdf) */

  if (max_weight > 0.0){
    for (s=0; s < Z->nsamples; s++) {
      Z->sample[s].weight = (max_weight - Z->sample[s].weight)/max_weight;
	Z->sample[s].weight = ((MAXWEIGHT-1.0)*Z->sample[s].weight) + 1.0;
      }   
  }

  
}


iftDataSet *iftPGMKernelDataSet(char *dirname, iftAdjRel *A, int nsamples_per_image);

iftDataSet *iftPGMKernelDataSet(char *dirname, iftAdjRel *A, int nsamples_per_image)
{
  char        command[200],filename[100];
  FILE       *fp;
  int         s,i,j,k,p,q,nimages=0,nsamples=0,*sample,nelems,Dmax,D;
  iftImage   *img;
  iftDataSet *Z;
  iftVoxel    u,v;

  /* Find maximum possible displacement */

  Dmax = INFINITY_INT_NEG;
  for (k=0; k < A->n; k++) 
    if ((D=MAX(abs(A->dx[k]),abs(A->dy[k])))>Dmax)
      Dmax = D;

  /* Count number of training images */

  sprintf(command,"ls -v %s > temp.txt",dirname);
  if (system(command) == -1) iftError("Command error","iftPGMKernelDataSet");
  fp = fopen("temp.txt","r");
  while(!feof(fp)){
    if (fscanf(fp,"%s",filename)!=1) iftError("Reading error","iftPGMKernelDataSet");
    nimages++;
  }
  nimages--;
  fclose(fp);

  /* Select samples per image randomly */

 
  nsamples = nsamples_per_image * nimages;
  Z        = iftCreateDataSet(nsamples,A->n);
  strcpy(Z->dirname,dirname);

  fp = fopen("temp.txt","r");
  s        = 0;
  for (i=0; i < nimages; i++) {

    /* Read image */

    if (fscanf(fp,"%s",filename)!=1) iftError("Reading error","iftPGMKernelDataSet");
    
    sprintf(command,"convert %s/%s temp.pgm",dirname,filename);
    if (system(command) == -1) iftError("Command error","iftPGMKernelDataSet");
    img = iftReadImageP5("temp.pgm");    
    if (system("rm -f temp.pgm") == -1) iftError("Command error","iftPGMKernelDataSet");
    
    /* Select candidate samples, within the valid image domain */

    sample = iftAllocIntArray(img->n);
    j = 0; u.z = 0;
    for (u.y=Dmax; u.y < img->ysize - Dmax; u.y++) 
      for (u.x=Dmax; u.x < img->xsize - Dmax; u.x++) {
	p = iftGetVoxelIndex(img,u);
	sample[j]=p;
	j++;
      }
    nelems = j;
    nsamples = nsamples_per_image;
    if (nsamples > nelems) 
      iftError("Number of samples per image is too high","iftPGMKernelDataSet");

    /* Select samples randomly */

    while (nsamples > 0){
      j = iftRandomInteger(0,nelems-1);
      p = sample[j];
      u = iftGetVoxelCoord(img,p);
      for (k=0; k < A->n; k++) {
	v = iftGetAdjacentVoxel(A,u,k);
	q = iftGetVoxelIndex(img,v);
	Z->sample[s].feat[k]=img->val[q];
      }
      Z->sample[s].id = i;
      iftSwitchValues(&sample[j],&sample[nelems-1]);
      nelems--; nsamples--; s++;
    }
    
    iftFree(sample);
    iftDestroyImage(&img);
  }

  if (system("rm -f temp.txt") == -1) iftError("Command error","iftPGMKernelDataSet");
  fclose(fp);

  return(Z);
}

char       *iftGetImageFile(iftDataSet *Z, int id);

char *iftGetImageFile(iftDataSet *Z, int id)
{
  char command[400],*pathname,imgname[100];
  FILE *fp;
  int fileid;

  pathname = iftAllocCharArray(200);

  if (strcmp(Z->dirname,"None")==0)
    iftError("It requires an image dataset","iftGetImageFile");

  sprintf(command,"ls -v %s > temp.txt",Z->dirname);
  if (system(command) == -1) iftError("Command error","iftGetImageFile");
  fp = fopen("temp.txt","r");
  fileid = -1;  
  while(!feof(fp)&&(fileid!=id)){
    if (fscanf(fp,"%s",imgname)!=1) iftError("Reading error","iftGetImageFile");
    fileid++;
  }
  
  fclose(fp);
  if (system("rm -f temp.txt")==-1) iftError("Command error","iftGetImageFile");

  if (fileid != id) {
    iftError("Image file not found","iftGetImageFile");
  }
  
  sprintf(pathname,"%s/%s",Z->dirname,imgname);
  
  return(pathname);
}

void iftMImageIndexMatrices(iftConvNetwork *convnet)
{
  int xsize[convnet->nstages], ysize[convnet->nstages], zsize[convnet->nstages], nbands[convnet->nstages]; 
  iftMImage     *img;
  iftFastAdjRel *F;
  int            l,i;

  /* Compute image dimensions along the network */

  iftImageDimensionsAlongNetwork(convnet,xsize,ysize,zsize,nbands);

  /* Before input normalization */
  /*
  img = iftCreateMImage(xsize[0],ysize[0],zsize[0],nbands[0]);
  if (zsize[0] != 1) // 3D
    A   = iftSpheric(0.0);
  else
    A   = iftCircular(0.0);   
  F   = iftCreateFastAdjRel(A,img->tby,img->tbz);
  iftDestroyAdjRel(&A);
  convnet->img_ind[0] = iftMImageIndicesToMatrix(img,F,1);
  iftDestroyMImage(&img);
  iftDestroyFastAdjRel(&F);
  */
  /* After input normalization */
  /*
  img = iftCreateMImage(xsize[0],ysize[0],zsize[0],nbands[0]);
  F   = iftCreateFastAdjRel(convnet->input_norm_adj,img->tby,img->tbz);
  convnet->img_ind[1] = iftMImageIndicesToMatrix(img,F,1);
  iftDestroyMImage(&img);
  iftDestroyFastAdjRel(&F);
  */

  for (l=0, i=2, j=i+1, k=i+2; l < convnet->nlayers; l++,i=i+3,j=i+1,k=i+2)
    {
    
      /* After filtering */

      img = iftCreateMImage(xsize[i-1],ysize[i-1],zsize[i-1],nbands[i-1]);
      F   = iftCreateFastAdjRel(convnet->k_bank[l]->A,img->tby,img->tbz);
      //  convnet->img_ind[i] = iftMImageIndicesToMatrix(img,F,1);
      convnet->img_ind[l] = iftMImageIndicesToMatrix(img,F,1);
      iftDestroyMImage(&img);
      iftDestroyFastAdjRel(&F);

      /* After pooling */
      /*
      img = iftCreateMImage(xsize[j-1],ysize[j-1],zsize[j-1],nbands[j-1]);
      F   = iftCreateFastAdjRel(convnet->pooling_adj[l],img->tby,img->tbz);
      convnet->img_ind[j] = iftMImageIndicesToMatrix(img,F,convnet->stride[l]);
      iftDestroyMImage(&img);
      iftDestroyFastAdjRel(&F);
      */
      /* After normalization */
      /*
      img = iftCreateMImage(xsize[k-1],ysize[k-1],zsize[k-1],nbands[k-1]);
      F   = iftCreateFastAdjRel(convnet->norm_adj[l],img->tby,img->tbz);
      convnet->img_ind[k] = iftMImageIndicesToMatrix(img,F,1);
      iftDestroyMImage(&img);
      iftDestroyFastAdjRel(&F);
      */
    }

}

iftMImage *iftPoolingInd(iftMImage *input_img, iftMatrix *input_ind, int output_xsize, int output_ysize, int output_zsize, int output_nbands, float alpha) 
{

  iftMatrix     *input      = iftGetVoxelValueMatrix(input_ind,input_img);
  iftMImage     *output_img = iftCreateMImage(output_xsize,output_ysize,output_zsize,output_nbands);
  int N = input->nrows/input_img->m; // adjacency size

#pragma omp parallel for shared(input, output_img)
  for (int c=0; c < input->ncols; c++) { // for each voxel

    for (int m=0, ri=0, rf=ri+N; m < input_img->m; m++, ri += N, rf=ri+N) { // for each band

      double sum=0.0;

      for (int r=ri; r < rf; r++){  // visit adjacents for pooling
	int index = c + input->tbrow[r];
	sum += pow(input->val[index],alpha);
      }
      output_img->band[m].val[c] = pow(sum,1/alpha); // output pooling value
    }

  }      

  iftDestroyMatrix(&input);
  
  return (output_img);
}

iftMImage *iftNormalizeMImageInd(iftMImage *input_img, iftMatrix *input_ind, int output_xsize, int output_ysize, int output_zsize, int output_nbands) 
{

  iftMatrix     *input      = iftGetVoxelValueMatrix(input_ind,input_img);
  iftMImage     *output_img = iftCreateMImage(output_xsize,output_ysize,output_zsize,output_nbands);
  int            N = input->nrows/input_img->m;

#pragma omp parallel for shared(input, output_img)
  for (int c=0; c < input->ncols; c++) { // for each voxel

    double sum=0.0;

    for (int r=0; r < input->nrows; r++){  // for all adjacents in all bands
      int index = c + input->tbrow[r];
      sum += input->val[index]*input->val[index];
    }

    sum = sqrt(sum);

    if (sum > Epsilon) {
      for (int r=0, b=0; r < input->nrows; r += N, b++) {  // for each voxel of each band
	int index = c + input->tbrow[r];      
	output_img->band[b].val[c] = (float) input->val[index]/sum;
      }
    }

  }      

  iftDestroyMatrix(&input);
  
  return (output_img);
}



iftImage  *iftCroppedFastLinearFilter(iftImage *img, iftKernel *K)
{
	if(iftIsColorImage(img))
		iftError("Input must be a grayscale image.", "iftCroppedFastLinearFilter" );

	iftFastAdjRel *F=iftCreateFastAdjRel(K->A,img->tby,img->tbz);
	iftImage *fimg=iftCreateImage(img->xsize - 2*F->bx, img->ysize - 2*F->by, img->zsize - 2*F->bz);

	#pragma omp parallel for shared(img,fimg,K,F)
	for (int i=0; i < F->n; i++) {
		int output_p = 0;
		iftVoxel u;
		for (u.z=F->bz; u.z < img->zsize-F->bz; u.z++)
			for (u.y=F->by; u.y < img->ysize-F->by; u.y++)
				for (u.x=F->bx; u.x < img->xsize-F->bx; u.x++) {
					int q = (u.x + img->tby[u.y] + img->tbz[u.z]) + F->dq[i];
					fimg->val[output_p] += (int)((float) img->val[q] * K->weight[i] + 0.5);
					output_p++;
				}
	}

	iftDestroyFastAdjRel(&F);


	return(fimg);
}

iftImage* iftCroppedFastLinearFilterByMatrixMult(iftImage *img, iftKernel *K){
	if(iftIsColorImage(img))
		iftError("Input must be a grayscale image.", "iftCroppedFastLinearFilter" );

	iftFastAdjRel *F=iftCreateFastAdjRel(K->A,img->tby,img->tbz);
	iftImage *fimg=iftCreateImage(img->xsize - 2*F->bx, img->ysize - 2*F->by, img->zsize - 2*F->bz);

	iftMatrix *im = iftCreateMatrix(F->n, fimg->n);
	iftMatrix *km = iftCreateMatrix(1, F->n);

	int i;
	for(i = 0; i < F->n; i++)
		km->val[i] = K->weight[i];

	iftVoxel u;
	int p = 0;
	for (u.z=F->bz; u.z < img->zsize - F->bz; u.z++)
		for (u.y=F->by; u.y < img->ysize - F->by; u.y++){
			int b = img->tby[u.y] + img->tbz[u.z];
			for (u.x=F->bx; u.x < img->xsize - F->bx; u.x++){
				for (int i=0; i < F->n; i++) {
					int q =  u.x + b + F->dq[i];
					im->val[iftGetMatrixIndex(im,i,p)] = img->val[q];
				}
				p++;
			}
		}

	iftMatrix *result = iftMultMatrices(im,km);

	iftDestroyMatrix(&im);
	iftDestroyMatrix(&km);
	iftDestroyFastAdjRel(&F);

	for(i = 0; i < fimg->n; i++)
		fimg->val[i] = result->val[i];

	iftDestroyMatrix(&result);

	return fimg;
}



typedef struct ift_matrix {
  float **val;
  int ncols,nrows;
} iftMatrix;


iftMatrix *iftCreateMatrix(int ncols,int nrows);
void       iftDestroyMatrix(iftMatrix **A);
iftMatrix *iftCopyMatrix(iftMatrix *A);
iftMatrix *iftTransposedMatrix(iftMatrix *A);
iftMatrix *iftMultMatrices(iftMatrix *A, iftMatrix *B);
iftMatrix *iftFastMultMatrices(iftMatrix *A, iftMatrix *B);
iftMatrix *iftInvertedMatrix(iftMatrix *A);
float      iftMatrixDeterminant(iftMatrix *A);
iftMatrix *iftCoFactorMatrix(iftMatrix *A);
void       iftPrintMatrix(iftMatrix *A);
iftMatrix *iftRotationMatrix(char axis, float theta); 
iftMatrix *iftTranslationMatrix(iftVector T);
iftMatrix *iftScaleMatrix(float sx, float sy, float sz);
iftVector  iftTransformVector(iftMatrix *A, iftVector u);
iftPoint   iftTransformPoint(iftMatrix *A, iftPoint  u);
iftVoxel   iftTransformVoxel(iftMatrix  *A, iftVoxel  u);
iftMatrix *iftIdentityMatrix(int ncols);
iftMatrix *iftRotationMatrixToAlignVectorWithZ(iftVector v);
iftMatrix *iftRotationMatrixToAlignZWithVector(iftVector v);
iftMatrix *iftExtendedMatrix(iftMatrix *M, int ncols, int nrows);
void       iftSingleValueDecomp(iftMatrix *A, iftMatrix **U, iftMatrix **S, iftMatrix **Vt);


iftMatrix *iftCreateMatrix(int ncols,int nrows)
{
  iftMatrix *A=NULL;
  float *aux;
  int i;

  A = (iftMatrix *) iftAlloc(1,sizeof(iftMatrix));
  if(A == NULL)
    iftError(MSG1,"iftCreateMatrix");

  aux    = (float *)  iftAllocFloatArray(nrows*ncols);
  A->val = (float **) iftAlloc(nrows, sizeof(float *));
  if ((A->val == NULL)||(aux == NULL))
    iftError(MSG1,"iftCreateMatrix");
  A->val[0] = aux;
  for(i=1; i < nrows; i++) 
    A->val[i] = A->val[i-1] + ncols;

  A->ncols = ncols;
  A->nrows = nrows;
 
  return(A);
}



void iftDestroyMatrix(iftMatrix **A)
{
  iftMatrix *aux1=*A;
  float     *aux2;

  if(aux1 != NULL){
    if (aux1->val != NULL) {
      aux2 = aux1->val[0];
      if (aux2 != NULL) iftFree(aux2);
      iftFree(aux1->val);
    }
    iftFree(aux1);
    *A = NULL;
  }
}

iftMatrix *iftCopyMatrix(iftMatrix *A)
{
  iftMatrix *B;
  int i,j;

  B = iftCreateMatrix(A->ncols, A->nrows);
  
  for (i=0; i < A->nrows; i++) 
    for (j=0; j < A->ncols; j++) 
      B->val[i][j] = A->val[i][j];
  
  return(B);
}

void   iftPrintMatrix(iftMatrix *A)
{
  int i, j;
  
  fprintf(stdout,"\n");
  for(i=0; i < A->nrows; i++){
    for(j=0; j < A->ncols; j++)
      fprintf(stdout,"%3.5lf ", A->val[i][j]);
    fprintf(stdout,"\n");
  }
  fprintf(stdout,"\n");
}


iftMatrix *iftMultMatrices(iftMatrix *A, iftMatrix *B)
{
  iftMatrix *M = NULL;
  int i,j,k;
  
  if(A->ncols!=B->nrows)
    iftError("Cannot multiply matrices","iftMultMatrices");
  
  M = iftCreateMatrix(B->ncols, A->nrows);
  for(i=0; i < M->nrows; i++){
    for(j=0; j < M->ncols; j++){
       M->val[i][j] = 0.0;
       for (k=0; k<A->ncols; k++)
	 M->val[i][j] += A->val[i][k]*B->val[k][j];
    }
  }
  return(M);
}

iftMatrix *iftFastMultMatrices(iftMatrix *A, iftMatrix *B)
{
  iftMatrix *M = NULL;
  int i,j,k;
  double *A1, *B1, *M1; /* M1 = alpha A1*B1 + beta M1 */
  double  alpha=1.0, beta=0.0; 
 
  if(A->ncols!=B->nrows)
    iftError("Cannot multiply matrices","iftFastMultMatrices");

  /* Transform Matrices into Vectors for blas */
  
  A1 = iftAllocDoubleArray(A->ncols*A->nrows);
  B1 = iftAllocDoubleArray(B->ncols*B->nrows);
  M1 = iftAllocDoubleArray(B->ncols*A->nrows);

  k=0;
  for(i=0; i < A->nrows; i++){
    for(j=0; j < A->ncols; j++){
      A1[k] = A->val[i][j];
      k++;
    }
  }
  k=0;
  for(i=0; i < B->nrows; i++){
    for(j=0; j < B->ncols; j++){
      B1[k] = B->val[i][j];
      k++;
    }
  }

  /* Compute fast multiplication between matrices */

  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, A->nrows, B->ncols, A->ncols, alpha, A1, A->ncols, B1, B->ncols, beta, M1, B->ncols);

  iftFree(A1); iftFree(B1);
  
  /* Copy result into output matrix */

  M = iftCreateMatrix(B->ncols, A->nrows);
  k = 0;
  for(i=0; i < M->nrows; i++){
    for(j=0; j < M->ncols; j++){
       M->val[i][j] = M1[k];
       k++;
    }
  }
  
  iftFree(M1);

  return(M);
}


iftMatrix *iftTransposedMatrix(iftMatrix *A)
{
  iftMatrix *B = NULL;
  int i,j;
    
  B = iftCreateMatrix(A->nrows, A->ncols);
  for(i=0; i < B->nrows; i++){
    for(j=0; j < B->ncols; j++){
      B->val[i][j] = A->val[j][i];
    }
  }
  return(B);
}

iftMatrix *iftRotationMatrix(char axis, float theta)
{
  iftMatrix *A;
  float cos_theta,sin_theta;

  A         = iftCreateMatrix(4,4);
  theta     = theta*PI/180.0;
  cos_theta = cosf(theta); 
  sin_theta = sinf(theta); 

  switch(axis) {

  case AXIS_X:

    A->val[0][0] = 1.0; A->val[0][1] = 0.0; A->val[0][2] = 0.0; A->val[0][3] = 0.0;
    A->val[1][0] = 0.0; A->val[1][1] = cos_theta; A->val[1][2] = -sin_theta; A->val[1][3] = 0.0;
    A->val[2][0] = 0.0; A->val[2][1] = sin_theta; A->val[2][2] = cos_theta;  A->val[2][3] = 0.0;
    A->val[3][0] = 0.0; A->val[3][1] = 0.0; A->val[3][2] = 0.0; A->val[3][3] = 1.0;  
    break;

  case AXIS_Y:

    A->val[0][0] = cos_theta;  A->val[0][1] = 0.0;  A->val[0][2] = sin_theta;  A->val[0][3] = 0.0;
    A->val[1][0] = 0.0;  A->val[1][1] = 1.0;  A->val[1][2] = 0.0;  A->val[1][3] = 0.0;
    A->val[2][0] = -sin_theta;  A->val[2][1] = 0.0;  A->val[2][2] = cos_theta;  A->val[2][3] = 0.0;
    A->val[3][0] = 0.0;  A->val[3][1] = 0.0;  A->val[3][2] = 0.0;  A->val[3][3] = 1.0;
    
    break;

  case AXIS_Z:

    A->val[0][0] = cos_theta;  A->val[0][1] = -sin_theta;  A->val[0][2] = 0.0;  A->val[0][3] = 0.0;
    A->val[1][0] = sin_theta;  A->val[1][1] =  cos_theta;  A->val[1][2] = 0.0;  A->val[1][3] = 0.0;
    A->val[2][0] = 0.0;  A->val[2][1] = 0.0;  A->val[2][2] = 1.0;  A->val[2][3] = 0.0;
    A->val[3][0] = 0.0;  A->val[3][1] = 0.0;  A->val[3][2] = 0.0;  A->val[3][3] = 1.0;
    
    break;

  default:
    iftError("Invalid option for the axis","iftRotationMatrix");
  }

  return(A);
}

iftMatrix *iftTranslationMatrix(iftVector T)
{
  iftMatrix *A;

  A = iftCreateMatrix(4,4);

  A->val[0][0] = 1.0;    A->val[0][1] = 0.0;    A->val[0][2] = 0.0;    A->val[0][3] = T.x;
  A->val[1][0] = 0.0;    A->val[1][1] = 1.0;    A->val[1][2] = 0.0;    A->val[1][3] = T.y;
  A->val[2][0] = 0.0;    A->val[2][1] = 0.0;    A->val[2][2] = 1.0;    A->val[2][3] = T.z;
  A->val[3][0] = 0.0;    A->val[3][1] = 0.0;    A->val[3][2] = 0.0;    A->val[3][3] = 1.0;

  return(A);
}

iftMatrix *iftScaleMatrix(float sx, float sy, float sz)
{
  iftMatrix *A;

  A = iftCreateMatrix(4,4);

  A->val[0][0] = sx;    A->val[0][1] = 0.0;    A->val[0][2] = 0.0;    A->val[0][3] = 0;
  A->val[1][0] = 0.0;    A->val[1][1] = sy;    A->val[1][2] = 0.0;    A->val[1][3] = 0;
  A->val[2][0] = 0.0;    A->val[2][1] = 0.0;    A->val[2][2] = sz;    A->val[2][3] = 0;
  A->val[3][0] = 0.0;    A->val[3][1] = 0.0;    A->val[3][2] = 0.0;    A->val[3][3] = 1.0;

  return(A);
}

iftVector iftTransformVector(iftMatrix *A, iftVector u)
{
  iftMatrix *um,*res;
  iftVector v;

  um = iftCreateMatrix(1,4);
  um->val[0][0]=u.x;
  um->val[1][0]=u.y;
  um->val[2][0]=u.z;
  um->val[3][0]=1.0;
  res=iftFastMultMatrices(A,um);
  v.x = res->val[0][0];
  v.y = res->val[0][1];
  v.z = res->val[0][2];
  iftDestroyMatrix(&um);
  iftDestroyMatrix(&res);

  return(v);
}

iftPoint iftTransformPoint(iftMatrix *A, iftPoint u)
{
  iftMatrix *um,*res;
  iftPoint v;

  um = iftCreateMatrix(1,4);
  um->val[0][0]=u.x;
  um->val[1][0]=u.y;
  um->val[2][0]=u.z;
  um->val[3][0]=1.0;
  res=iftFastMultMatrices(A,um);
  v.x = res->val[0][0];
  v.y = res->val[0][1];
  v.z = res->val[0][2];
  iftDestroyMatrix(&um);
  iftDestroyMatrix(&res);

  return(v);
}

iftVoxel iftTransformVoxel(iftMatrix *A, iftVoxel u)
{
  iftMatrix *um,*res;
  iftVoxel v;

  um = iftCreateMatrix(1,4);
  um->val[0][0]=u.x;
  um->val[1][0]=u.y;
  um->val[2][0]=u.z;
  um->val[3][0]=1.0;
  res=iftFastMultMatrices(A,um);
  v.x = ROUND(res->val[0][0]);
  v.y = ROUND(res->val[0][1]);
  v.z = ROUND(res->val[0][2]);
  iftDestroyMatrix(&um);
  iftDestroyMatrix(&res);

  return(v);
}

iftMatrix *iftInvertedMatrix(iftMatrix *A)
{
  iftMatrix *B=NULL,*C=NULL;
  float det;
  int i,j; 

  if (A->ncols != A->nrows) 
    iftError("Matrix is not square","iftInvertedMatrix");

  det = iftMatrixDeterminant(A);

  if (fabs(det) < Epsilon){
    iftError("Matrix is not invertible","iftInvertedMatrix");
  }else{
    C = iftCoFactorMatrix(A);
    B = iftTransposedMatrix(C); /* Adjoint Matrix */
    for (i=0; i < B->nrows; i++) 
      for (j=0; j < B->ncols; j++){
	B->val[i][j] /= det;
      } 
  }
  iftDestroyMatrix(&C);

  return(B);
}

float iftMatrixDeterminant(iftMatrix *A)
{
  int        i,j,k,l, N=A->ncols;
  float      det = 0;
  iftMatrix *B = NULL;

   if (A->ncols != A->nrows) 
     iftError("Matrix is not square","iftMatrixDeterminant");

   if (N == 1) { /* trivial case */
     det = A->val[0][0];
   } else { /* 2 x 2 matrix */
     if (N == 2) { 
       det = A->val[0][0] * A->val[1][1] - A->val[1][0] * A->val[0][1];
     } else { /* N x N matrix */
       det = 0;
       for (k=0; k < N; k++) {
	 B   = iftCreateMatrix(N-1,N-1);
  	 for (i=1; i < N; i++){
	   l = 0;
	   for (j=0; j < N; j++) {
	     if (j != k){
	       B->val[i-1][l] = A->val[i][j];
	       l++;
	     }
	   }
	 }       
	 det += powf(-1.0,k+2.0) * A->val[0][k] * iftMatrixDeterminant(B);
	 iftDestroyMatrix(&B);
       }
     }
   }
   return(det);
}

iftMatrix *iftCoFactorMatrix(iftMatrix *A)
{
  int        i1,j1,i2,j2,i3,j3,N=A->ncols;
  float      det;
  iftMatrix *B, *C;

  if (A->ncols != A->nrows) 
    iftError("Matrix is not square","iftCoFactorMatrix");

  B = iftCreateMatrix(N,N);
  C = iftCreateMatrix(N-1,N-1);

  for (j1=0; j1 < N; j1++) {
    for (i1=0; i1 < N; i1++) {
      
      i3 = 0;
      for (i2=0; i2 <N ; i2++) {
	if (i2 != i1){
	  j3 = 0;
	  for (j2=0; j2 < N; j2++) {
	    if (j2 != j1){
	      C->val[i3][j3] = A->val[i2][j2];
	      j3++;
	    }
	  }
	  i3++;
	}
      }
      det = iftMatrixDeterminant(C);
      B->val[i1][j1] = powf(-1.0,i1+j1+2.0) * det;
    }
  }

  iftDestroyMatrix(&C);
  return(B);
}

iftMatrix *iftIdentityMatrix(int ncols)
{
  iftMatrix *I=iftCreateMatrix(ncols,ncols);
  int i;
  
  for (i=0; i < ncols; i++) 
    I->val[i][i] = 1.0;

  return(I);
}

/* This function aims at aligning a vector v with the direction of the
   axis Z. First, it rotates v by theta_x around the axis X until it
   falls on the xz plane. The value of theta_x can be obtained from
   the projection v_{yz} of v onto the yz plane as atan(vy/vz). It
   then rotates v (already on the XZ plane) of -theta_y until it
   aligns with the axis Z. The value of theta_y can be obtained from
   the xz plane as atan(vx/vz'), where vz' = vz / cos(theta_x). */

iftMatrix *iftRotationMatrixToAlignVectorWithZ(iftVector v)
{
  float theta_x,theta_y, m = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z); 
  iftMatrix *Rx,*Ry,*M;

  if (m == 0.0)
    iftError("There is no vector","iftRotationMatrixToAlignVectorWithZ");

  /* Make sure it is normalized */

  v.x /= m;
  v.y /= m;
  v.z /= m;

  /* treat the special cases where the tangent is not computable */

  if (v.z == 0.0) {
    if (v.x == 0.0)
      theta_y = 0.0;
    else
      theta_y = 90.0;

    if (v.y == 0.0)
      theta_x = 0.0;
    else
      theta_x = 90.0;

  }else{
    theta_x   = atanf(v.y/v.z);
    float vz1 = v.z / cosf(theta_x);
    theta_x   = theta_x*180.0/PI;
    theta_y   = atanf(v.x/vz1)*180.0/PI;
  }

  /* Compute the corresponding Rx and Ry matrices, and multiply them
     to create the desired matrix. */

  Rx = iftRotationMatrix(AXIS_X,theta_x);
  Ry = iftRotationMatrix(AXIS_Y,-theta_y);
  M  = iftFastMultMatrices(Ry,Rx);

  iftDestroyMatrix(&Ry);  
  iftDestroyMatrix(&Rx);

  return(M);
}

/* This function computes the inverse of the above matrix in order to
   align the axis Z with a vector v. Essentially, it has to compute
   the inverse of Ry(-theta_y)Rx(theta_x), which is
   Rx(-theta_x)Ry(theta_y). */

iftMatrix *iftRotationMatrixToAlignZWithVector(iftVector v)
{
  float theta_x,theta_y, m = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z); 
  iftMatrix *Rx,*Ry,*M;

  if (m == 0.0)
    iftError("There is no vector","iftRotationMatrixToAlignVectorWithZ");

  v.x /= m;
  v.y /= m;
  v.z /= m;

  if (v.z == 0.0) {
    if (v.x == 0.0)
      theta_y = 0.0;
    else
      theta_y = 90.0;

    if (v.y == 0.0)
      theta_x = 0.0;
    else
      theta_x = 90.0;
  }else{
    theta_x   = atanf(v.y/v.z);
    float vz1 = v.z / cosf(theta_x);
    theta_x   = theta_x*180.0/PI;
    theta_y   = atanf(v.x/vz1)*180.0/PI;
  }

  Rx = iftRotationMatrix(AXIS_X,-theta_x);
  Ry = iftRotationMatrix(AXIS_Y,theta_y);
  M  = iftFastMultMatrices(Rx,Ry);

  iftDestroyMatrix(&Ry);  
  iftDestroyMatrix(&Rx);

  return(M);
}

iftMatrix *iftExtendedMatrix(iftMatrix *M, int ncols, int nrows)
{
  iftMatrix *E=iftCreateMatrix(ncols,nrows);
  int i,j; 

  if ((ncols < M->ncols)&&(nrows < M->nrows))
    iftError("Cannot extend the dimensions of the matrix","iftExtendedMatrix");

  for (i=0; i < E->nrows; i++) 
    for (j=0; j < E->ncols; j++) {
      if ((i < M->nrows)&&(j < M->ncols))
	E->val[i][j] = M->val[i][j];
      else{ // set 1 to the main diagonal
	if (i==j) 
	  E->val[i][j] = 1.0;
      }
    }

  return(E);

}


/* Decompose A_[m x n] into U_[m x m] S_[m x n] V^t_[n x n], where S
   contains the singular values and the matrices U and V^t are
   orthogonal matrices. 
*/

extern void sgesdd_( char* jobz, int* m, int* n, float* a, int* lda, float* s, float* u, int* ldu, float* vt, int* ldvt, float* work, int* lwork, int* iwork, int* info );


void iftSingleValueDecomp(iftMatrix *A, iftMatrix **U, iftMatrix **S, iftMatrix **Vt)
{
  int m = A->nrows, n = A->ncols, lda = m, ldu = m;
  int i,j,k,D=MIN(m,n);
  int ldvt = n, info, lwork, *iwork;
  float *work, wkopt;
  float *s, *u, *vt;
  float *a;

  /* Alloc work memory space */

  iwork = iftAllocIntArray(8*D);
  s     = iftAllocFloatArray(n);
  u     = iftAllocFloatArray(ldu*m);
  vt    = iftAllocFloatArray(ldvt*n);
  a     = iftAllocFloatArray(lda*n);

  /* Load input matrix A (Note that Fortran saves array information
     column by column) */

  k=0; 
  for (j=0; j < n; j++) { 
    for (i=0; i < m; i++) { 
      a[k]=A->val[i][j];
      k++;
    }
  }

  /* Query and allocate the optimal workspace */
  lwork = NIL;
  sgesdd_( "Singular vectors", &m, &n, a, &lda, s, u, &ldu, vt, &ldvt, &wkopt,
	   &lwork, iwork, &info );
  lwork = (int)wkopt;
  work  = iftAllocFloatArray(lwork);


  /* Compute SVD */
  sgesdd_( "Singular vectors", &m, &n, a, &lda, s, u, &ldu, vt, &ldvt, work,
	   &lwork, iwork, &info );

  iftFree(work);
  iftFree(iwork);
  iftFree(a);

  /* Check for convergence */
  if( info > 0 ) {
    iftError("The algorithm computing SVD failed to converge","iftSVD");
  }
  
  /* Get matrices after decomposition */

  *U  = iftCreateMatrix(D,m);

  *S  = iftCreateMatrix(D,1);
 
  *Vt = iftCreateMatrix(n,D); 

  k=0;
  //  if (m > n)  {
  //    for (j=0; j < m; j++) { 
    for (j=0; j < D; j++) { 
      for (i=0; i < m; i++) { 
	//	if (j < D){
	  (*U)->val[i][j] = u[k];
	  k++;
	  //	}
      }
    }
    /*
}else{
    for (j=0; j < m; j++) { 
      for (i=0; i < m; i++) { 
	if (j < D){
	  (*U)->val[i][j] = u[k];
	  k++;
	}
      }
    }
  }
    */
  for (k=0; k < D; k++)  
    (*S)->val[0][k] = s[k];
  
  k=0; 
  //  if (m > n) { 
    for (j=0; j < n; j++) { 
      //      for (i=0; i < n; i++) { 
      for (i=0; i < D; i++) { 
	//	if (i < D){
	  (*Vt)->val[i][j] = vt[k];
	  k++;
	  //	}
      }
    }
    /*
  } else {
    for (j=0; j < n; j++) { 
      for (i=0; i < n; i++) { 
	if (i < D){
	  (*Vt)->val[i][j] = vt[k];
	  k++;
	}
      }
    }
  }
    */

  iftFree(s);
  iftFree(u);
  iftFree(vt);
}

void iftNormalizeMatrix(iftMatrix *M)
{
  int   i,j;
  float norm=iftFrobeniusNorm(M);
  
  if (norm > 0.0){
    for (i=0; i < M->nrows; i++) 
      for (j=0; j < M->ncols; j++) {
	M->val[i][j] /= norm;
      }
  }
}

float iftFrobeniusNorm(iftMatrix *M)
{
  int   i,j;
  float norm=0.0;

  for (i=0; i < M->nrows; i++) 
    for (j=0; j < M->ncols; j++) {
      norm += M->val[i][j]*M->val[i][j];
    }

  norm = sqrtf(norm);
  return(norm);
}

/* Each column is considered a vector upto a last column */

char iftOrthogonalColumns(iftMatrix *A, int lastcol)
{
  int i,j,k;
  float aux=0;

  for (i=0; i <= lastcol; i++) {
    for (k=i+1; k <= lastcol; k++) {
      /* inner product between distinct columns must be 0 */ 
      for (j=0; j < A->nrows; j++) {
	aux += A->val[j][i]*A->val[j][k];
      }
    }
  }
  
  iftPrintMatrix(A);
  printf("aux %f\n",aux);

  if (fabs(aux) < Epsilon) 
    return(1);
  else
    return(0);
}

float iftInnerProdColumns(iftMatrix *A, int col1, int col2)
{
  int i;
  float inner=0.0;
  
  for (i=0; i < A->nrows; i++) 
    inner += A->val[i][col1]*A->val[i][col2];

  return(inner);
}


void iftSmoothFrontier(iftImageForest *fst, iftSet **Frontier, iftBMap *inFrontier, int num_smooth_iterations, float smooth_factor)
{
  iftImage  *prev_label, *next_label, *img=fst->img;
  iftFImage *prev_weight,*next_weight;
  float     *sum, norm_factor,max_membership, border_weight;
  int        l,i,p,q,r,max_label,iter,tmp;
  iftSet    *prev_frontier=*Frontier, *next_frontier=NULL, *Subtree=NULL;
  iftVoxel   u,v;
  iftAdjRel *A = fst->A;

  /* Initialization */

  prev_label  = iftCopyImage(fst->label);
  next_label  = iftCopyImage(prev_label);
  iftMaximumValue(prev_label); 
  sum         = iftAllocFloatArray(prev_label->maxval+1);
  prev_weight = iftCreateFImage(prev_label->xsize,prev_label->ysize,prev_label->zsize);
  next_weight = iftCreateFImage(next_label->xsize,next_label->ysize,next_label->zsize);

  for (p=0; p < prev_label->n; p++) {
    prev_weight->val[p]=next_weight->val[p]=1.0;
  }

  /* Smooth frontier and reset its path values */

  for (iter=0; iter < num_smooth_iterations; iter++) {

    fprintf(stdout,"Processing iteration %d\n",iter+1);

    while(prev_frontier != NULL){

      p = iftRemoveSet(&prev_frontier);

      iftInsertSet(&next_frontier,p);	
      u   = iftGetVoxelCoord(prev_label,p);
      
      for (l=0; l <= prev_label->maxval; l++){
	sum[l]= 0.0; 
      }

      norm_factor = 0.0;

      for (i=1; i < A->n; i++) {
	v = iftGetAdjacentVoxel(A,u,i);
	if (iftValidVoxel(prev_label,v)){
	  q = iftGetVoxelIndex(prev_label,v);
	  border_weight = ((float)img->val[q]/img->maxval);
	  border_weight = 1.0 / (1.0 + (smooth_factor * border_weight));
	  sum[prev_label->val[q]] += prev_weight->val[q]*border_weight;
	  norm_factor             += border_weight;
	  if (iftBMapValue(inFrontier,q)==0){ /* expand frontier */  
	    iftInsertSet(&next_frontier,q);
	    iftBMapSet1(inFrontier,q);
	  }
	}
      }

      for (l=0; l <= prev_label->maxval; l++)
	sum[l]  = sum[l] / norm_factor;
    
      max_membership = INFINITY_FLT_NEG; max_label=NIL;
      for (l=0; l <= prev_label->maxval; l++)
	if (sum[l] > max_membership){
	  max_membership = sum[l];
	  max_label      = l;
	}
	
      next_label->val[p]  = max_label;
      next_weight->val[p] = sum[max_label];
    }

    prev_frontier = next_frontier;
    next_frontier = NULL;

    for (r=0; r < prev_label->n; r++) {
      prev_weight->val[r] = next_weight->val[r];
      prev_label->val[r]  = next_label->val[r];
    }
  }


  iftFree(sum);
  iftDestroyFImage(&prev_weight);
  iftDestroyFImage(&next_weight);
  iftDestroyImage(&next_label);
  *Frontier  = prev_frontier;

  /* Fix the forest by first making available to be conquered all
     voxels whose label has changed and their subtrees  */

  for (p=0; p < fst->label->n; p++) {
    if ((fst->root->val[p]!=p)&&
  	(fst->label->val[p]!=prev_label->val[p])){
      iftInsertSet(&Subtree,p);
      fst->pathval->val[p]=INFINITY_INT;
      while (Subtree != NULL) {
  	r = iftRemoveSet(&Subtree);
  	u = iftGetVoxelCoord(fst->root,r);
  	for (i=1; i < A->n; i++) {
  	  v = iftGetAdjacentVoxel(A,u,i);
  	  if (iftValidVoxel(fst->root,v)){
  	    q = iftGetVoxelIndex(fst->root,v);
  	    if (fst->pred->val[q]==r){
  	      fst->pathval->val[q] = INFINITY_INT;
  	      iftInsertSet(&Subtree,q);
  	    }
  	  }
  	}
      }
    }
  }

  /* Insert in priority queue the seed voxels, which will be the
     neighbors with the same label of the region to be conquered. */

  for (p=0; p < fst->label->n; p++) {
    if (fst->pathval->val[p]==INFINITY_INT){
      u = iftGetVoxelCoord(fst->root,p);
      for (i=1; i < A->n; i++) {
  	v = iftGetAdjacentVoxel(A,u,i);
  	if (iftValidVoxel(fst->root,v)){
  	  q = iftGetVoxelIndex(fst->root,v);
  	  if ((fst->root->val[q]!=q)&&
  	      (prev_label->val[q]==prev_label->val[p])&&
  	      (fst->pathval->val[q]!=INFINITY_INT) )
  	    {
   	      if (fst->Q->L.elem[q].color==WHITE)
   		iftInsertGQueue(&fst->Q, q);
  	    }
  	}
      }
    }
  }

 
  iftDestroyImage(&fst->label);
  fst->label = prev_label;

  /* execute the IFT to reconstruct the forest under the new labeling
     constraint. This forest is not optimum, since this is a relaxed
     IFT, but it maintains the connectivity between roots and voxels
     of the same label, respecting the filtering process. */

  while(!iftEmptyGQueue(fst->Q)){
    p=iftRemoveGQueue(fst->Q);
    u=iftGetVoxelCoord(fst->img,p);
    
    for (i=1; i < A->n; i++){
      v  = iftGetAdjacentVoxel(A,u,i);
      if (iftValidVoxel(fst->img,v)){
  	q = iftGetVoxelIndex(fst->img,v);
  	if (fst->Q->L.elem[q].color != BLACK){
  	  tmp = MAX(fst->pathval->val[p],fst->img->val[q]);
  	  if (tmp < fst->pathval->val[q]){
  	    fst->root->val[q]    = fst->root->val[p];
  	    fst->pred->val[q]    = p;
  	    fst->label->val[q]   = fst->label->val[p];
  	    fst->pathval->val[q] = tmp;
  	    iftInsertGQueue(&fst->Q, q);
  	  }
  	}
      }
    }
  }

  /* Verify forest consistency: This can be removed when we decide for
     the final procedure. */

  for (p=0; p < fst->label->n; p++) {
    r = p;
    while (fst->root->val[r]!=r) {
      if (fst->label->val[r]!=fst->label->val[fst->pred->val[r]])
  	iftError("This cannot happen","here");
      r = fst->pred->val[r];
    }
  }
  
}



  //  float dist[8];
/*   if (n < 64) { */
/*     dist[0]=0.0; */
/*     for (int i=0; i < n; i++) */
/*       dist[0] += (f1[i]-f2[i])*(f1[i]-f2[i])*alpha[i]; */
/*   return(sqrtf(dist[0])); */

/*   }else{ */
/*   int delta = n/4; */
/*   omp_set_num_threads(4); */

/* #pragma omp parallel sections  */
/* { */
/* #pragma omp section  */
/*   { */
/*     dist[0]=0.0; */
/*     for (int i=0; i < delta; i++) */
/*       dist[0] += (f1[i]-f2[i])*(f1[i]-f2[i])*alpha[i]; */
/*   } */
/* #pragma omp section  */
/*   { */
/*     dist[1]=0.0; */
/*     for (int i=delta; i < 2*delta; i++) */
/*       dist[1] += (f1[i]-f2[i])*(f1[i]-f2[i])*alpha[i]; */
/*   } */
/* #pragma omp section  */
/*   { */
/*     dist[2]=0.0; */
/*     for (int i=2*delta; i < 3*delta; i++) */
/*       dist[2] += (f1[i]-f2[i])*(f1[i]-f2[i])*alpha[i]; */
/*   } */

/* #pragma omp section  */
/*   { */
/*     dist[3]=0.0; */
/*     for (int i=3*delta; i < 4*delta; i++) */
/*       dist[3] += (f1[i]-f2[i])*(f1[i]-f2[i])*alpha[i]; */
/*   } */
/*  } */
/*   } */
/*   for (int i=1; i < 4; i++)  */
/*     dist[0] += dist[i]; */
 
/*   return(sqrtf(dist[0])); */

iftMatrix *iftCompEigenVectors(iftDataSet *Z, int ncomps)
{
  iftMatrix *corr=NULL;
  iftMatrix *eigenvectors=NULL;
  float *mean=NULL,*aux=NULL,norm, proj;
  int s,i,j,comp,*sample;

  if (ncomps > Z->nfeats) 
    iftError("Invalid number of components","iftPrincipalComponents");
  
  /* Compute the mean value of each feature in the dataset */

  mean = iftAllocFloatArray(Z->nfeats);
  for (s=0; s < Z->nsamples; s++) {
    for (i=0; i < Z->nfeats; i++) {
      mean[i] += Z->sample[s].feat[i];
    }
  }

  for (i=0; i < Z->nfeats; i++){ 
    mean[i] /= Z->nsamples;  
  }

  /* Compute correlation (or covariance) matrix */

  corr = iftCovarianceMatrix(Z);

  /* Each column of this matrix will be one eigenvector */

  eigenvectors = iftCreateMatrix(ncomps,Z->nfeats);
  
  /* Execute the fast fixed-point algorithm to find the eigenvectors */

  aux    = iftAllocFloatArray(Z->nfeats);
  sample = iftAllocIntArray(Z->nsamples);
  for (s=0; s < Z->nsamples; s++) 
    sample[s]=s;

  iftRandomSeed();

  for (comp=0; comp < ncomps; comp++, s++) {

    /* initialize eigenvector with a random feature vector */

    i = iftRandomInteger(0,Z->nsamples-1-comp);
    s = sample[i];
    iftSwitchValues(&sample[i],&sample[Z->nsamples-1-comp]);

    for (i=0; i < Z->nfeats; i++)
      eigenvectors->val[i][comp] = Z->sample[s].feat[i];//-mean[i];


    do {


      /* Update eigenvector by multiplying it by the correlation (or
	 covariance) matrix */  
      /*      
      for(i=0; i < Z->nfeats; i++){
	aux[i] = 0.0;
	for(j=0; j < Z->nfeats; j++){
	  aux[i] += corr->val[j][i]*eigenvectors->val[j][comp];
	}
      }

      for(i=0; i < Z->nfeats; i++){
	eigenvectors->val[i][comp] = aux[i];
      }
      */
      /* Perform Gram-Schmidt Orthonormalization */
      
      for (j=0; j < comp; j++) {  
	proj = iftInnerProdColumns(eigenvectors,comp,j);
	for (i=0; i < Z->nfeats; i++) {
	  eigenvectors->val[i][comp] -= (proj * eigenvectors->val[i][j]);
	}
      }

      norm = 0.0;
      for(i=0; i < Z->nfeats; i++)
	norm += (eigenvectors->val[i][comp]*eigenvectors->val[i][comp]);
      norm = sqrtf(norm);
      
      if (norm > Epsilon){
	for(i=0; i < Z->nfeats; i++)      
	  eigenvectors->val[i][comp] /= norm;
      }

    } while(!iftOrthogonalColumns(eigenvectors,comp));
  }

  iftFree(mean);
  iftDestroyMatrix(&corr);
  iftFree(aux);
  iftFree(sample);
  return(eigenvectors);
}

  /* /\* Among the non-filtered voxels in the frontier, detect the seed */
  /*    voxels, which will be used to update the forest, insert them in */
  /*    the queue, and prepare the subtrees of the non-seed voxels to fix */
  /*    disconnections. *\/ */

  
  /* F = *Frontier; */
  /* while (F != NULL) { */
  /*   p = F->elem; */

  /*   if (fst->pathval->val[p]!=INFINITY_INT){ /\* non-filtered voxel *\/ */

  /*     u   = iftGetVoxelCoord(fst->label,p); */

  /*     disconnection = NIL; */
  /*     for (i=1; i < A->n; i++) { */
  /* 	v = iftGetAdjacentVoxel(A,u,i); */
  /* 	if (iftValidVoxel(fst->label,v)){ */
  /* 	  q = iftGetVoxelIndex(fst->label,v); */
  /* 	  if (iftBMapValue(inFrontier,q)==1){ */
  /* 	    if (fst->label->val[q]==fst->label->val[p]){ /\* p is a seed voxel *\/ */
  /* 	      disconnection = 0; */
  /* 	      iftInsertGQueue(&fst->Q,p); */
  /* 	      break; */
  /* 	    }else{ /\* p and q have distinct labels *\/ */
  /* 	      if (fst->pred->val[p]==q) /\* the subree of p might be */
  /* 					   disconnected */
  /* 					 *\/ */
  /* 		disconnection=1; */
  /* 	    } */
  /* 	  } */
  /* 	} */
  /*     } */

  /*     if (disconnection==1) { /\* Set label of the subtree of p to */
  /* 			         undefined and reset their path values to */
  /* 			         fix disconnection *\/ */
  /* 	iftInsertSet(&Subtree,p); */

  /* 	while (Subtree != NULL) { */
  /* 	  p = iftRemoveSet(&Subtree); */
  /* 	  u = iftGetVoxelCoord(fst->label,p); */

  /* 	  for (i=1; i < A->n; i++) { */
  /* 	    v = iftGetAdjacentVoxel(A,u,i); */
  /* 	    if (iftValidVoxel(fst->label,v)){ */
  /* 	      q = iftGetVoxelIndex(fst->label,v); */
  /* 	      if (fst->pred->val[q] == p){ */
  /* 		fst->label->val[q]   = NIL; */
  /* 		fst->pathval->val[q] = INFINITY_INT; */
  /* 		iftInsertSet(&Subtree,q); */
  /* 	      } */
  /* 	    } */
  /* 	  } */
  /* 	} */
  /*     } */
  /*   } */
  /*   F = F->next; */
  /* } */

  /* /\* Execute IFT to update forest and fix disconnected regions *\/ */

  /* while (!iftEmptyGQueue(fst->Q)){ */
  /*   p=iftRemoveGQueue(fst->Q); */
  /*   u=iftGetVoxelCoord(fst->img,p); */

  /*   for (i=1; i < A->n; i++){ */
  /*     v  = iftGetAdjacentVoxel(A,u,i); */
  /*     if (iftValidVoxel(fst->img,v)){ */
  /* 	q = iftGetVoxelIndex(fst->img,v); */
  /* 	if ((fst->label->val[q]==fst->label->val[p])|| */
  /* 	    (fst->label->val[q]==NIL)){ */
  /* 	  if (fst->Q->L.elem[q].color != BLACK){ */
	  
  /* 	    tmp = MAX(fst->pathval->val[p],fst->img->val[q]); */
  /* 	    if (tmp < fst->pathval->val[q]){ /\* For this path-value */
  /* 						function, this implies */
  /* 						that q has never been */
  /* 						inserted in Q. *\/ */
  /* 	      fst->root->val[q]    = fst->root->val[p]; */
  /* 	      fst->pred->val[q]    = p; */
  /* 	      fst->pathval->val[q] = tmp; */
  /* 	      iftInsertGQueue(&fst->Q, q); */
  /* 	    } */
  /* 	  } */
  /* 	} */
  /*     } */
  /*   } */
  /* } */


void iftWeightByGrayHistogram(iftDataSet *Z, int nbins)
{
  int    s;
  int    binsize, b;
  float *hist, max_weight;

  if (Z->img == NULL) 
    iftError("It is not an image dataset","iftWeightByGrayHistogram");
  
  Z->img->maxval = iftMaximumValue(Z->img);
  max_weight     = 0.0;
  hist           = iftGrayHistogram(Z->img,nbins);
  binsize        = (Z->img->maxval+1)/nbins + 1;

  for (s=0; s < Z->nsamples; s++) {
    b = Z->img->val[Z->voxel[s]]/binsize;
    Z->sample[s].weight = hist[b];
    if (Z->sample[s].weight > max_weight)
      max_weight = Z->sample[s].weight;
  }

  iftFree(hist);

  if (max_weight > 0.0)
    for (s=0; s < Z->nsamples; s++) {
      Z->sample[s].weight = (max_weight - Z->sample[s].weight)/max_weight;
      Z->sample[s].weight = ((MAXWEIGHT-1.0) * Z->sample[s].weight) + 1.0; 
    }
}

void iftWeightByColorHistogram(iftDataSet *Z, int nbins)
{
  int      s;
  int      binsize, xsize, xysize, b;
  iftColor YCbCr, RGB;
  float   *hist, max_weight;

  if (Z->img == NULL) 
    iftError("It is not an image dataset","iftWeightByColorHistogram");
  else{
    if (Z->img->Cb == NULL)
      iftError("Dataset image is not colored","iftWeightByColorHistogram");
  }
  
  hist    = iftColorHistogram(Z->img,nbins);
  xsize   = (int) powf(nbins,1.0/3.0);
  xysize  = xsize*xsize;
  binsize = 256/xsize + 1;

  max_weight     = 0.0;
  for (s=0; s < Z->nsamples; s++) {
    YCbCr.val[0] = Z->img->val[Z->voxel[s]];
    YCbCr.val[1] = Z->img->Cb[Z->voxel[s]];
    YCbCr.val[2] = Z->img->Cr[Z->voxel[s]];
    RGB          = iftYCbCrtoRGB(YCbCr);
    b            = ((RGB.val[0]/binsize) + 
		    (xsize  * (RGB.val[1]/binsize)) + 
		    (xysize * (RGB.val[2]/binsize)) );
    Z->sample[s].weight = hist[b];
    if (Z->sample[s].weight > max_weight)
      max_weight = Z->sample[s].weight;
  }

  iftFree(hist);

  if (max_weight > 0.0)
    for (s=0; s < Z->nsamples; s++) {
      Z->sample[s].weight = (max_weight - Z->sample[s].weight)/max_weight;
      Z->sample[s].weight = ((MAXWEIGHT-1.0) * Z->sample[s].weight) + 1.0; 
    }
}

int iftSelectSamplesPerClassByWeight(iftDataSet *Z, float perc, uchar status)
{
  int s, *sample=NULL, *count=NULL, i;
  int t, high, nsamples, c, *class_size=NULL, tot_nsamples;

  if (Z->nclasses == 0)
    iftError("There are no classes","iftSelectSamplesPerClassByWeight");

  if (perc == 1.0) {
    for (s=0; s < Z->nsamples; s++)
      Z->sample[s].status = status;
    if (status==TRAIN)
      Z->ntrainsamples = Z->nsamples;
    return(Z->nsamples);
  }


  iftRandomSeed();
  
  // Count number of available samples per class

  class_size = iftAllocIntArray(Z->nclasses+1);
  for (s=0; s < Z->nsamples; s++)
    if (Z->sample[s].status==TEST)
      class_size[Z->sample[s].truelabel]++;

  // Verify percentage and number of available samples per class
 
  for (c=1; c <= Z->nclasses; c++) {
    nsamples = (int)(perc*class_size[c]);
    if (nsamples >= class_size[c])
      nsamples = class_size[c];
    if (nsamples <= 0){
      fprintf(stderr,"For class %d\n",c);
      iftError("No available samples","iftSelectSamplesPerClassByWeight");
    }
  }

  // Store and randomly select available samples per class

  tot_nsamples = 0;
  for (c=1; c <= Z->nclasses; c++) {

    nsamples = (int)(perc*class_size[c]);
    if (nsamples >= class_size[c])
      nsamples = class_size[c];
    tot_nsamples += nsamples;
    sample = iftAllocIntArray(class_size[c]);
    count  = iftAllocIntArray(class_size[c]);
    t=0;
    for (s=0; s < Z->nsamples; s++)
      if ((Z->sample[s].truelabel==c)&&(Z->sample[s].status==TEST)){
	sample[t]=s;
	count[t]=(int)(100.0*(1.0 - Z->sample[s].weight/MAXWEIGHT));
	t++;
      }

    t = 0; high = class_size[c]-1;
    while (t < nsamples) {
      i = iftRandomInteger(0,high);
      s = sample[i];
      if (count[i]==0){
	Z->sample[s].status=status;
	iftSwitchValues(&sample[i],&sample[high]);
	iftSwitchValues(&count[i],&count[high]);
	t++; high--;
      }else{
	count[i]--;
      }
    }
    iftFree(count);
    iftFree(sample);
  }
  
  if (status==TRAIN)
    Z->ntrainsamples += tot_nsamples;

  iftFree(class_size);

  return(tot_nsamples);
}


int iftSelectSamplesByWeight(iftDataSet *Z, float perc, uchar status)
{ 
  int s, *sample, i, t, high, nsamples, navailsamples;
  int *count;

  fprintf(stdout,"Selecting training samples\n");

  // Verify if it is the trivial case of selecting all available
  // samples.

  if (perc == 1.0) {
    for (s=0; s < Z->nsamples; s++) 
      Z->sample[s].status = status;
    if (status==TRAIN)
      Z->ntrainsamples = Z->nsamples;  
    return(Z->nsamples);
  }

  // Count number of available samples

  navailsamples = 0;
  for (s=0; s < Z->nsamples; s++) 
    if (Z->sample[s].status==TEST)
      navailsamples++;
      

  // Verify number of available samples and percentage
  
  nsamples = (int)(navailsamples * perc);  

  if ((nsamples == 0) || (nsamples > navailsamples))
    iftError("No available samples or invalid percentage","iftSelectSamples");

  if (status==TRAIN) 
    Z->ntrainsamples += nsamples;


  // Store available samples
  
  sample = iftAllocIntArray(navailsamples); 
  count  = iftAllocIntArray(navailsamples); 

  t=0;
  for (s=0; s < Z->nsamples; s++) 
    if (Z->sample[s].status==TEST){    
      sample[t]=s;      
      count[t]=(int)(100.0*(1.0 - Z->sample[s].weight/MAXWEIGHT));
      t++;
    }

  // Randomly select available samples

  iftRandomSeed();

  t = 0; high = navailsamples-1;
  while (t < nsamples) {
    i = iftRandomInteger(0,high);
    s = sample[i];
    if (count[i]==0){
      Z->sample[s].status=status;
      iftSwitchValues(&sample[i],&sample[high]);
      iftSwitchValues(&count[i],&count[high]);
      t++; high--;
    }else{
      count[i]--;
    }
  }
  iftFree(count);
  iftFree(sample);
  return(nsamples);
}

#ifndef IFT_RANDOMINTEGERS_H_
#define IFT_RANDOMINTEGERS_H_

#include "iftCommon.h"
#include "iftSort.h"

typedef struct ift_randomintegers {
  int   *number;      /* set of random integers in [low,high] */
  int    low, high;   /* the lowest and highest numbers in the set */
  int    nelems;      /* total number of elements in the set */
  int    current;     /* current index to retrieve a number from the
			 set. It advances one by one in a circular
			 way. */
} iftRandomIntegers;

/* Creates random integers from low to high by repeating each number
   ntimes (uniform distribution). Other examples can be found in
   iftDataSet.c and iftSeeds.c. */

iftRandomIntegers *iftCreateRandomIntegers(int low, int high, int ntimes);
void               iftDestroyRandomIntegers(iftRandomIntegers **ri);
int                iftGetRandomInteger(iftRandomIntegers *ri);


#endif


#include "iftRandomIntegers.h"

iftRandomIntegers *iftCreateRandomIntegers(int low, int high, int ntimes)
{
  iftRandomIntegers *ri=NULL;
  int i, j, number;
  float *aux=NULL;

  if (low >= high)
    iftError("The lowest number must be lower than the highest one","iftCreateRandomIntegers");
    
  if (ntimes <= 0)
    iftError("Each number must occur at least one time","iftCreateRandomIntegers");

  ri = (iftRandomIntegers *)iftAlloc(1,sizeof(iftRandomIntegers));

  ri->low     = low;
  ri->high    = high;
  ri->current = 0;
  ri->nelems  = (high-low+1)*ntimes;

  ri->number  = iftAllocIntArray(ri->nelems); 

  /* Generate numbers with uniform distribution */

  j = 0;
  for (number=low; number <= high; number++) 
    for (i=0; i < ntimes; i++) {
      ri->number[j]=number;
      j++;
    }
  
  /* randomize the numbers */

  iftRandomSeed();

  aux   = iftAllocFloatArray(ri->nelems);
  for (i=0; i < ri->nelems; i++) {
      aux[i] = (float)((double) rand () / ((double) RAND_MAX));
  }
  iftFQuickSort(aux, ri->number, 0, ri->nelems-1, INCREASING); 
  iftFree(aux);

  return(ri);
}


void iftDestroyRandomIntegers(iftRandomIntegers **ri)
{
  iftRandomIntegers *aux = *ri;

  if (aux != NULL) {
    iftFree(aux->number);
    iftFree(aux);
    *ri = NULL;
  }
}

int iftGetRandomInteger(iftRandomIntegers *ri)
{
  int number = ri->number[ri->current];

  ri->current = (ri->current + 1)%ri->nelems;

  return(number);
}



/* Creates random integers with the indices of the dataset repeated as
   many times as the weight of their corresponding sample. */

iftRandomIntegers *iftRandomIntegersByWeight(iftDataSet *Z)
{
  iftRandomIntegers *ri=NULL;
  int i, j, s, number, ntimes;
  float *aux=NULL;

  ri = (iftRandomIntegers *)iftAlloc(1,sizeof(iftRandomIntegers));

  ri->low     = 0;
  ri->high    = Z->nsamples-1;
  ri->current = 0;
  ri->nelems  = 0;
  for (s=0; s < Z->nsamples; s++) {
    ri->nelems += (int)Z->sample[s].weight;
  }
  ri->number  = iftAllocIntArray(ri->nelems); 


  /* Generate numbers based on sample weight */

  j = 0;
  for (number=ri->low; number <= ri->high; number++) {
    ntimes =  (int)Z->sample[number].weight;
    for (i=0; i < ntimes; i++) {
      ri->number[j]=number;
      j++;
    }
  }

  /* randomize the numbers */

  iftRandomSeed();

  aux   = iftAllocFloatArray(ri->nelems);
  for (i=0; i < ri->nelems; i++) {
      aux[i] = (float)((double) rand () / ((double) RAND_MAX));
  }
  iftFQuickSort(aux, ri->number, 0, ri->nelems-1, INCREASING); 
  iftFree(aux);

  return(ri);
}

iftRandomIntegers *iftRandomIntegersByHistogram(iftImage *img)
{
  iftRandomIntegers *ri=NULL;
  int i, j, p, number, ntimes;
  float *aux=NULL;
  int   *histo=iftAllocIntArray(iftMaximumValue(img));
  
  ri = (iftRandomIntegers *)iftAlloc(1,sizeof(iftRandomIntegers));

  ri->low     = 0;
  ri->high    = img->n-1;
  ri->current = 0;
  ri->nelems  = 0;
  for (p=0; p < img->n; p++) 
    histo[img->val[p]]++;
  for (p=0; p < img->n; p++) 
    ri->nelems += histo[img->val[p]];

  ri->number  = iftAllocIntArray(ri->nelems); 

  /* Generate numbers based on image gray-scale histogram */

  j = 0;
  for (number=ri->low; number <= ri->high; number++) {
    ntimes = histo[img->val[number]];
    for (i=0; i < ntimes; i++) {
      ri->number[j]=number;
      j++;
    }
  }
  iftFree(histo);

  /* randomize the numbers */

  iftRandomSeed();

  aux   = iftAllocFloatArray(ri->nelems);
  for (i=0; i < ri->nelems; i++) {
      aux[i] = (float)((double) rand () / ((double) RAND_MAX));
  }
  iftFQuickSort(aux, ri->number, 0, ri->nelems-1, INCREASING); 
  iftFree(aux);

  return(ri);
}


/* ------ begin of relaxed IFT -------------------------------------- */

typedef struct ift_frontiernode {
  int     voxel;   /* voxel in the frontier set between objects */
  float  *prev_membership;  /* fuzzy membership map with respect to
			       each object: before filtering */ 
  float  *next_membership;  /* fuzzy membership map with respect to
			       each object: after filtering */ 
  iftSet *adj;              /* adjacent nodes in the frontier set */
} iftFrontierNode;
  
typedef struct ift_frontiergraph {
  iftFrontierNode *node; /* frontier set between objects */
  int nnodes;   /* number of nodes in the frontier set */
  iftAdjRel *A; /* adjacent relation between frontier nodes */
  iftImage  *label; /* label image with all objects */
  iftBMap   *in_frontier; /* indicates when a voxel is in the frontier set */
} iftFrontierGraph;

iftFrontierGraph *iftCreateFrontierGraph(iftImage *label, iftAdjRel *A);
void       iftDestroyFrontierGraph(iftFrontierGraph **graph);
void       iftExpandFrontier(iftFrontierGraph *graph);
iftImage  *iftSmoothObjects(iftImage *label, iftFImage *weight, int niters);

/* Create frontier graph with voxels between regions of distinct labels */

iftFrontierGraph *iftCreateFrontierGraph(iftImage *label, iftAdjRel *A)
{
  iftFrontierGraph *graph=NULL;
  iftSet *F=NULL;
  iftVoxel u,v;
  int s,t,p,q,i;
  
  label->maxval = iftMaximumValue(label);
  if (label->maxval < 1)
    iftError("Invalid label image","iftCreateFrontierGraph");
  graph = (iftFrontierGraph *) iftAlloc(1,sizeof(iftFrontierGraph));
  graph->A     = A;
  graph->label = label;
  graph->in_frontier  = iftCreateBMap(label->n);
  graph->nnodes = 0;

  /* Compute the frontier nodes  and its size */

  for (u.z=0; u.z < label->zsize; u.z++)
    for (u.y=0; u.y < label->ysize; u.y++)
      for (u.x=0; u.x < label->xsize; u.x++){
	p = iftGetVoxelIndex(label,u);
	if (label->val[p] > 0){
	  for (i=1; i < A->n; i++) {
	    v.x = u.x + A->dx[i];
	    v.y = u.y + A->dy[i];
	    v.z = u.z + A->dz[i];
	    if (iftValidVoxel(label,v)){
	      q = iftGetVoxelIndex(label,v);
	      if (label->val[q]!=label->val[p]){
		if (iftBMapValue(graph->in_frontier,p)==0){
		  iftInsertSet(&F,p);
		  iftBMapSet1(graph->in_frontier,p);
		  graph->nnodes++;
		}
		if (iftBMapValue(graph->in_frontier,q)==0){
		  iftInsertSet(&F,q);
		  iftBMapSet1(graph->in_frontier,q);
		  graph->nnodes++;
		}
	      }
	    }
	  } 
	}
      }

  /* initialize the frontier set */

  graph->node = (iftFrontierNode *)iftAlloc(graph->nnodes,sizeof(iftFrontierNode));
  if (graph->node==NULL)
    iftError(MSG1,"iftCreateFrontierGraph");
  
  s=0;
  while (F != NULL) { // node and membership information
    p = iftRemoveSet(&F);
    graph->node[s].voxel = p;
    graph->node[s].prev_membership = iftAllocFloatArray(label->maxval+1);
    graph->node[s].next_membership = iftAllocFloatArray(label->maxval+1);
    graph->node[s].prev_membership[label->val[p]] = 1.0; 
    graph->node[s].next_membership[label->val[p]] = 1.0; 
    graph->node[s].adj = NULL;
    s++;
  }

  for (s=0; s < graph->nnodes; s++) { // add adjacent nodes
    p = graph->node[s].voxel;
    u.x = iftGetXCoord(label,p);
    u.y = iftGetYCoord(label,p);
    u.z = iftGetZCoord(label,p);
    for (i=1; i < A->n; i++) {
      v.x = u.x + A->dx[i];
      v.y = u.y + A->dy[i];
      v.z = u.z + A->dz[i];
      if (iftValidVoxel(label,v)){
	q = iftGetVoxelIndex(label,v);
	if (iftBMapValue(graph->in_frontier,q)){
	  for (t=0; t < graph->nnodes; t++) {
	    if (graph->node[t].voxel==q){
	      iftInsertSet(&graph->node[s].adj,t);
	      break;
	    }
	  }
	}
      }
    }
  }

  return(graph);
}

/* Destroy frontier node list */

void iftDestroyFrontierNode(iftFrontierNode **node, int nnodes)
{
  iftFrontierNode *aux=*node;
  int i;

  if (aux != NULL) {
    for(i=0; i < nnodes; i++){
      iftFree(aux[i].prev_membership);
      iftFree(aux[i].next_membership);
      iftDestroySet(&(aux[i].adj));
    }
    iftFree(aux);
    *node = NULL;
  }  
}

/* Destroy frontier graph */

void iftDestroyFrontierGraph(iftFrontierGraph **graph)
{
  iftFrontierGraph *aux=*graph;

  if (aux != NULL) {
    iftDestroyBMap(&(aux->in_frontier));
    iftDestroyFrontierNode(&(aux->node),aux->nnodes);
    iftFree(aux);
    *graph = NULL;
  }
  
}

/* Expand the frontier set */

void iftExpandFrontier(iftFrontierGraph *graph)
{
  iftAdjRel *A=graph->A;
  iftImage  *label=graph->label;
  iftSet *F=NULL;
  iftVoxel u,v;
  int s,t,p,q,i,l;
  int nnodes;
  iftFrontierNode *node;

  /* Compute the expansion nodes and their number */

  nnodes=0;
  for (s=0; s < graph->nnodes; s++) { 
    p = graph->node[s].voxel;
    u.x = iftGetXCoord(label,p);
    u.y = iftGetYCoord(label,p);
    u.z = iftGetZCoord(label,p);
    for (i=1; i < A->n; i++) {
      v.x = u.x + A->dx[i];
      v.y = u.y + A->dy[i];
      v.z = u.z + A->dz[i];
      if (iftValidVoxel(label,v)){
	q = iftGetVoxelIndex(label,v);
	if (iftBMapValue(graph->in_frontier,q)==0){
	  iftInsertSet(&F,q); nnodes++;
	  iftBMapSet1(graph->in_frontier,q);
	}
      }
    }
  }
  
  /* initialize the expanded frontier set */

  node = (iftFrontierNode *)iftAlloc(nnodes+graph->nnodes,sizeof(iftFrontierNode));
  if (node==NULL)
    iftError(MSG1,"iftExpandFrontier");
  
  for (s=0; s < graph->nnodes; s++) { /* copy information from previous nodes */
    node[s].voxel = graph->node[s].voxel;
    node[s].prev_membership = iftAllocFloatArray(label->maxval+1);
    node[s].next_membership = iftAllocFloatArray(label->maxval+1);
    // this is correct: next_membership initializes both, prev and next
    for (l=0; l <= label->maxval; l++){
      node[s].prev_membership[l] = graph->node[s].next_membership[l];  
      node[s].next_membership[l] = graph->node[s].next_membership[l];
    }
    node[s].adj = NULL;
    while (graph->node[s].adj != NULL){
      t = iftRemoveSet(&(graph->node[s].adj));
      iftInsertSet(&(node[s].adj),t);
    }
  }

  while (F != NULL) { // add new nodes and their information
    p = iftRemoveSet(&F);
    node[s].voxel = p;
    node[s].prev_membership = iftAllocFloatArray(label->maxval+1);
    node[s].next_membership = iftAllocFloatArray(label->maxval+1);
    node[s].prev_membership[label->val[p]] = 1.0; 
    node[s].next_membership[label->val[p]] = 1.0; 
    node[s].adj = NULL;
    s++;
  }

  iftDestroyFrontierNode(&(graph->node),graph->nnodes); // update frontier node list
  graph->node   = node;
  graph->nnodes = graph->nnodes + nnodes;
 
  // update adjacent list in an incremental way

  for (s=graph->nnodes-nnodes; s < graph->nnodes; s++){ 
    p = graph->node[s].voxel;
    u.x = iftGetXCoord(label,p);
    u.y = iftGetYCoord(label,p);
    u.z = iftGetZCoord(label,p);
    for (i=1; i < A->n; i++) {
      v.x = u.x + A->dx[i];
      v.y = u.y + A->dy[i];
      v.z = u.z + A->dz[i];
      if (iftValidVoxel(label,v)){
	q = iftGetVoxelIndex(label,v);
	if (iftBMapValue(graph->in_frontier,q)){
	  for (t=0; t < graph->nnodes; t++) { 
	    if (graph->node[t].voxel==q){
	      iftUnionSetElem(&graph->node[s].adj,t);
	      iftUnionSetElem(&graph->node[t].adj,s); /* update the adjacent
						      list of the
						      previous
						      node too */
	      break;
	    }
	  }
	}
      }
    }
  }

}

/* Smooth objects (relaxed IFT) */

iftImage  *iftSmoothObjects(iftImage *label, iftFImage *weight, int niters)
{
  iftAdjRel *A;
  iftFrontierGraph *graph;
  float *sum, norm_factor,max_membership;
  int s,t,l,p,q,max_label, iter;
  iftSet *adj;
  iftImage *nlabel=iftCopyImage(label);

  if (niters < 1) 
    iftError("Invalid number of iterations","iftSmoothObjects");

  if (label->zsize == 1) // 2D
    A = iftCircular(sqrtf(2.0));
  else // 3D
    A = iftSpheric(sqrtf(3.0));


  graph = iftCreateFrontierGraph(label,A);


  sum = iftAllocFloatArray(label->maxval+1);

  for (iter=0; iter < niters; iter++) {
    printf("Processing iteration %d\n",iter+1);

    if (iter > 0){
      printf("Expanding frontier set\n");
      iftExpandFrontier(graph);
    }

    for (s=0; s < graph->nnodes; s++) {

      p = graph->node[s].voxel;

      for (l=0; l <= label->maxval; l++)
	sum[l]=0;
      norm_factor = 0;
    
      for (adj=graph->node[s].adj; adj != NULL; adj = adj->next){
	t = adj->elem;
	q = graph->node[t].voxel;

	for (l=0; l <= label->maxval; l++){
	  sum[l] +=  graph->node[t].prev_membership[l]*weight->val[q];
	  norm_factor += weight->val[q];
	}
      }

      for (l=0; l <= label->maxval; l++){
	graph->node[s].next_membership[l]  = sum[l] / norm_factor;
      }
    }
    
  }

  /* Final labeling */
  
  for (s=0; s < graph->nnodes; s++) {
    p = graph->node[s].voxel;

    max_membership = INFINITY_FLT_NEG; max_label=NIL;
    for (l=0; l <= label->maxval; l++)
      if (graph->node[s].next_membership[l] > max_membership){
	max_membership = graph->node[s].next_membership[l];
	max_label      = l;
      }
    nlabel->val[p] = max_label;
  }

  iftFree(sum);
  iftDestroyFrontierGraph(&graph);
  iftDestroyAdjRel(&A);

  return(nlabel);
}


/* ----- end of relaxed IFT -------------------------------------------- */

/* iftSet *iftTreeRemoval(iftImageForest *fst, iftLabeledSet *seed) */
/* { */
/*   int      i, p, q, n=fst->img->n, V0; */
/*   iftVoxel u,v; */
/*   iftAdjRel *A=fst->A; */
/*   iftLabeledSet *S; */
/*   iftSet   *Frontier=NULL; */
/*   iftBMap  *RemRoot = iftCreateBMap(n), *inFrontier = iftCreateBMap(n); */
/*   iftImage *pathval=fst->pathval,*root=fst->root,*pred=fst->pred; */
/*   iftImage *label=fst->label; */
/*   iftImage *img=fst->img; */
/*   int      *RemNode=iftAllocIntArray(n), first=0, last=0; */

/*   if (fst->Q->C.removal_policy == MINVALUE) */
/*     V0 = INFINITY_INT; */
/*   else // MAXVALUE */
/*     V0 = INFINITY_INT_NEG; */

/*   /\* Mark all roots, whose trees will be removed, reinitialize path */
/*   and predecessor values of those roots, and insert them in RemNode, in */
/*   order to reinitialize the path and predecessor values of the */
/*   remaining nodes of each removed trees. *\/ */

/*   S = seed; */
/*   while (S != NULL) { */
/*     if ((S->label == NIL)&& */
/* 	(pathval->val[S->elem] != V0)){ // remove only trees of */
/*                                   // distinct labels in regions */
/*                                   // already conquered */
/*       p = root->val[S->elem]; // get a removal root p */
    
/*       if (iftBMapValue(RemRoot,p)==0){ // p is not in RemRoot */
/* 	pathval->val[p] = V0;   */
/* 	RemNode[last]=p; last++; */
/* 	iftBMapSet1(RemRoot,p); */
/*       } */
/*     } */
/*     S = S->next; */
/*   } */

/*   /\* Forest removal: reinitialize nodes of the removed trees and */
/*      create frontier set for the next DIFT. *\/ */

/*   while (first != last) { */
/*     p = RemNode[first]; first++; */
/*     u.x = iftGetXCoord(img,p); */
/*     u.y = iftGetYCoord(img,p); */
/*     u.z = iftGetZCoord(img,p); */
/*     for (i=1; i < A->n; i++) { */
/*       v.x = u.x + A->dx[i]; */
/*       v.y = u.y + A->dy[i]; */
/*       v.z = u.z + A->dz[i]; */
/*       if (iftValidVoxel(img,v)){ */
/* 	q   = iftGetVoxelIndex(img,v); */

/* 	if (pred->val[q]==p){ */
/* 	  pathval->val[q]=V0; pred->val[q] = NIL; */
/* 	  RemNode[last]=q; last++; */
/* 	}else{ // frontier must contain only pixels of the same removed label */
/* 	  if (iftBMapValue(RemRoot,root->val[q])==0){ */
/* 	    if (label->val[q]==label->val[p]) { */
/* 	      if (iftBMapValue(inFrontier,q)==0){ */
/* 		iftInsertSet(&Frontier,q); iftBMapSet1(inFrontier,q); */
/* 	      } */
/* 	    } */
/* 	  } */
/* 	} */
/*       } */
/*     } */
/*   } */
/*   iftDestroyBMap(&inFrontier); */
/*   iftDestroyBMap(&RemRoot); */
/*   iftFree(RemNode); */

/*   return(Frontier); */
/* } */


/* iftSet *iftCompRemovalWithFrontier(iftImageForest *fst, iftLabeledSet *seed) */
/* { */
/*   int        i, p, q, n=fst->img->n, V0; */
/*   iftVoxel   u,v; */
/*   iftAdjRel *A=fst->A; */
/*   iftLabeledSet *S=NULL; */
/*   iftSet   *Frontier=NULL; */
/*   iftImage *pathval=fst->pathval,*pred=fst->pred; */
/*   iftImage *img=fst->img, *label=fst->label; */
/*   iftFIFO  *F=iftCreateFIFO(n); */

/*   if (fst->Q->C.removal_policy == MINVALUE) */
/*     V0 = INFINITY_INT; */
/*   else // MAXVALUE */
/*     V0 = INFINITY_INT_NEG; */

/*   /\* Remove all connected components with at least one removal seed */
/*      and insert the removal seed in another set to find the frontier */
/*      voxels afterwards. *\/ */

/*   S = seed; */
/*   while (S != NULL) { */
/*     if ((S->label == NIL)&&(pathval->val[S->elem] != V0)) { // Removal marker must be in regions not removed yet */
/*       pathval->val[S->elem] = V0;  pred->val[S->elem] = NIL; // Remove element */
/*       // Remove its largest connected component */
/*       iftInsertFIFO(F,S->elem); */
/*       while (!iftEmptyFIFO(F)) { */
/* 	p = iftRemoveFIFO(F); */
/* 	u = iftGetVoxelCoord(img,p); */
/* 	for (i=1; i < A->n; i++) { */
/* 	  v = iftGetAdjacentVoxel(A,u,i); */
/* 	  if (iftValidVoxel(img,v)){ */
/* 	    q   = iftGetVoxelIndex(img,v); */
/* 	    if (pathval->val[q] != V0){ */
/* 	      if (label->val[q]==label->val[p]){ */
/* 		pathval->val[q] = V0;  pred->val[q] = NIL; // Remove element */
/* 		iftInsertFIFO(F,q); */
/* 	      } */
/* 	    } */
/* 	  } */
/* 	} */
/*       } */
/*       // Reset FIFO without reinitializing the color array */
/*       F->first = F->last = 0; */
/*     } */
/*     S = S->next; */
/*   } */

/*   iftDestroyFIFO(&F); */

/*   /\* Find the frontier voxels of non-removed regions *\/ */

/*   for (p=0; p < img->n; p++) { */
/*     if (pathval->val[p] != V0){ */
/*       u = iftGetVoxelCoord(img,p); */
/*       for (i=1; i < A->n; i++) { */
/* 	v = iftGetAdjacentVoxel(A,u,i); */
/* 	if (iftValidVoxel(img,v)){ */
/* 	  q   = iftGetVoxelIndex(img,v); */
/* 	  if (pathval->val[q] == V0){ // p is a frontier voxel */
/* 	    iftInsertSet(&Frontier,p);  */
/* 	    break; */
/* 	  } */
/* 	} */
/*       } */
/*     } */
/*   } */
  
/*   return(Frontier); */
/* } */

/* void iftRelabelTrees(iftImageForest *fst, iftLabeledSet *seed) */
/* { */
/*   int      i, p, q, n=fst->img->n, l; */
/*   iftVoxel u,v; */
/*   iftAdjRel *A=fst->A; */
/*   iftLabeledSet *S; */
/*   iftImage *img=fst->img, *label=fst->label; */
/*   iftFIFO  *F=iftCreateFIFO(n); */
/*   iftBMap  *Relabeled=iftCreateBMap(n); */

/*   // Relabel the connected component whose label has changed in the seed set */

/*   S = seed; */
/*   while (S != NULL) { */
/*     if ((S->label != label->val[S->elem])&& */
/* 	(iftBMapValue(Relabeled,S->elem)==0)) {  */
/*       l = label->val[S->elem]; */
/*       label->val[S->elem]=S->label; */
/*       iftBMapSet1(Relabeled,S->elem); */
/*       iftInsertFIFO(F,S->elem); */
/*       while (!iftEmptyFIFO(F)) { */
/* 	p = iftRemoveFIFO(F); */
/* 	u = iftGetVoxelCoord(img,p); */
/* 	for (i=1; i < A->n; i++) { */
/* 	  v = iftGetAdjacentVoxel(A,u,i); */
/* 	  if (iftValidVoxel(img,v)){ */
/* 	    q   = iftGetVoxelIndex(img,v); */
/* 	    if ((label->val[q]==l)&& */
/* 		(iftBMapValue(Relabeled,q)==0)){ */
/* 	      label->val[q]=S->label;	       */
/* 	      iftBMapSet1(Relabeled,q); */
/* 	      iftInsertFIFO(F,q); */
/* 	    } */
/* 	  } */
/* 	} */
/*       } */
/*       iftResetFIFO(F); */
/*     } */
/*     S = S->next; */
/*   } */

/*   iftDestroyFIFO(&F); */
/*   iftDestroyBMap(&Relabeled); */

/* } */

#ifndef IFT_SEGMENT_H_
#define IFT_SEGMENT_H_

#include "iftCommon.h"
#include "iftSet.h"
#include "iftAdjacency.h"
#include "iftImage.h"

typedef struct ift_chaincode { 
  int *code;         // list of indices to reach adjacent pixels 
                     // For instance, (A->dx[code[0]],A->dy[code[0]])
                     // is the displacement to go from the (x,y) 
                     // coordinates of the first point to the next point.
  int  first, last;  // its terminal points
  int  n;            // number of elements in code
} iftChainCode;
 
typedef struct ift_segment {
  iftSet *S;        // list of segment points
  int first, last;  // its terminal points
  iftSet *J;        // its junction   
} iftSegment;

iftSegment   *iftCreateSegment();
void          iftDestroySegment(iftSegment **seg);
iftChainCode *iftCreateSegChainCode(iftSegment *seg, iftImage *img, iftAdjRel *A);
void          iftDestroyChainCode(iftChainCode **chain);
float         iftSegmentLength(iftSegment *seg, iftImage *img, iftAdjRel *A, float scale, float smooth_factor);
float         iftChainCodeLength(int *code, int start, int end, int first, int last, iftImage *img, iftAdjRel *A, float scale, float smooth_factor);
int           iftNumberOfJunctionPoints(iftSegment *seg);
iftSegment   *iftGetSubSegment(iftSegment *seg, int ss);
void          iftPrintSegment(iftSegment *seg);


#endif

#include "iftSegment.h"

iftSegment *iftCreateSegment()
{
  iftSegment *seg=(iftSegment *)iftAlloc(1, sizeof(iftSegment));
  seg->S = NULL;
  seg->J = NULL;
  seg->first = NIL;
  seg->last  = NIL;
  return(seg);
}

void iftDestroySegment(iftSegment **seg)
{
  iftSegment *aux=*seg;

  if (aux != NULL) {
    iftDestroySet(&(aux->S));
    iftDestroySet(&(aux->J));
    iftFree(aux);
    *seg = NULL;
  }
}

iftChainCode *iftCreateSegChainCode(iftSegment *seg, iftImage *img, iftAdjRel *A)
{
  iftChainCode *chain = (iftChainCode *) iftAlloc(1, sizeof(iftChainCode));
  iftVoxel      u,v;
  int           i, j, dx, dy, dz;
  iftSet       *S;
  
  chain->first = seg->first;
  chain->last  = seg->last;

  S = seg->S; chain->n=0;
  while (S != NULL) { // compute number of elements 
    chain->n++;
    S = S->next;
  }

  chain->n   -= 1;
  chain->code = iftAllocIntArray(chain->n);

  S   = seg->S; 
  u.x = iftGetXCoord(img,S->elem);
  u.y = iftGetYCoord(img,S->elem);
  u.z = iftGetZCoord(img,S->elem);

  j   = 0;
  while (S->next != NULL) {
    v.x = iftGetXCoord(img,S->next->elem);
    v.y = iftGetYCoord(img,S->next->elem);
    v.z = iftGetZCoord(img,S->next->elem);
    for (i=1; i < A->n; i++) {
      dx = v.x - u.x;
      dy = v.y - u.y;
      dz = v.z - u.z;
      if ((dx==A->dx[i])&&(dy==A->dy[i])&&(dz==A->dz[i])){
	chain->code[j]=i; j++;
	break;
      }
    }
    u.x=v.x; u.y=v.y; u.z=v.z;
    S = S->next;
  }
  if (j != chain->n)
    iftError("Chain code extraction failed","iftCreateSegChainCode");
  
  return(chain);
}


void iftDestroyChainCode(iftChainCode **chain)
{
  iftChainCode *aux=*chain;

  if (aux != NULL) {
    iftFree(aux->code);
    iftFree(aux);
    *chain = NULL;
  }
}


float iftSegmentLength(iftSegment *seg, iftImage *img, iftAdjRel *A, float scale, float smooth_factor)
{
  iftChainCode *chain=iftCreateSegChainCode(seg,img,A);
  float length = iftChainCodeLength(chain->code, 0, chain->n-1, chain->first, chain->last, img, A, scale, smooth_factor);

  iftDestroyChainCode(&chain);

  return(length);
}

float iftChainCodeLength(int *code, int start, int end, int first, int last, iftImage *img, iftAdjRel *A, float scale, float smooth_factor)
{
  iftVoxel P0, P1, P2, P, Pm;
  float    dist, dmax=0.0, P1P2, L1, L2;
  int      i, imax;
    
 
  P1.x = iftGetXCoord(img,first);
  P1.y = iftGetYCoord(img,first);
  P1.z = iftGetZCoord(img,first);

  P2.x = iftGetXCoord(img,last);
  P2.y = iftGetYCoord(img,last);
  P2.z = iftGetZCoord(img,last);

  P1P2 = iftVoxelDistance(P1,P2);

  /* Find the most distance point to the line P1P2, excluding the
     first and last points */

  imax = NIL;
  Pm.x = Pm.y = Pm.z = NIL;
  P.x = P1.x;   P.y = P1.y; P.z = P1.z; 
  for (i=start; i < end; i++) {
    P0.x = P.x + A->dx[code[i]];
    P0.y = P.y + A->dy[code[i]];
    P0.z = P.z;
    dist = iftPointLineDist2D(P0, P1, P2, P1P2);
    if (dist > dmax) {
      dmax = dist;
      imax = i+1;
      Pm.x = P0.x;  Pm.y = P0.y; Pm.z = P0.z; 
    }
    P.x = P0.x; P.y = P0.y; P.z = P0.z; 
  }

  if ((dmax < smooth_factor)&&(dmax >= 0)){ // end of recursion
    return(P1P2/scale);
  }else{
    i  = iftGetVoxelIndex(img,Pm);
    L1 = iftChainCodeLength(code, start, imax, first, i, img, A, scale, smooth_factor);
    L2 = iftChainCodeLength(code, imax, end, i, last, img, A, scale, smooth_factor);
    return(L1+L2);
  }
}

int iftNumberOfJunctionPoints(iftSegment *seg)
{
  iftSet *J=seg->J;
  int n;

  n=0;
  while (J!=NULL){ 
    n++;
    J = J->next;
  }
  return(n);
}

iftSegment *iftGetSubSegment(iftSegment *seg, int ss)
{
  iftSegment *subseg=iftCreateSegment();
  iftSet *S,*J;
  int i, nssegs=iftNumberOfJunctionPoints(seg)+1;

  if ((ss < 1)&&(ss > nssegs))
    iftError("Invalid subsegment number","iftGetSubSegment");
      
  if (ss==1){
    subseg->first = seg->first;
    subseg->last  = seg->J->elem;
  }else{
    if (ss==nssegs){
      J = seg->J;
      while (J->next != NULL)
	J=J->next;
      subseg->first = J->elem;
      subseg->last  = seg->last;
    }else{
      i=2;
	  J = seg->J;
	  while (i != ss){
	    J = J->next;
	    i++;
	  }
	  subseg->first = J->elem; 
	  subseg->last  = J->next->elem;
    }
  }
  
  S = seg->S;
  while(S->elem != subseg->first) 
    S = S->next;
  while(S->elem != subseg->last) {
    iftInsertSet(&subseg->S,S->elem);
    S = S->next;
  }
  iftInsertSet(&subseg->S,subseg->last);
  iftInvertSet(&subseg->S);
  
  return(subseg);
}

void iftPrintSegment(iftSegment *seg)
{
  iftSet *S;

  printf("first %d\n",seg->first);
  printf("last  %d\n",seg->last);
  S = seg->S;
  while (S != NULL) {
    printf("%d ",S->elem);
    S = S->next;
  }
}


/* Set *iftGetLine(int *code, int n, int first, int last, iftImage *img, iftAdjRel *A) */
/* { */
/*   int dx = abs(pn.x - p1.x), dy = abs (pn.y - p1.y); */
/*   int pk = 2*dy-dx; /\* decision variable for lines y=mx+b where -1 <= m <= 1 *\/ */
/*   int twody = 2*dy , twodydx = 2*(dy-dx); */
/*   int qk = 2*dx-dy; /\* decision variable for lines y=mx+b where m < -1 */
/*                       or m > 1 *\/ */
/*   int twodx = 2*dx ,twodxdy = 2*(dx-dy); */
/*   float d,dd; */
/*   int npts; */
/*   Pixel p,pend; */
/*   bool increase; /\* true for quadrants 1 and 2 and false for quadrants */
/* 		    3 and 4 *\/ */
  
/*   if(pn.x==p1.x)  */
/*     increase=true; */
/*   else  */
/*     if(pn.x>p1.x){ */
/*       if(pn.y>p1.y) /\* quadrant 1 *\/  */
/* 	increase=true; */
/*       else /\* quadrant 4 *\/ */
/* 	increase=false; */
/*     } else { /\* pn.x<p1.x *\/ */
/*       if(pn.y>p1.y) /\* quadrant 3 *\/ */
/* 	increase=false; */
/*       else /\* quadrant 4 *\/ */
/* 	increase=true; */
/*     } */
  
/*   /\* Find start and end points *\/ */
  
/*  if(dx >= dy){ /\* -1 <= m <= 1 *\/ */
/*    if(p1.x > pn.x) { */
/*      p.x    = pn.x; */
/*      p.y    = pn.y; */
/*      d      = dn; */
/*      npts   = p1.x - pn.x + 1;      */
/*      dd = (d1-dn)/(npts-1); */
/*      pend.x = p1.x; */
/*    } */
/*    else { /\* p1.x <= pn.x *\/ */
/*      p.x = p1.x; */
/*      p.y = p1.y; */
/*      d   = d1; */
/*      npts = pn.x - p1.x + 1; */
/*      dd = (dn-d1)/(npts-1); */
/*      pend.x = pn.x; */
/*    } */
/*    DrawPixel(img,p,d,zbuff); */
/*    while(p.x < pend.x){ */
/*      p.x++; */
/*      if(pk < 0){ */
/*        pk +=twody; */
/*      } else{ */
/*        if(increase)  */
/* 	 p.y++; */
/*        else  */
/* 	 p.y--; */
/*        pk += twodydx; */
/*      } */
/*      d += dd;  */
/*      DrawPixel(img,p,d,zbuff); */
/*    } */
/*  } else { /\* dx < dy: m > 1 or m < -1 *\/ */
  
/*    if(p1.y > pn.y) { */
/*      p.x = pn.x; */
/*      p.y = pn.y; */
/*      d      = dn; */
/*      npts   = p1.y - pn.y + 1;       */
/*      dd = (d1-dn)/(npts-1); */
/*      pend.y = p1.y; */
/*    } */
/*    else { /\* p1.y <= pn.y *\/ */
/*      p.x = p1.x; */
/*      p.y = p1.y; */
/*      d      = d1; */
/*      npts   = pn.y - p1.y + 1;      */
/*      dd = (dn-d1)/(npts-1); */
/*      pend.y = pn.y; */
/*    }	  */
/*    DrawPixel(img,p,d,zbuff); */
/*    while( p.y < pend.y){ */
/*      p.y++; */
/*      if( qk < 0){ */
/*        qk +=twodx; */
/*      } else{ */
/*       if(increase)  */
/* 	p.x++; */
/*       else  */
/* 	p.x--; */
/*       qk += twodxdy; */
/*      } */
/*      d += dd; */
/*      DrawPixel(img,p,d,zbuff); */
/*     } */
/*  } */

/* } */


#ifndef IFT_CONTOURS_H_
#define IFT_CONTOURS_H_

#include "iftCommon.h"
#include "iftAdjacency.h"
#include "iftImage.h"
#include "iftSet.h"
#include "iftLIFO.h"
#include "iftSeeds.h"

typedef struct ift_contour { 
  int *ccode;        // chain code: list of indices to reach adjacent pixels 
                     // For instance, (A->dx[code[0]],A->dy[code[0]])
                     // is the displacement to go from the (x,y) 
                     // coordinates of the first point to the next point.
  int  first, last;  // its terminal points
  int  npts;         // number of points in chain code (on the contour)
  int  z;            // z-coordinate of the contour points
} iftContour;

typedef struct ift_contours {
  iftContour *cnt;       // all object contours in a slice by slice fashion
  int         ncontours; // number of contours
  iftImage   *label;     // labeled image whose domain contains the contours
} iftContours;

iftContours *iftExtractContours(iftImage *label);
void         iftDestroyContours(iftContours **cnts);
iftImage    *iftSmoothContours(iftContours *cnts, float smooth_factor);

#endif

#include "iftContours.h"

/*------------------------- Private Methods -------------------------------*/

void iftDrawLine(iftImage *img, iftVoxel p1, iftVoxel pn, int label) 
{ 
  int dx = abs(pn.x - p1.x), dy = abs (pn.y - p1.y);
  int pk = 2*dy-dx; /* decision variable for lines y=mx+b where -1 <= m <= 1 */
  int twody = 2*dy , twodydx = 2*(dy-dx);
  int qk = 2*dx-dy; /* decision variable for lines y=mx+b where m < -1
                      or m > 1 */
  int twodx = 2*dx ,twodxdy = 2*(dx-dy);
  iftVoxel p,pend;
  char increase; /* true for quadrants 1 and 2 and false for quadrants
		    3 and 4 */
  
  if(pn.x==p1.x)
    increase=1;
  else{
    if(pn.x>p1.x){
      if(pn.y>p1.y) /* quadrant 1 */
	increase=1;
      else /* quadrant 4 */
	increase=0;
    } else { /* pn.x<p1.x */
      if(pn.y>p1.y) /* quadrant 3 */
	increase=0;
      else /* quadrant 4 */
	increase=1;
    }
  }

  /* Find start and end points */
  
 if(dx >= dy){ /* -1 <= m <= 1 */
   if(p1.x > pn.x) {
     p.x    = pn.x;
     p.y    = pn.y;
     p.z    = pn.z;
     pend.x = p1.x;
   }
   else { /* p1.x <= pn.x */
     p.x = p1.x;
     p.y = p1.y;
     p.z = p1.z;
     pend.x = pn.x;
   }

   img->val[iftGetVoxelIndex(img,p)] = label;
   while(p.x < pend.x){
     p.x++;
     if(pk < 0){
       pk +=twody;
     } else{
       if(increase)
	 p.y++;
       else
	 p.y--;
       pk += twodydx;
     }
     img->val[iftGetVoxelIndex(img,p)]=label;
   }

 } else { /* dx < dy: m > 1 or m < -1 */
  
   if(p1.y > pn.y) {
     p.x = pn.x;
     p.y = pn.y;
     p.z = pn.z;
     pend.y = p1.y;
   }
   else { /* p1.y <= pn.y */
     p.x = p1.x;
     p.y = p1.y;
     p.z = p1.z;
     pend.y = pn.y;
   }
  
   img->val[iftGetVoxelIndex(img,p)]=label;
   while( p.y < pend.y){
     p.y++;
     if( qk < 0){
       qk +=twodx;
     } else{
      if(increase)
	p.x++;
      else
	p.x--;
      qk += twodxdy;
     }
     img->val[iftGetVoxelIndex(img,p)]=label;
   }
 }

}


/* Computes a smooth contour from its chain code and draws its label
   in img */

void iftSmoothContour(int *ccode, int i0, int in, int first, int last, iftImage *img, iftAdjRel *A, int label, float smooth_factor)
{
  iftVoxel P0, P1, P2, P, Pm;
  float    dist, dmax=INFINITY_FLT_NEG, P1P2;
  int      i, imax, index;
    
  P1.x = iftGetXCoord(img,first);
  P1.y = iftGetYCoord(img,first);
  P1.z = iftGetZCoord(img,first);
  
  P2.x = iftGetXCoord(img,last);
  P2.y = iftGetYCoord(img,last);
  P2.z = iftGetZCoord(img,last);
    
  P1P2 = iftEuclDistXY(P1,P2);

  
  /* Find the most distance point to the line P1P2, excluding the
     first and last points */
    
  imax = NIL;
  Pm.x = Pm.y = Pm.z = NIL;
  P.x = P1.x;   P.y = P1.y; P.z = P1.z; 
  for (i=i0; i < in; i++) {
    P0.x = P.x + A->dx[ccode[i]];
    P0.y = P.y + A->dy[ccode[i]];
    P0.z = P.z;
    dist = iftPointLineDist2D(P0, P1, P2, P1P2);
    if (dist > dmax) {
      dmax = dist;
      imax = i+1;
      Pm.x = P0.x;  Pm.y = P0.y; Pm.z = P0.z; 
    }    
    P.x = P0.x; P.y = P0.y; P.z = P0.z; 
  }

  if ((dmax < smooth_factor)&&(dmax >= 0)){ // end of recursion
      iftDrawLine(img,P1,P2,label);
  }else{
    index  = iftGetVoxelIndex(img,Pm);
    iftSmoothContour(ccode, i0, imax, first, index, img, A, label, smooth_factor);
    iftSmoothContour(ccode, imax, in, index, last, img, A, label, smooth_factor);
  }

}


/*-------------------------- Public Methods --------------------------------*/

/* Extract contours in a slice by slice fashion */

iftContours *iftExtractContours(iftImage *label)
{
  iftImage  *pred=NULL,*bndr=NULL;
  int        p,q,r,i,left,right;
  int        dx,dy,j,k;
  iftLIFO   *LIFO=NULL;
  iftAdjRel *A=NULL,*L=NULL,*R=NULL;
  iftVoxel   u,v,w,t;
  iftContours *cnts=NULL;
  iftSet    *S=NULL, *Saux=NULL;

  A      = iftCircular(1.0);
  bndr   = iftObjectBorders(label,A);
  iftDestroyAdjRel(&A);

  A       = iftClockCircular(sqrtf(2.0));
  L       = iftLeftSide(A,1.0);
  R       = iftRightSide(A,1.0);
  pred    = iftCreateImage(label->xsize,label->ysize,label->zsize);
  LIFO    = iftCreateLIFO(label->n);
  cnts    = (iftContours *)iftAlloc(1,sizeof(iftContours));
  cnts->ncontours = 0;
  cnts->label     = label;


  /* Compute predecessor map of each contour, its last point, and
     count the number of contours */

  iftSetImage(pred,NIL);
  for (w.z=0; w.z < label->zsize; w.z++) {
    for (w.y=0; w.y < label->ysize; w.y++) {
      for (w.x=0; w.x < label->xsize; w.x++) {

	r = iftGetVoxelIndex(label,w);
	u.z = v.z = t.z = w.z;  
	
	if ((bndr->val[r]!=0)&&(pred->val[r]==NIL)&&iftValidContPoint(label,A,L,R,r)){
	  iftInsertLIFO(LIFO,r);
	  pred->val[r] = r;

	  while(!iftEmptyLIFO(LIFO)){
	    
	    p   = iftRemoveLIFO(LIFO);
	    u.x = iftGetXCoord(label,p);
	    u.y = iftGetYCoord(label,p);
	    
	    for (i=1; i < A->n; i++){
	      
	      v.x = u.x + A->dx[i];
	      v.y = u.y + A->dy[i];
	      
	      if (iftValidVoxel(label,v)){
		q = iftGetVoxelIndex(label,v);
		
		if ((pred->val[q]==r)&&(pred->val[p]!=r)&&(pred->val[p]!=q)){	
		  pred->val[q]=p;
		  cnts->ncontours++;
		  iftInsertSet(&S,q);
		  iftResetLIFO(LIFO);
		  break;
		}

		t.x = u.x + L->dx[i];
		t.y = u.y + L->dy[i];
		
		if (iftValidVoxel(label,t))
		  left = iftGetVoxelIndex(label,t);
		else
		  left = -1;
		
		t.x = u.x + R->dx[i];
		t.y = u.y + R->dy[i];
		
		if (iftValidVoxel(label,t))
		  right = iftGetVoxelIndex(label,t);
		else
		  right = -1;
		
		if ((bndr->val[q]!=0)&&(pred->val[q]==NIL)&&
		    (((left!=-1)&&(right!=-1)&&(label->val[left]!=label->val[right]))||
		     ((left==-1)&&(right!=-1)&&(label->val[right]!=0)) ||
		     ((right==-1)&&(left!=-1)&&(label->val[left]!=0)))){
		  pred->val[q] = p;
		  iftInsertLIFO(LIFO,q);	    
		}
	      }
	    }
	  }	  
	}
      }
    }
  }
   
  iftDestroyAdjRel(&A);
  iftDestroyAdjRel(&L);
  iftDestroyAdjRel(&R);
  iftDestroyLIFO(&LIFO);
  iftDestroyImage(&bndr);

  A = iftCircular(sqrtf(2.0));
  cnts->cnt = (iftContour *) iftAlloc(cnts->ncontours,sizeof(iftContour));
  if (cnts->cnt == NULL)
    iftError(MSG1,"iftExtractContours");

  /* count number of points in each contour and store first and last
     points, and their z-coordinate */
  
  for (i=0, Saux = S; i < cnts->ncontours; i++, Saux = Saux->next) {
    cnts->cnt[i].npts = 0;
    p    = Saux->elem;    
    cnts->cnt[i].first = p;
    while(pred->val[p]!=p){
      cnts->cnt[i].npts++;
      p = pred->val[p];      
    }
    cnts->cnt[i].last = p;
    cnts->cnt[i].z     = iftGetZCoord(label,p);
    cnts->cnt[i].npts++;
  }
  iftDestroySet(&S);

  /* Store contour points by chain coding */

  for (i=0; i < cnts->ncontours; i++) {
    cnts->cnt[i].ccode = iftAllocIntArray(cnts->cnt[i].npts-1);
    p   = cnts->cnt[i].first;    
    j   = 0;
    while(pred->val[p]!=p){ /* the code starts at the last and ends at
			       the first point */
      u.x = iftGetXCoord(label,p);
      u.y = iftGetYCoord(label,p);
      v.x = iftGetXCoord(label,pred->val[p]);
      v.y = iftGetYCoord(label,pred->val[p]);
      for (k=1; k < A->n; k++) {
	dx = v.x - u.x;
	dy = v.y - u.y;
	if ((dx==A->dx[k])&&(dy==A->dy[k])){
	  cnts->cnt[i].ccode[j]=k;
	  j++;
	  break;
	}
      }
      p = pred->val[p];      
    }
    if (j != (cnts->cnt[i].npts-1))
      iftError("Chain code extraction failed","iftExtractContours");
  }

  iftDestroyAdjRel(&A);
  iftDestroyImage(&pred);

  return(cnts);
}

void  iftDestroyContours(iftContours **cnts)
{
  iftContours *aux=*cnts;
  int i;

  if (aux != NULL) {
    for (i=0; i < aux->ncontours; i++) 
      iftFree(aux->cnt[i].ccode);
    iftFree(aux->cnt);
    iftFree(aux);
    *cnts = NULL;
  }
}

iftImage *iftSmoothContours(iftContours *cnts, float smooth_factor)
{
  iftImage *label=cnts->label;
  iftImage *contours=iftCreateImage(label->xsize,label->ysize,label->zsize);
  iftAdjRel *A=iftCircular(sqrtf(2.0));
  int i,j;
  iftVoxel P;

  if (smooth_factor > 0){
   for (i=0; i < cnts->ncontours; i++) 
     iftSmoothContour(cnts->cnt[i].ccode, 0, cnts->cnt[i].npts-2, cnts->cnt[i].first, cnts->cnt[i].last, contours, A, label->val[cnts->cnt[i].first], smooth_factor); 
  }else{
    for (i=0; i < cnts->ncontours; i++) {
      contours->val[cnts->cnt[i].first] = label->val[cnts->cnt[i].first];      
      P.x = iftGetXCoord(label,cnts->cnt[i].first);
      P.y = iftGetYCoord(label,cnts->cnt[i].first);
      P.z = cnts->cnt[i].z;
      for (j=0; j < cnts->cnt[i].npts-1; j++) {
	P.x = P.x + A->dx[cnts->cnt[i].ccode[j]];
	P.y = P.y + A->dy[cnts->cnt[i].ccode[j]];
	contours->val[iftGetVoxelIndex(label,P)]=label->val[cnts->cnt[i].first];
      }
    }
  }
  iftDestroyAdjRel(&A);
  return(contours);
}

iftErrorClassification iftClassifyBinarySegmentationErrors(iftImage* gt_image, iftImage* cl_image){
  iftErrorClassification errors;
  errors.tp = 0; errors.fp = 0; errors.tn = 0; errors.fn = 0;

  int i;
  for(i = 0; i < cl_image->n; i++){
    //Object Pixel
    if(gt_image->val[i] != 0){
      if(cl_image->val[i] == 2)
        errors.tp++;
      else
        errors.fn++;
    }
    //Background Pixel
    else{
      if(cl_image->val[i] == 1)
        errors.tn++;
      else
        errors.fp++;
    }
  }

  return errors;
}




iftMatrix *iftRotationMatrixByPCA(iftDataSet *Z)
{
  iftMatrix  *A,*U,*S,*Vt;

  if (!iftIsCentralizedDataSet(Z))
    iftError("Dataset must be centralized","iftRotationMatrixByPCA");

  /* Compute Principal Components */

  puts("[1]");
  A  = iftCovarianceMatrix(Z);
  puts("[2]");
  iftSingleValueDecomp(A,&U,&S,&Vt);

  puts("[3]");
  iftDestroyMatrix(&A);
  iftDestroyMatrix(&S);
  /*
  if (Z->nsamples < Z->nfeats) {
    iftDestroyMatrix(&U);
    return(Vt);
  }else{
  */
    iftDestroyMatrix(&Vt);
    return(U);
    //  }

}


void iftSingleValueDecomp(iftMatrix *A, iftMatrix **U, iftMatrix **S, iftMatrix **Vt)
{
  int m = A->nrows, n = A->ncols, lda = m, ldu = m;
  int k,D=MIN(m,n);
  int ldvt = n, info, lwork, *iwork;
  float *work, wkopt;
  float *s, *u, *vt;
  float *a;

  /* Alloc work memory space */

  iwork = iftAllocIntArray(8*D);
  s     = iftAllocFloatArray(n);
  u     = iftAllocFloatArray(ldu*m);
  vt    = iftAllocFloatArray(ldvt*n);
  a     = iftAllocFloatArray(lda*n);

  /* Load input matrix A (Note that Fortran saves array information
     column by column) */

  for (k=0; k < A->n; k++) {
    a[k]=(float)A->val[k];
  }

  /* Query and allocate the optimal workspace */
  lwork = NIL;
  sgesdd_( "Singular vectors", &m, &n, a, &lda, s, u, &ldu, vt, &ldvt, &wkopt,
	   &lwork, iwork, &info );
  lwork = (int)wkopt;
   work = iftAllocFloatArray(lwork);


  /* Compute SVD */
  sgesdd_( "Singular vectors", &m, &n, a, &lda, s, u, &ldu, vt, &ldvt, work,
	   &lwork, iwork, &info );

  iftFree(work);
  iftFree(iwork);
  iftFree(a);

  /* Check for convergence */
  if( info > 0 ) {
    iftError("The algorithm computing SVD failed to converge","iftSVD");
  }

  /* Get matrices after decomposition */


  *U  = iftCreateMatrix(D,m);


  *S  = iftCreateMatrix(D,1);


  *Vt = iftCreateMatrix(n,D);


  for (k=0; k < (*U)->n; k++)
    (*U)->val[k] = u[k];


  for (k=0; k < (*S)->n; k++)
    (*S)->val[k] = s[k];



  for (k=0; k < (*Vt)->n; k++)
    (*Vt)->val[k] = vt[k];


  iftFree(s);
  iftFree(u);
  iftFree(vt);
}


    iftDataSet *iftTransFeatSpaceByPCA(iftDataSet *Z, int num_of_comps)
    {
        int         s,i,j;
        iftDataSet *Zt;
        iftMatrix  *U;



        if ((!iftIsTrainingDataSet(Z))||(!iftIsCentralizedDataSet(Z)))
            iftError("It requires a centralized training set","iftTransFeatSpaceByPCA");


        if ((num_of_comps > Z->nfeats)||(num_of_comps > Z->nsamples))
            iftError("There is no need to reduce feature space","iftTransFeatSpaceByPCA");

        /* Compute rotation matrix according to PCA */

        U  = iftRotationMatrixByPCA(Z);

        /* Rotate dataset and select the principal componentes */

        Zt = iftCreateDataSet(Z->nsamples,num_of_comps);
        Zt->iftArcWeight = Z->iftArcWeight;
        Zt->function_number = Z->function_number;
        Zt->nclasses     = Z->nclasses;
        Zt->ref_data     = Z->ref_data;
        for (s=0; s < Zt->nsamples; s++) {
            Zt->sample[s].truelabel  = Z->sample[s].truelabel;
            Zt->sample[s].status = Z->sample[s].status;
            Zt->sample[s].id     = Z->sample[s].id;

            /* Rotate and Project Feature Space */

            for (j=0; j < num_of_comps; j++) {
                Zt->sample[s].feat[j]=0;
                for (i=0; i < U->ncols; i++)
                    Zt->sample[s].feat[j] += Z->sample[s].feat[i]*U->val[iftGetMatrixIndex(U,i,j)];
            }

        }

        Zt->ntrainsamples  = Z->ntrainsamples;

        /* Copy feature space parameters */

        Zt->fsp    = iftCopyFeatSpaceParam(Z);
        Zt->fsp.R  = iftCopyMatrix(U);
        Zt->fsp.ncomps = num_of_comps;
        for (i=0; i < Zt->nfeats; i++){
            Zt->alpha[i]=1.0;
        }

        iftDestroyMatrix(&U);

        return(Zt);
    }



iftImage *iftFastDilate(iftImage *img, iftAdjRel *A)
{
  iftImage *dil = iftCopyImage(img);
  iftFastAdjRel *F = iftCreateFastAdjRel(A,img->tby,img->tbz);

#pragma omp parallel for shared(img,dil,F)
  for (int i=1; i < F->n; i++) {    
    iftVoxel u;
    for (u.z=F->bz; u.z < img->zsize-F->bz; u.z++) 
      for (u.y=F->by; u.y < img->ysize-F->by; u.y++) 
	for (u.x=F->bx; u.x < img->xsize-F->bx; u.x++) {
	  int p = u.x + img->tby[u.y] + img->tbz[u.z];
	  int q = p + F->dq[i];
	  if (img->val[q]>dil->val[p]){
#pragma omp critical 
	    {
	    dil->val[p] = img->val[q];
	    }
	  }
	}
  }

  iftDestroyFastAdjRel(&F);

  return(dil);
}


iftImage *iftFastErode(iftImage *img, iftAdjRel *A)
{
  iftImage *ero=iftCopyImage(img);
  iftFastAdjRel *F = iftCreateFastAdjRel(A,img->tby,img->tbz);

#pragma omp parallel for shared(img,ero,F)
  for (int i=1; i < F->n; i++) {    
    iftVoxel u;
    for (u.z=F->bz; u.z < img->zsize-F->bz; u.z++) 
      for (u.y=F->by; u.y < img->ysize-F->by; u.y++) 
	for (u.x=F->bx; u.x < img->xsize-F->bx; u.x++) {
	  int p = u.x + img->tby[u.y] + img->tbz[u.z];
	  int q = p + F->dq[i];
	  if (img->val[q]<ero->val[p]){
            #pragma omp critical
	    {
	    ero->val[p] = img->val[q];
	    }
	  }
	}
  }

  iftDestroyFastAdjRel(&F);

  return(ero);
}

iftImage *iftWatershedLIFO(iftImage *basins, iftAdjRel *A, iftLabeledSet *seed)
{
  iftImage  *pathval = NULL, *label = NULL;
  iftGQueue  *Q = NULL;
  int      i, p, q, tmp;
  iftVoxel    u, v;
  iftLabeledSet *S = seed;

  // Initialization
  pathval  = iftCreateImage(basins->xsize, basins->ysize, basins->zsize);
  label    = iftCreateImage(basins->xsize, basins->ysize, basins->zsize);
  Q        = iftCreateGQueue(iftMaximumValue(basins) + 1, basins->n, pathval->val);
  iftSetTieBreak(Q, LIFOBREAK);

  while (S != NULL)
  {
    p = S->elem;
    label->val[p]   = S->label;
    pathval->val[p] = 0;
    iftInsertGQueue(&Q, p);
    S = S->next;
  }

  for (p = 0; p < basins->n; p++)
  {
    if (Q->L.elem[p].color == WHITE)
    {
      pathval->val[p] = INFINITY_INT;
      label->val[p]   = 0;
      iftInsertGQueue(&Q, p);
    }
  }


  // Image Foresting Transform
  while (!iftEmptyGQueue(Q))
  {
    p = iftRemoveGQueue(Q);
    u = iftGetVoxelCoord(basins, p);

    for (i = 1; i < A->n; i++)
    {
      v = iftGetAdjacentVoxel(A, u, i);

      if (iftValidVoxel(basins, v))
      {
        q = iftGetVoxelIndex(basins, v);
        if (Q->L.elem[q].color == GRAY)
        {
          tmp = MAX(pathval->val[p], basins->val[q]);
          if (tmp <= pathval->val[q])
          {
            if (Q->L.elem[q].color == GRAY)
              iftRemoveGQueueElem(Q, q);
            label->val[q]   = label->val[p];
            pathval->val[q] = tmp;
            iftInsertGQueue(&Q, q);
          }
        }
      }
    }
  }

  iftDestroyGQueue(&Q);
  iftDestroyImage(&pathval);

  iftCopyVoxelSize(basins, label);

  return (label);
}
void iftDiffWatershedLIFO(iftImageForest *fst, iftLabeledSet *seed)
{
  iftAdjRel *A = fst->A;
  iftGQueue *Q = fst->Q;
  iftVoxel   u, v;
  int        i, p, q, tmp;
  char       trees_for_removal;
  iftSet    *Frontier = NULL, *Processed=NULL;
  iftLabeledSet *S;
  iftBMap   *processed = fst->processed;
  iftImage  *pathval = fst->pathval, *pred = fst->pred, *label = fst->label;
  iftImage  *root = fst->root, *basins = fst->img, *marker = fst->marker;

  iftSetTieBreak(Q, LIFOBREAK);
  iftFillBMap(processed, 0);

  // Verify if there are trees for removal
  trees_for_removal = 0;
  S = seed;
  while (S != NULL)
  {
    if (S->label == NIL) // Removal marker
    {
      trees_for_removal = 1;
      break;
    }
    S = S->next;
  }

  // Remove marked trees
  if (trees_for_removal)
  {
    Frontier = iftTreeRemoval(fst, seed);
    while (Frontier != NULL)
    {
      p = iftRemoveSet(&Frontier);
      iftInsertGQueue(&Q, p);
    }
  }

  // Trivial path initialization for new seeds
  S = seed;
  while (S != NULL)
  {
    p = S->elem;
    if (S->label != NIL)
    {
      if (pathval->val[p] > S->handicap)
      {
        if (Q->L.elem[p].color == GRAY)
        {
          /* p is also a frontier voxel, but the priority is it as a seed. */
          iftRemoveGQueueElem(Q, p);
        }

        label->val[p]   = S->label;
        pathval->val[p] = S->handicap;
        root->val[p]    = p;
        pred->val[p]    = NIL;
        marker->val[p]  = S->marker;
        iftInsertGQueue(&Q, p);
      }
    }
    S = S->next;
  }
  //Insert all the other pixels in the queue, with the same cost
  for (p = 0; p < basins->n; p++)
  {
    if (Q->L.elem[p].color == WHITE)
    {
      iftInsertGQueue(&Q, p);
    }
  }


  /* Image Foresting Transform */
  while (!iftEmptyGQueue(Q))
  {
    p = iftRemoveGQueue(Q);
    iftInsertSet(&Processed,p);
    u = iftGetVoxelCoord(basins, p);
    iftBMapSet1(processed, p);

    for (i = 1; i < A->n; i++)
    {
      v = iftGetAdjacentVoxel(A, u, i);
      if (iftValidVoxel(basins, v))
      {
        q = iftGetVoxelIndex(basins, v);

        if (Q->L.elem[q].color == GRAY)
        {
          tmp = MAX(pathval->val[p], basins->val[q]);

          /* if pred[q]=p then p and q belong to a tie-zone */
          if (tmp <= pathval->val[q])
          {
            if (Q->L.elem[q].color == GRAY)
              iftRemoveGQueueElem(Q, q);
            pred->val[q]     = p;
            root->val[q]     = root->val[p];
            label->val[q]    = label->val[p];
            marker->val[q]   = marker->val[p];
            pathval->val[q]  = tmp;
            iftInsertGQueue(&Q, q);
          }
        }
      }
    }
  }

  while(Processed != NULL){
    p = iftRemoveSet(&Processed);
    Q->L.elem[p].color = WHITE;
  }

}

void iftDiffWatershedTree(iftImageForest *fst, iftLabeledSet *seed)
{
  iftAdjRel *A=fst->A;
  iftGQueue *Q=fst->Q;
  iftVoxel   u,v;
  int        i,p,q,tmp;
  char       trees_for_removal;
  iftSet    *Frontier=NULL, *Processed=NULL;
  iftLabeledSet *S;
  iftImage  *pathval=fst->pathval,*pred=fst->pred,*label=fst->label;
  iftImage  *root=fst->root, *basins=fst->img;

  // Verify if there are trees for removal
  trees_for_removal=0;
  S = seed;
  while (S != NULL) { 
    if (S->label == NIL){// Removal marker
      trees_for_removal=1;
      break;
    }
    S = S->next;
  }

  // Remove marked trees  
  if (trees_for_removal) {
    Frontier = iftTreeRemoval(fst,seed);
    while (Frontier != NULL) {
      p = iftRemoveSet(&Frontier); 
      iftInsertGQueue(&Q,p);
    }
  }
 
  // Trivial path initialization for new seeds
  S = seed;
  while(S != NULL){
    p=S->elem;
    if (S->label != NIL){
      if (Q->L.elem[p].color == GRAY) { /* p is also a frontier voxel,
           but the priority is it as a seed. */
  iftRemoveGQueueElem(Q,p);
      }
      label->val[p]=S->label; 
      pathval->val[p]=0; 
      root->val[p]=p; 
      pred->val[p]=NIL; 
      iftInsertGQueue(&Q,p);
    }
    S = S->next;
  }

  /* Image Foresting Transform */
  while(!iftEmptyGQueue(Q)) {
    p=iftRemoveGQueue(Q);
    iftInsertSet(&Processed,p);
    u.x = iftGetXCoord(basins,p);
    u.y = iftGetYCoord(basins,p);
    u.z = iftGetZCoord(basins,p);

    for (i=1; i < A->n; i++){
      v.x = u.x + A->dx[i];
      v.y = u.y + A->dy[i];
      v.z = u.z + A->dz[i];
      if (iftValidVoxel(basins,v)){ 
  q = iftGetVoxelIndex(basins,v);
  if (Q->L.elem[q].color != BLACK){
    tmp = MAX(pathval->val[p],basins->val[q]);
    if ((tmp < pathval->val[q]) || ((pred->val[q]==p)))
    {
      if (Q->L.elem[q].color == GRAY) { 
        iftRemoveGQueueElem(Q,q);
      }
      pred->val[q]  = p;
      root->val[q]  = root->val[p];
      label->val[q] = label->val[p];
      pathval->val[q]  = tmp;
      iftInsertGQueue(&Q, q);
    }
  }
      }
    }
  }

  while(Processed != NULL){
    p = iftRemoveSet(&Processed);
    Q->L.elem[p].color = WHITE;
  }

}

void iftDiffWatershedComp(iftImageForest *fst, iftLabeledSet *seed)
{
  iftAdjRel *A=fst->A;
  iftGQueue *Q=fst->Q;
  iftVoxel   u,v;
  int        i,p,q,tmp;
  char       trees_for_removal;
  iftSet    *Frontier=NULL, *Processed=NULL;
  iftLabeledSet *S;
  iftImage  *pathval=fst->pathval,*pred=fst->pred,*label=fst->label;
  iftImage  *root=fst->root, *basins=fst->img;

  // Verify if there are trees for removal
  trees_for_removal=0;
  S = seed;
  while (S != NULL) { 
    if (S->label == NIL){ // Removal marker
      trees_for_removal=1;
      break;
    }
    S = S->next;
  }

  // Remove marked trees  
  if (trees_for_removal) {
    Frontier = iftCompRemoval(fst,seed);
    while (Frontier != NULL) {
      p = iftRemoveSet(&Frontier); 
      iftInsertGQueue(&Q,p);
    }
  }
 
  // Trivial path initialization for new seeds 
  S = seed;
  while(S != NULL){
    p=S->elem;
    if (S->label != NIL){
      if (Q->L.elem[p].color == GRAY) { /* p is also a frontier voxel,
           but the priority is it as a seed. */
  iftRemoveGQueueElem(Q,p);
      }
      label->val[p]=S->label; 
      pathval->val[p]=0; 
      root->val[p]=p; 
      pred->val[p]=NIL; 
      iftInsertGQueue(&Q,p);      
    }
    S = S->next;
  }

  /* Image Foresting Transform */
  while(!iftEmptyGQueue(Q)) {
    p=iftRemoveGQueue(Q);
    iftInsertSet(&Processed,p);

    u.x = iftGetXCoord(basins,p);
    u.y = iftGetYCoord(basins,p);
    u.z = iftGetZCoord(basins,p);

    for (i=1; i < A->n; i++){
      v.x = u.x + A->dx[i];
      v.y = u.y + A->dy[i];
      v.z = u.z + A->dz[i];
      if (iftValidVoxel(basins,v)){ 
  q = iftGetVoxelIndex(basins,v);
  if (Q->L.elem[q].color != BLACK){
    tmp = MAX(pathval->val[p],basins->val[q]);
    if ((tmp < pathval->val[q]) || ((pred->val[q]==p)))
    {
      if (Q->L.elem[q].color == GRAY) { 
        iftRemoveGQueueElem(Q,q);
      }
      pred->val[q]  = p;
      root->val[q]  = root->val[p];
      label->val[q] = label->val[p];
      pathval->val[q]  = tmp;
      iftInsertGQueue(&Q, q);
    }
  }
      }
    }
  }

  while(Processed != NULL){
    p = iftRemoveSet(&Processed);
    Q->L.elem[p].color = WHITE;
  }

}

void iftIncWatershed(iftImageForest *fst, iftLabeledSet *seed)
{
  iftAdjRel *A=fst->A;
  iftGQueue *Q=fst->Q;
  iftVoxel   u,v;
  int        i,p,q,tmp;
  char       trees_for_removal;
  iftLabeledSet *S;
  iftImage  *pathval=fst->pathval,*pred=fst->pred,*label=fst->label;
  iftImage  *root=fst->root, *basins=fst->img;

  // Verify if there are trees for removal

  trees_for_removal=0;
  S = seed;

  while (S != NULL) { 
    if ((S->label != label->val[S->elem])&&
  (S->label >= 0) && 
  (label->val[S->elem] >= 0) &&
        (pathval->val[S->elem] < INFINITY_INT)){ // new label in a
                                                 // region already 
                                                 // conquered
      trees_for_removal=1;
      break;
    }
    S = S->next;
  }

  // Remove marked trees  
  if (trees_for_removal) {
    iftCompRemovalWithoutFrontier(fst,seed);
  }
 
  // Trivial path initialization for new seeds 

  S = seed;
  while(S != NULL){
    p=S->elem;
    if (S->label != NIL){
      if (Q->L.elem[p].color == GRAY) { /* p is also a frontier voxel,
           but the priority is it as a seed. */
  iftRemoveGQueueElem(Q,p);
      }
      label->val[p]=S->label; 
      pathval->val[p]=0; 
      root->val[p]=p; 
      pred->val[p]=NIL; 
      iftInsertGQueue(&Q,p);
    }
    S = S->next;
  }

  /* Image Foresting Transform */

  while(!iftEmptyGQueue(Q)) {
    p=iftRemoveGQueue(Q);
    u.x = iftGetXCoord(basins,p);
    u.y = iftGetYCoord(basins,p);
    u.z = iftGetZCoord(basins,p);

    for (i=1; i < A->n; i++){
      v.x = u.x + A->dx[i];
      v.y = u.y + A->dy[i];
      v.z = u.z + A->dz[i];
      if (iftValidVoxel(basins,v)){ 
  q = iftGetVoxelIndex(basins,v);
  if (Q->L.elem[q].color != BLACK){
    tmp = MAX(pathval->val[p],basins->val[q]);
    if ((tmp < pathval->val[q])||(pred->val[q]==p)){
      if (Q->L.elem[q].color == GRAY) { 
        iftRemoveGQueueElem(Q,q);
      }
      pred->val[q]  = p;
      root->val[q]  = root->val[p];
      label->val[q] = label->val[p];
      pathval->val[q]  = tmp;
      iftInsertGQueue(&Q, q);
    }
  }
      }
    }
  }

}

/* Compute kappa-connected segmentation for multiple objects with seed
   competition. The image of markers indicate labels 0 for background,
   1,2,..., for objects and NIL for available voxels. */

iftImage *iftKappaConnectedSegm(iftImage *basins, iftAdjRel *A, iftImage *markers, int area_thres)
{
  iftImage  *pathval=NULL,*label=iftCopyImage(markers),*root=NULL;
  iftImage  *wavefront_size=NULL,*kappa=NULL,*wavefront_pathval=NULL;
  iftGQueue *Q=NULL;
  int        i,r,p,q,tmp;
  iftVoxel   u,v;
  
  // Initialization 

  pathval  = iftCreateImage(basins->xsize,basins->ysize,basins->zsize);
  root     = iftCreateImage(basins->xsize,basins->ysize,basins->zsize);
  wavefront_size = iftCreateImage(basins->xsize,basins->ysize,basins->zsize);
  wavefront_pathval = iftCreateImage(basins->xsize,basins->ysize,basins->zsize);
  kappa    = iftCreateImage(basins->xsize,basins->ysize,basins->zsize);
  Q        = iftCreateGQueue(iftMaximumValue(basins)+1,basins->n,pathval->val);

  for (p=0; p < basins->n; p++) {
    root->val[p]=p;
    wavefront_size->val[p]=0;
    wavefront_pathval->val[p]=0;
    kappa->val[p]    = INFINITY_INT;
    pathval->val[p]  = INFINITY_INT;
    if (label->val[p] >= 0){
      pathval->val[p]=0;
      iftInsertGQueue(&Q,p);
    }else{
      label->val[p]=0;
    }
  }


  // Image Foresting Transform

  while(!iftEmptyGQueue(Q)) {
    p=iftRemoveGQueue(Q);
    r=root->val[p];
    u = iftGetVoxelCoord(basins,p);

    if ((kappa->val[r]==INFINITY_INT)&&
      (label->val[r] != 0)){
      if (pathval->val[p] != wavefront_pathval->val[r]){
      wavefront_size->val[r]=1;
      wavefront_pathval->val[r] = pathval->val[p];
      }else{
      wavefront_size->val[r]+=1;
      }
      if (wavefront_size->val[r] > area_thres)
      kappa->val[r] = MAX(wavefront_pathval->val[r]-1,0);
    }

    if (pathval->val[p] <= kappa->val[r]){
  
      for (i=1; i < A->n; i++){
  v = iftGetAdjacentVoxel(A,u,i);

  if (iftValidVoxel(basins,v)){ 
    q = iftGetVoxelIndex(basins,v);
    if (pathval->val[q] > pathval->val[p]){
      tmp = MAX(pathval->val[p],basins->val[q]);
      if (tmp < pathval->val[q]){ 
        if (pathval->val[q]!=INFINITY_INT)
    iftRemoveGQueueElem(Q,q);
        root->val[q]     = r;
        pathval->val[q]  = tmp;
        iftInsertGQueue(&Q, q);
      }
    }
  }
      }
    }
  }



  for (p=0; p < root->n; p++){
    r = root->val[p];
    if (pathval->val[p] <= kappa->val[r]) 
      label->val[p] = label->val[r];
  }

  iftDestroyGQueue(&Q);
  iftDestroyImage(&pathval);
  iftDestroyImage(&root);
  iftDestroyImage(&wavefront_size);
  iftDestroyImage(&wavefront_pathval);
  iftDestroyImage(&kappa);

  iftCopyVoxelSize(basins,label);

  return(label);
}










/*------ Scene to analyze file conversion ------*/
typedef struct {
    int hdrlen;
    int bpp;
    int dimensions;
    int W, H, D, T;
    float dx, dy, dz;
    int be_hint;
    int dt;
} AnalyzeHdr;

typedef struct {
    int hdrlen;
    char data_type[10];
    char db_name[18];
    int extents;
    short int error;
    char regular;
    char hkey0;
} Ana_Hdr1;

typedef struct {
    short int dim[8];
    short int unused[7];
    short int data_type;
    short int bpp;
    short int dim_un0;
    float pixdim[8];
    float zeroes[8];
    int maxval;
    int minval;
} Ana_Hdr2;


static int ana_fio_big_endian = -1;

void ana_fio_init() {
    short int x;
    char *p;

    x = 0x0001;
    p = (char *) (&x);

    if ((*p) == 0)
        ana_fio_big_endian = 1;
    else
        ana_fio_big_endian = 0;
}


int ana_fio_swab_16(int val) {
    int x;
    val &= 0xffff;
    x = (val & 0xff) << 8;
    x |= (val >> 8);
    return x;
}


int ana_fio_swab_32(int val) {
    int x;
    val &= 0xffffffff;
    x = (val & 0xff) << 24;
    x |= ((val >> 8) & 0xff) << 16;
    x |= ((val >> 16) & 0xff) << 8;
    x |= ((val >> 24) & 0xff);
    return x;
}


void *SwapEndian(void *Addr, const int Nb) {
    static char Swapped[16];
    switch (Nb) {
        case 2:
            Swapped[0] = *((char *) Addr + 1);
            Swapped[1] = *((char *) Addr);
            break;
        case 3:    // As far as I know, 3 is used only with RGB images
            Swapped[0] = *((char *) Addr + 2);
            Swapped[1] = *((char *) Addr + 1);
            Swapped[2] = *((char *) Addr);
            break;
        case 4:
            Swapped[0] = *((char *) Addr + 3);
            Swapped[1] = *((char *) Addr + 2);
            Swapped[2] = *((char *) Addr + 1);
            Swapped[3] = *((char *) Addr);
            break;
        case 8:
            Swapped[0] = *((char *) Addr + 7);
            Swapped[1] = *((char *) Addr + 6);
            Swapped[2] = *((char *) Addr + 5);
            Swapped[3] = *((char *) Addr + 4);
            Swapped[4] = *((char *) Addr + 3);
            Swapped[5] = *((char *) Addr + 2);
            Swapped[6] = *((char *) Addr + 1);
            Swapped[7] = *((char *) Addr);
            break;
        case 16:
            Swapped[0] = *((char *) Addr + 15);
            Swapped[1] = *((char *) Addr + 14);
            Swapped[2] = *((char *) Addr + 13);
            Swapped[3] = *((char *) Addr + 12);
            Swapped[4] = *((char *) Addr + 11);
            Swapped[5] = *((char *) Addr + 10);
            Swapped[6] = *((char *) Addr + 9);
            Swapped[7] = *((char *) Addr + 8);
            Swapped[8] = *((char *) Addr + 7);
            Swapped[9] = *((char *) Addr + 6);
            Swapped[10] = *((char *) Addr + 5);
            Swapped[11] = *((char *) Addr + 4);
            Swapped[12] = *((char *) Addr + 3);
            Swapped[13] = *((char *) Addr + 2);
            Swapped[14] = *((char *) Addr + 1);
            Swapped[15] = *((char *) Addr);
            break;
    }
    return (void *) Swapped;
}


int ana_fio_read_8(FILE *f) {
    char val;

    if (ana_fio_big_endian < 0) ana_fio_init();
    if (fread(&val, 1, 1, f) != 1) return -1;

    return ((int) val);
}


int ana_fio_read_16(FILE *f, int is_big_endian) {
    short int val;

    if (ana_fio_big_endian < 0) ana_fio_init();
    if (fread(&val, 1, 2, f) != 2) return -1;

    if ((is_big_endian != 0) != (ana_fio_big_endian != 0))
        val = (short int) ana_fio_swab_16(val);
    return ((int) val);
}


int ana_fio_read_32(FILE *f, int is_big_endian) {
    int val;

    if (ana_fio_big_endian < 0) ana_fio_init();
    if (fread(&val, 1, 4, f) != 4) return -1;

    if ((is_big_endian != 0) != (ana_fio_big_endian != 0))
        val = (short int) ana_fio_swab_32(val);
    return val;
}


float ana_fio_read_float32(FILE *f, int is_big_endian) {
    int val;
    void *vp;
    float *p, q;
    val = ana_fio_read_32(f, is_big_endian);
    vp = (void *) (&val);
    p = (float *) (vp);
    q = *p;
    return q;
}


int ana_fio_abs_read_8(FILE *f, long offset) {
    if (fseek(f, offset, SEEK_SET) != 0) return -1;
    return (ana_fio_read_8(f));
}


int ana_fio_abs_read_16(FILE *f, long offset, int is_big_endian) {
    if (fseek(f, offset, SEEK_SET) != 0) return -1;
    return (ana_fio_read_16(f, is_big_endian));
}


int ana_fio_abs_read_32(FILE *f, long offset, int is_big_endian) {
    if (fseek(f, offset, SEEK_SET) != 0) return -1;
    return (ana_fio_read_32(f, is_big_endian));
}


float ana_fio_abs_read_float32(FILE *f, long offset, int is_big_endian) {
    union {
        int i;
        float f;
    } val;
    val.i = ana_fio_abs_read_32(f, offset, is_big_endian);
    return val.f;
}


int ana_fio_write_8(FILE *f, int val) {
    char v;
    if (ana_fio_big_endian < 0) ana_fio_init();
    v = (char) (val & 0xff);
    if (fwrite(&v, 1, 1, f) != 1) return -1;
    return 0;
}


int ana_fio_write_16(FILE *f, int is_big_endian, int val) {
    short int v;
    if (ana_fio_big_endian < 0) ana_fio_init();
    v = (short int) (val & 0xffff);
    if ((is_big_endian != 0) != (ana_fio_big_endian != 0))
        v = (short int) ana_fio_swab_16(v);
    if (fwrite(&v, 1, 2, f) != 2) return -1;
    return 0;
}


int ana_fio_write_32(FILE *f, int is_big_endian, int val) {
    int v = val;
    if (ana_fio_big_endian < 0) ana_fio_init();
    if ((is_big_endian != 0) != (ana_fio_big_endian != 0))
        v = ana_fio_swab_32(v);
    if (fwrite(&v, 1, 4, f) != 4) return -1;
    return 0;
}


int ana_fio_write_float32(FILE *f, int is_big_endian, float val) {
    float v, *w;
    int i, *j;
    void *vp;
    if (ana_fio_big_endian < 0) ana_fio_init();
    v = val;
    if ((is_big_endian != 0) != (ana_fio_big_endian != 0)) {
        vp = (void *) (&v);
        j = (int *) (vp);
        i = *j;
        i = ana_fio_swab_32(i);
        w = (float *) j;
        v = *w;
    }
    if (fwrite(&v, 1, 4, f) != 4) return -1;
    return 0;
}


int ana_fio_abs_write_8(FILE *f, long offset, int val) {
    if (fseek(f, offset, SEEK_SET) != 0) return -1;
    return (ana_fio_write_8(f, val));
}


int ana_fio_abs_write_16(FILE *f, long offset, int is_big_endian, int val) {
    if (fseek(f, offset, SEEK_SET) != 0) return -1;
    return (ana_fio_write_16(f, is_big_endian, val));
}


int ana_fio_abs_write_32(FILE *f, long offset, int is_big_endian, int val) {
    if (fseek(f, offset, SEEK_SET) != 0) return -1;
    return (ana_fio_write_32(f, is_big_endian, val));
}


int ana_fio_abs_write_float32(FILE *f, long offset, int is_big_endian, float val) {
    if (fseek(f, offset, SEEK_SET) != 0) return -1;
    return (ana_fio_write_float32(f, is_big_endian, val));
}


int ana_fio_write_zeroes(FILE *f, int n) {
    while (n > 0) {
        if (n >= 4) {
            if (ana_fio_write_32(f, 0, 0) != 0) return -1;
            n -= 4;
        } else if (n >= 2) {
            if (ana_fio_write_16(f, 0, 0) != 0) return -1;
            n -= 2;
        } else {
            if (ana_fio_write_8(f, 0) != 0) return -1;
            n--;
        }
    }
    return 0;
}


int ana_fio_abs_write_zeroes(FILE *f, long offset, int n) {
    if (fseek(f, offset, SEEK_SET) != 0) return -1;
    return (ana_fio_write_zeroes(f, n));
}



float IntSwap (int f)                       //Convert int data to big-endian, the default
{                                           //binary format required by ParaView
    union
    {
        int f;
        unsigned char b[4];
    } dat1, dat2;

    dat1.f = f;
    dat2.b[0] = dat1.b[3];
    dat2.b[1] = dat1.b[2];
    dat2.b[2] = dat1.b[1];
    dat2.b[3] = dat1.b[0];
    return dat2.f;
}



iftImage *iftAna2Scn(const char *format, ...) {
    va_list args;
    char filename[BUFSIZ];

    va_start(args, format);
    vsprintf(filename, format, args);
    va_end(args);

    AnalyzeHdr hdr;
    int obpp = -1;
    int be = 0;

    char *base = iftBasename(filename);

    char *hdr_name = iftConcatStrings(2, base, ".hdr");

    // Reading .hdr file
    FILE *fp = fopen(hdr_name, "rb");
    if (fp == NULL) {
        iftExit("Cannot open file: \"%s\"", "iftReadImageAnalyze", hdr_name);
    }
    iftFree(hdr_name);

    hdr.hdrlen = ana_fio_abs_read_32(fp, 0, be);
    if (hdr.hdrlen != 348) {
        be = 1;
        hdr.hdrlen = ana_fio_abs_read_32(fp, 0, be);
    }
    if (hdr.hdrlen != 348)
        iftExit("This is not an Analyze header!", "iftReadImageAnalyze");
    hdr.dt = ana_fio_abs_read_16(fp, 40 + 30, be);
    hdr.bpp = ana_fio_abs_read_16(fp, 40 + 32, be);
    hdr.dimensions = ana_fio_abs_read_16(fp, 40, be);
    hdr.W = ana_fio_abs_read_16(fp, 40 + 2, be);
    hdr.H = ana_fio_abs_read_16(fp, 40 + 4, be);
    hdr.D = ana_fio_abs_read_16(fp, 40 + 6, be);
    hdr.T = ana_fio_abs_read_16(fp, 40 + 8, be);

    hdr.dx = ana_fio_abs_read_float32(fp, 40 + 36 + 4, be);
    hdr.dy = ana_fio_abs_read_float32(fp, 40 + 36 + 8, be);
    hdr.dz = ana_fio_abs_read_float32(fp, 40 + 36 + 12, be);

    fclose(fp);

    if (hdr.dimensions == 4 && hdr.T != 1)
        iftExit("This file has a 3D time series, I can't convert it.", "iftReadImageAnalyze");

    if (hdr.dimensions < 3 || hdr.dimensions > 4)
        iftExit("This file is not a 3D volume, I can't convert it.", "iftReadImageAnalyze");

    if (obpp < 0) obpp = hdr.bpp;
    if (hdr.bpp != 8 && hdr.bpp != 16 && hdr.bpp != 32)
        iftExit("This file is not 8-bit neither 16-bit nor 32-bit data. It is not supported.", "iftReadImageAnalyze");
    hdr.be_hint = be;

    // Reading the img file
    char *img_name = iftConcatStrings(2, base, ".img");
    fp = fopen(img_name, "rb");
    if (fp == NULL) {
        iftExit(MSG_FILE_OPEN_ERROR, "iftReadImageAnalyze", img_name);
    }
    iftFree(img_name);
    iftFree(base);

    iftImage *img = iftCreateImage(hdr.W, hdr.H, hdr.D);
    if (img == NULL) {
        iftExit("iftCreateScene() error", "iftReadImageAnalyze");
    }
    img->dx = hdr.dx;
    img->dy = hdr.dy;
    img->dz = hdr.dz;
    
for (v.z = 0; v.z < image->zsize; v.z++)
        for (v.y = image->ysize - 1; v.y >= 0; v.y--) // reverse order
            for (v.x = 0; v.x < image->xsize; v.x++) {
                p = v.x + image->tby[v.y] + image->tbz[v.z];
                if (hdr.bpp == 8) {
                    n = fread(&val8, sizeof(unsigned char), 1, f);
                    image->val[p] = (int) val8;
                }
                if (hdr.bpp == 16) {
                    n = fread(&val16, sizeof(short int), 1, f);
                    if (hdr.be_hint)
                        val16 = (short int) ana_fio_swab_16(val16);
                    image->val[p] = (int) val16;
                }
                if (hdr.bpp == 32) {
                    printf("hdr.bpp = %d\n", hdr.bpp);
                    printf("hdr.dt = %d\n", hdr.dt);
                    puts("************************");
                    if (hdr.dt == 8) { // integer
                        n = fread(&val32, sizeof(long int), 1, f);
                        if (hdr.be_hint)
                            val32 = (long int) ana_fio_swab_32(val32);
                        image->val[p] = (int) val32;
                    }
                    else {
                        puts("----------------------------");
                        // iftExit("Image with depth more than 32. Float scene not supported. The function does not know what to do",
                        //         "iftReadImageAnalyze");
                        data = (float *) iftAlloc(hdr.W * hdr.H * hdr.D, sizeof(float));
                        n = fread(&fval32, sizeof(float), 1, f);
                        if (hdr.be_hint)
                            fval32 = *((float *) SwapEndian((void *) (&fval32), sizeof(float)));
                        data[p] = fval32;
                        if (fmin > fval32)
                            fmin = fval32;
                        if (fmax < fval32)
                            fmax = fval32;
                    }
                }
                if (n != 1) {
                    iftDestroyImage(&image);
                    iftExit("Error reading file.", "iftReadImageAnalyze");
                }
            }


    
    fclose(fp);
    return img;
}

void iftScn2Ana(const iftImage *img, const char *format, ...) {
    va_list args;
    char filename[IFT_STR_DEFAULT_SIZE];

    va_start(args, format);
    vsprintf(filename, format, args);
    va_end(args);

    char *out_basename = iftBasename(filename);

    Ana_Hdr1 hdr1;
    Ana_Hdr2 hdr2;
    int i;
    FILE *f;
    int maxval;
    iftVoxel v;
    int p, n = 0;
    unsigned char val8;
    short int val16;
    long int val32;

    // Writing header file
    char *out_hdr_pathname = iftConcatStrings(2, out_basename, ".hdr");
    f = fopen(out_hdr_pathname, "wb");
    if (f == NULL) {
        iftExit("Cannot open file: \"%s\"", "iftWriteImageAnalyze", out_hdr_pathname);
    }
    iftFree(out_hdr_pathname);

    memset(&hdr1, 0, sizeof(hdr1));
    memset(&hdr2, 0, sizeof(hdr2));

    for (i = 0; i < 8; i++) {
        hdr2.pixdim[i] = 0.0;
        hdr2.zeroes[i] = 0.0;
    }

    // First header segment
    hdr1.hdrlen = 348;
    hdr1.regular = 'r';

    ana_fio_abs_write_32(f, 0, 0, hdr1.hdrlen);
    ana_fio_abs_write_zeroes(f, 4, 34);
    ana_fio_abs_write_8(f, 38, hdr1.regular);
    ana_fio_abs_write_8(f, 39, hdr1.hkey0);

    // Second header segment

    hdr2.dim[0] = 4;
    hdr2.dim[1] = img->xsize;
    hdr2.dim[2] = img->ysize;
    hdr2.dim[3] = img->zsize;
    hdr2.dim[4] = 1;

    maxval = iftMaximumValue(img);
    hdr2.bpp = 8;
    if (maxval >= 256)
        hdr2.bpp = 16;
    if (maxval >= 65535)
        hdr2.bpp = 32;
    hdr2.data_type = hdr2.bpp / 4;
    hdr2.pixdim[0] = 1.0; //Orientation is lost
    hdr2.pixdim[1] = img->dx;
    hdr2.pixdim[2] = img->dy;
    hdr2.pixdim[3] = img->dz;
    hdr2.maxval = maxval;
    hdr2.minval = 0;

    for (i = 0; i < 8; i++)
        ana_fio_abs_write_16(f, 40 + 0 + 2 * i, 0, hdr2.dim[i]);
    for (i = 0; i < 7; i++)
        ana_fio_abs_write_16(f, 40 + 16 + 2 * i, 0, hdr2.unused[i]);

    ana_fio_abs_write_16(f, 40 + 30, 0, hdr2.data_type);
    ana_fio_abs_write_16(f, 40 + 32, 0, hdr2.bpp);
    ana_fio_abs_write_16(f, 40 + 34, 0, hdr2.dim_un0);

    for (i = 0; i < 8; i++)
        ana_fio_abs_write_float32(f, 40 + 36 + 4 * i, 0, hdr2.pixdim[i]);
    for (i = 0; i < 8; i++)
        ana_fio_abs_write_float32(f, 40 + 68 + 4 * i, 0, hdr2.zeroes[i]);

    ana_fio_abs_write_32(f, 40 + 100, 0, hdr2.maxval);
    ana_fio_abs_write_32(f, 40 + 104, 0, hdr2.minval);

    // Third header segment (patient info)

    ana_fio_abs_write_zeroes(f, 148, 200);
    fclose(f);


    // Writing img file (RAW)
    char *out_img_pathname = iftConcatStrings(2, out_basename, ".img");
    f = fopen(out_img_pathname, "wb");
    if (f == NULL) {
        iftExit("Cannot open file: \"%s\"", "iftWriteImageAnalyze", out_img_pathname);
    }
    iftFree(out_img_pathname);

    for (v.z = 0; v.z < img->zsize; v.z++)
        for (v.y = img->ysize - 1; v.y >= 0; v.y--) // reverse order
            for (v.x = 0; v.x < img->xsize; v.x++) {
                p = v.x + img->tby[v.y] + img->tbz[v.z];
                if (hdr2.bpp == 8) {
                    val8 = (unsigned char) img->val[p];
                    n = fwrite(&val8, sizeof(unsigned char), 1, f);
                }
                if (hdr2.bpp == 16) {
                    val16 = (short int) img->val[p];
                    n = fwrite(&val16, sizeof(short int), 1, f);
                }
                if (hdr2.bpp == 32) {
                    if (hdr2.data_type == 8) { // integer
                        val32 = (long int) img->val[p];
                        n = fwrite(&val32, sizeof(long int), 1, f);
                    }
                    else
                        iftExit("Warning: float scene not supported.", "iftWriteImageAnalyze");
                }
                if (n != 1)
                    iftExit("Error reading img file.", "iftWriteImageAnalyze");
            }
    iftFree(out_basename);
    fclose(f);
}