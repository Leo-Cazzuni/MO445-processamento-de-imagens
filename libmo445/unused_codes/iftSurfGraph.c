#include "iftSFGraph.h"
/**
@file
@brief A description is missing here
*/
// Private methods of this module


char iftValidFloodingArc(iftSFGraph *graph, int s, int t)
{
  iftAdjSet *adj;
  int counter=0; // counts the number of adjacent nodes on the geodesic path

  if (graph->node[t].label==0){
    adj=graph->node[s].adj;
    while (adj != NULL){
      if (graph->node[adj->node].label==1)
	counter++;
      adj = adj->next;
    }
  }else{
    return(0);
  }

  if (counter >= 2) 
    return(0);
  else
    return(1);
}

char iftJordanTest(iftSFGraph *graph)
{
  int r= IFT_NIL,s,t;
  iftLIFO *L=iftCreateLIFO(graph->nnodes);
  iftAdjSet *adj;


  for (s=0; s < graph->nnodes; s++) 
    if (graph->node[s].label==1){
      r = s; 
      iftInsertLIFO(L,s);
      break;
    }
  
  while (!iftEmptyLIFO(L)){
    s = iftRemoveLIFO(L);
    adj = graph->node[s].adj;
    while (adj != NULL) {
      t = adj->node;
      if (graph->node[t].label==1){
	if (L->color[t] == IFT_WHITE){
	  iftInsertLIFO(L,t);
	}
      }
      adj = adj->next;
    }
  }
  iftDestroyLIFO(&L);


  if (r != IFT_NIL){
    adj = graph->node[s].adj;
    while (adj != NULL) {
      t = adj->node;
      if (t==r){
	return(1);
      }
      adj = adj->next;
    }
  }

  return(0);
}


iftGLTable *iftCreateGLTable(int n) 
{
  int s,t;
  iftGLTable *glt=(iftGLTable *)iftAlloc(1,sizeof(iftGLTable));

  glt->len = (float **) iftAlloc(n,sizeof(float *));
  if (glt->len != NULL){
    for (s=0; s < n; s++) 
      glt->len[s] = iftAllocFloatArray(n);
  }else{
    iftError(MSG_MEMORY_ALLOC_ERROR, "iftCreateGLTable");
  }
  glt->n = n;

  for (t=0; t < n; t++) 
    for (s=0; s < n; s++) 
      glt->len[t][s]= IFT_NIL;

  return(glt);
}

void iftDestroyGLTable(iftGLTable **glt)
{
  iftGLTable *aux=*glt;
  int s;

  if (aux != NULL) {
    for (s=0; s < aux->n; s++) 
      iftFree(aux->len[s]);
    iftFree(aux->len);
    iftFree(aux);
    *glt = NULL;
  }
}


// Public methods

iftSFGraph *iftCreateSFGraph(int nnodes)
{
    iftSFGraph *graph=(iftSFGraph *)iftAlloc(1,sizeof(iftSFGraph));
    int s;

    graph->nnodes = nnodes;
    graph->node   = (iftSFNode *)iftAlloc(nnodes,sizeof(iftSFNode));
    if (graph->node == NULL)
    {
      iftError(MSG_MEMORY_ALLOC_ERROR, "iftCreateSFGraph");
    }

    graph->pathval = iftAllocFloatArray(nnodes);
    //    graph->glt     = iftCreateGLTable(nnodes);
    graph->img     = NULL;
    graph->Q       = iftCreateFHeap(nnodes,graph->pathval);
    for (s=0; s < graph->nnodes; s++)
    {
        graph->node[s].adj      = NULL;
        graph->node[s].voxel    = IFT_NIL;
	graph->node[s].label    = 0;
	graph->node[s].pred     = IFT_NIL;
    }

    return(graph);
}

void iftDestroySFGraph(iftSFGraph **graph)
{
    int s;
    iftSFGraph *aux=(*graph);

    if (aux!=NULL)
    {
        for (s=0; s < aux->nnodes; s++)
        {
            if (aux->node[s].adj != NULL)
	      iftDestroyAdjSet(&(aux->node[s].adj));
        }
        iftFree(aux->node);
        iftFree(aux->pathval);
	//	iftDestroyGLTable(&aux->glt);
	iftDestroyFHeap(&(aux->Q));
        iftFree(aux);
        (*graph) = NULL;
    }
}

iftImage *iftSFGraphLabel(iftSFGraph *graph)
{
  iftImage *img=NULL;
  int p,s;

  if (graph->img == NULL) 
    iftError("Cannot create label image from graph","iftSFGraphLabel"); 

  img=iftCreateImage(graph->img->xsize,graph->img->ysize,graph->img->zsize);

  for (s=0; s < graph->nnodes; s++) {
    p = graph->node[s].voxel;
    img->val[p]=graph->node[s].label+1;
  }

  return(img);
}

iftFImage *iftSFGraphPathVal(iftSFGraph *graph)
{
  iftFImage *img=NULL;
  int p,s;

  if (graph->img == NULL) 
    iftError("Cannot create path value image from graph","iftSFGraphPathVal"); 

  img=iftCreateFImage(graph->img->xsize,graph->img->ysize,graph->img->zsize);

  for (s=0; s < graph->nnodes; s++) {
    p = graph->node[s].voxel;
    img->val[p]=graph->pathval[s];
  }

  return(img);
}

iftSFGraph *iftSurfaceGraph(iftImage *root, iftAdjRel *A)
{
  iftSFGraph *graph=NULL;
  int s,t,i,p,q,nnodes;
  iftVoxel u,v;
  float    arcw[A->n];
  
  for (i=0; i < A->n; i++) {
    arcw[i] = sqrtf((A->dx[i]*A->dx[i])+(A->dy[i]*A->dy[i])+
		    (A->dz[i]*A->dz[i]));
  }
  nnodes=0;
  for (p=1; p < root->n; p++) // root[0]=0 must never be a surface point
    if (root->val[p]==p){
      nnodes++;
    }

  graph = iftCreateSFGraph(nnodes);
  graph->img = root;

  s=0;
  for (p=1; p < root->n; p++) 
    if (root->val[p]==p){
      graph->node[s].voxel = p; s++;
    }

  for (s=0; s < graph->nnodes; s++) {

    p   = graph->node[s].voxel;
    u.x = iftGetXCoord(root,p);
    u.y = iftGetYCoord(root,p);
    u.z = iftGetZCoord(root,p);

    for (i=1; i < A->n; i++) {

      v.x = u.x + A->dx[i];
      v.y = u.y + A->dy[i];
      v.z = u.z + A->dz[i];

      if (iftValidVoxel(root,v)){

	q = iftGetVoxelIndex(root,v);

	if (root->val[q]==q){

	  for (t=0; t < graph->nnodes; t++) 
	    if (graph->node[t].voxel==q)
	      iftInsertAdjSet(&(graph->node[s].adj),t,arcw[i]);
	}
      }
    }
  }

  return(graph);
}

 char iftAdjacentSFNodes(iftSFGraph *graph, int s, int t, float *arcw)
{
  iftAdjSet *adj;

  adj = graph->node[s].adj;
  while (adj != NULL) {
    if (adj->node==t){
      *arcw=adj->arcw;
      return(1);
    }
    adj = adj->next;
  }

  *arcw=0.0;
  return(0);
}

float iftMaxGeodesicMeasure(iftSFGraph *graph, int *R, int nroots)
{
  int         s,i,j,r[nroots];
  float       gl,maxgl=0.0,arcw=0.0;

  for (s=0; s < graph->nnodes; s++) {
	graph->node[s].label   = 0;
	for (i=0; i < nroots; i++) 
	  if (graph->node[s].voxel==R[i])
	    r[i]=s;
  }

  
  for (i=0; i < nroots; i++) {    

    for (j=i+1; j < nroots; j++) {      

      /*
	if (graph->glt->len[r[i]][r[j]]==NIL){
	
	if (!iftAdjacentSFNodes(graph,r[i],r[j],&arcw)){
	
	graph->glt->len[r[i]][r[j]] = iftGeodesicLength(graph,r[i],r[j]);
	graph->glt->len[r[j]][r[i]] = graph->glt->len[r[i]][r[j]];
	
	s=r[j];	  
	while(s != NIL){
	graph->node[s].label   = 1;	    
	s = graph->node[s].pred;
	}
	
	}else{
	graph->node[r[i]].label   = 1;	    
	graph->node[r[j]].label   = 1;
	graph->glt->len[r[j]][r[i]] = graph->glt->len[r[i]][r[j]] = arcw;	    
	}
	
	}
	
	if (graph->glt->len[r[i]][r[j]] > maxgl){
	maxgl = graph->glt->len[r[i]][r[j]];
	}
      */
      
      if (!iftAdjacentSFNodes(graph,r[i],r[j],&arcw)){
	  	  
	gl = iftGeodesicLength(graph,r[i],r[j]);

	s=r[j];	  

	while(s != IFT_NIL){ // mark geodesic path on the surface
	  graph->node[s].label   = 1;	    
	  s = graph->node[s].pred;
	}
	
      }else{ // mark geodesic path on the surface
	graph->node[r[i]].label   = 1;	    
	graph->node[r[j]].label   = 1;
	gl = arcw;	    
      }
    
      if (gl > maxgl){
	maxgl = gl;
      }
      
    }
  }

  
  if (maxgl > 0.0){     
    if (iftJordanTest(graph)){
      return(iftGeodesicArea(graph));		
    }
  }

  return(maxgl);   
}


float iftGeodesicLength(iftSFGraph *graph, int r1, int r2)
{
  int         s,t;
  float       tmp,gl=0.0;
  iftAdjSet  *adj;


  // Initialization 

  for (s=0; s < graph->nnodes; s++) {
    graph->pathval[s]      = IFT_INFINITY_FLT;
    graph->node[s].pred    = IFT_NIL;
  }
  graph->pathval[r1] = 0.0;
  iftInsertFHeap(graph->Q,r1);

  // Image Foresting Transform from r1 to r2

  while(!iftEmptyFHeap(graph->Q)) {
    s=iftRemoveFHeap(graph->Q);

    if (s==r2) {
      gl = graph->pathval[s];            
      while(!iftEmptyFHeap(graph->Q)) 
      	iftRemoveFHeap(graph->Q);
      break;
    }
 
    adj = graph->node[s].adj;
    while (adj != NULL){ 
      t   = adj->node;
      if(graph->Q->color[t] != IFT_BLACK){
	tmp = graph->pathval[s] + adj->arcw;
	if (tmp < graph->pathval[t]){
	  graph->node[t].pred = s;
	  graph->pathval[t]   = tmp;
	  if(graph->Q->color[t] == IFT_WHITE)
	    iftInsertFHeap(graph->Q, t);
	  else
	    iftGoUpFHeap(graph->Q, graph->Q->pos[t]);
	}
      }
      adj = adj->next;
    }
  }

  iftResetFHeap(graph->Q);

  return(gl);
}


float iftGeodesicArea(iftSFGraph *graph)
{
  int         r,s,t,l,ncomp=0;
  iftAdjSet  *adj;
  iftFIFO    *F=iftCreateFIFO(graph->nnodes);
  iftFSet    *C=NULL;
  float       maxarea,area,totarea;

  l=2;

  for (r=0; r < graph->nnodes; r++) {

    if (graph->node[r].label==0){ 

      iftInsertFSet(&C,0.0);
      graph->node[r].label=l;
      iftInsertFIFO(F,r);
      
      while(!iftEmptyFIFO(F)){
	s = iftRemoveFIFO(F);
      
	adj = graph->node[s].adj;
	while (adj != NULL){ 
	  t   = adj->node;
	  
	  if (iftValidFloodingArc(graph,s,t)){
	    C->elem += adj->arcw;
	    graph->node[t].label=l;
	    iftInsertFIFO(F,t);
	  }
	  adj = adj->next;
	}
      }
      l++; ncomp++;
    }
  }

  iftDestroyFIFO(&F);


  if (ncomp <= 1){
    iftDestroyFSet(&C);
    return(IFT_NIL);
  }

  maxarea = 0.0; totarea=0.0;
  while (C != NULL){
    area = iftRemoveFSet(&C);
    totarea += area;
    if (area > maxarea)
     maxarea = area;    
  }
  return(totarea-maxarea);
}


// Code that maybe necessary


/*
int iftCollapseNode(iftIGraph *graph, int r1, int r2)
{
  int         s,t;
  float       tmp;
  iftAdjSet  *adj;

  // Initialization 

  for (s=0; s < graph->nnodes; s++) {
    graph->pathval[s] = IFT_INFINITY_FLT;
    graph->node[s].label = NIL;
  }

  graph->pathval[r1] = 0.0;
  graph->label[r1]    = r1;
  iftInsertFHeap(graph->Q,r1);
  graph->pathval[r2]= 0.0;
  graph->label[r2]   = r2;
  iftInsertFHeap(graph->Q,r2);


  // Image Foresting Transform

  while(!iftEmptyFHeap(graph->Q)) {
    s=iftRemoveFHeap(graph->Q);


    adj = graph->node[s].adj;
    while (adj != NULL){ 
      t   = adj->node;
      if(graph->Q->color[t]!=IFT_BLACK){	
	tmp = graph->pathval[s] + adj->arcw;
	if (tmp < graph->pathval[t]){
	  graph->node[t].label = graph->node[s].label;
	  graph->pathval[t]  = tmp;
	  iftUpdateFHeap(graph->Q, t);
	}
      }else{
	if(graph->node[t].label != graph->node[s].label) { // a collapse
	  while(!iftEmptyFHeap(graph->Q)) 
	    iftRemoveFHeap(graph->Q);
	  return(s);
	}
      }
      adj = adj->next;
    }
  }
  iftError("Could not find the geodesic path","iftGeodesicLength");
  return(NIL);
}

float iftAngleBetweenVectors(iftVector V1, iftVector V2)
{
  float mod1,mod2,x;

  mod1 = sqrtf((V1.x*V1.x) + (V1.y*V1.y) + (V1.z*V1.z));
  mod2 = sqrtf((V2.x*V2.x) + (V2.y*V2.y) + (V2.z*V2.z));
  if (mod1 != 0.0) {
    V1.x /= mod1;
    V1.y /= mod1;
    V1.z /= mod1;
  }else
    return(NIL);

  if (mod2 != 0.0) {
    V2.x /= mod2;
    V2.y /= mod2;
    V2.z /= mod2;
  }else
    return(NIL);

  x = V1.x*V2.x + V1.y*V2.y + V1.z*V2.z;
  if (x < -1.0) x = -1.0;
  if (x >  1.0) x =  1.0;
  return (180.0*acosf(x)/PI);
}


*/
