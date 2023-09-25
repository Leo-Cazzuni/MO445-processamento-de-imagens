#include "iftVPS.h"
/**
@file
@brief A description is missing here
*/
/*--------------------- Private Methods --------------------------------*/ 



/*------------------------ This part was in iftClassification.c -------------------------------- */
/* typedef struct ift_supfeatselecproblem {
   iftDataSet *Z; 
} iftSupFeatSelecProblem; 


iftSupFeatSelecProblem *iftCreateSupFeatSelecProblem(iftDataSet *Z, float train_perc)
{
  iftSupFeatSelecProblem *prob=(iftSupFeatSelecProblem *)iftAlloc(1,sizeof(iftSupFeatSelecProblem));

  prob->Z = Z;


  iftSelectSupTrainSamples(Z,train_perc);

  return(prob);
}

void iftDestroySupFeatSelecProblem(iftSupFeatSelecProblem **prob)
{
  if (*prob != NULL){ 
    iftFree(*prob);
    *prob = NULL;
  }
}


float iftVPSFitnessForSupFeatSelec(void *problem, uchar *theta)
{
  int i, nfeats=0;
  float value;
  iftSupFeatSelecProblem *prob= (iftSupFeatSelecProblem *) problem;
  iftDataSet  *Z=prob->Z;
  iftCplGraph *graph;

  for (i=0; i < Z->nfeats; i++) {
    nfeats += theta[i];
  }

  if (nfeats == 0){ // return maximum number of errors    
    return(Z->nsamples);
  }

  for (i=0; i < Z->nfeats; i++) {
    Z->alpha[i]=(float)theta[i];
  }

  /\* Compute fitness value *\/

  graph   = iftCreateCplGraph(Z);
  iftSupTrain(graph);
  iftClassify(graph,Z);
  iftDestroyCplGraph(&graph);
  
  value = iftSkewedTruePositives(Z);

  return(value);
}

------------ end of part removed from iftClassification.c -------------------- */

/* Funcion removed from iftClustering.c */
/* float iftUnsupFeatSelecVPSFitness(void *problem, float *theta) */
/* { */
/*   float value=0.0; */
/*   int i; */
/*   iftKnnGraph *graph; */
/*   iftUnsupFeatSelProb *prob= (iftUnsupFeatSelProb *) problem; */
/*   iftDataSet *Z=prob->Z; */

/*   for (i=0; i < Z->nfeats; i++) */
/*     prob->Z->alpha[i]=theta[i]; */

/*   /\* Compute fitness value *\/ */

/*   graph = iftCreateKnnGraph(prob->Z,prob->kmax); */
/*     iftUnsupTrain(graph, iftNormalizedCut); */
/*   value = iftNormalizedCut(graph); */
/*   iftDestroyKnnGraph(&graph); */
/*   return(value); */
/* } */


int iftEliminateRepeatedTeams(iftVPS *vps)
{
  int p,r,t, nteams;

  for (t=0; t < vps->nteams; t++) {
    if (vps->team[t].eliminated == 0){
      for (r=t+1; r < vps->nteams; r++) {
	if (vps->team[r].eliminated == 0){
	  for (p=0; p < vps->nplayers; p++) {
	    if (vps->team[t].player[p] != vps->team[r].player[p])  
	      break;
	  }
	  if (p == vps->nplayers) // eliminate replicated team
	    vps->team[r].eliminated = 1;
	}
      }
    }
  }

  nteams=0;
  for (t=0; t < vps->nteams; t++) 
    if (vps->team[t].eliminated==0)
      nteams ++;

  return(nteams);
}



int iftSetTeams(iftVPS *vps)
{
  char *status=iftAllocCharArray(100);
  int i,p,t,j,n, nteams;

 
  for (p=0; p < vps->nplayers; p++) {

    /* reset status for next player */

    for (i=0; i < 100; i++) 
      status[i]=0;

    /* create a probability distribution for a player p to be selected */

    n = iftRound(vps->Ps[p] * 100.0);

    if (n==100){
      for (t=0; t < vps->nteams; t++) {
	vps->team[t].player[p]=1;
      }
    }else{
      if (n == 0){
	for (t=0; t < vps->nteams; t++) {
	  vps->team[t].player[p]=0;
	}
      }else{
	j=0;
	while (j < n) {
	  i = iftRandomInteger(0,99);
	  if (status[i]==0){
	    status[i]=1;
	    j++;
	  }	
	}
	

	/* randomly decide to select or not for each team t the player p
	   according to its probability distribution */

	for (t=0; t < vps->nteams; t++) {
	  i = iftRandomInteger(0,99);
	  vps->team[t].player[p]=status[i];
	}
      }
    }
  }

  iftFree(status);

  // eliminate teams with no players

  for (t=0; t < vps->nteams; t++) {
    vps->team[t].eliminated = 1;
    for (p=0; p < vps->nplayers; p++) {
      if (vps->team[t].player[p]==1) {
	vps->team[t].eliminated = 0; // team is not empty
	break;
      }
    }
  }
  
  // eliminate teams with the same players 

  nteams = iftEliminateRepeatedTeams(vps);

  return(nteams);  
}

void iftSetInitTeams(iftVPS *vps)
{
  int p,t;

  for (t=0; t < vps->nteams; t++) 
    vps->team[t].eliminated = 1;

  for (p=0; p < vps->nplayers; p++) {
    vps->team[0].player[p]=1;
  }
  vps->team[0].eliminated=0;

  for (t=1; t <= vps->nplayers; t++) {
    vps->team[t].eliminated=0;
    for (p=0; p < vps->nplayers; p++) {
      if ((t-1)==p)
	vps->team[t].player[p]=1;
      else
	vps->team[t].player[p]=0;
    }
  }

  if (vps->nplayers > 2){
    for (t=vps->nplayers+1; t < 2*vps->nplayers+1; t++) {
      vps->team[t].eliminated=0;
      for (p=0; p < vps->nplayers; p++) {
	if ((t-vps->nplayers-1)==p)
	  vps->team[t].player[p]=0;
	else
	  vps->team[t].player[p]=1;
      }
    }
  }

  if (vps->nteams > 2*vps->nplayers+1)
    iftEliminateRepeatedTeams(vps);

}

void iftPlayTryouts(iftVPS *vps)
{
  int t;
  
  for (t=0; t < vps->nteams; t++) {
    if (vps->team[t].eliminated == 0){
      vps->team[t].fitness = vps->iftFitness(vps->problem, vps->team[t].player);
      //      printf("team %d: fitness %f \n",t,vps->team[t].fitness);
    }
  }
      
}

char iftSelectBestTeam(iftVPS *vps, uchar order)
{
  float *weight=iftAllocFloatArray(vps->nteams);
  int   *index=iftAllocIntArray(vps->nteams);
  int    t, p, nteams;
  float  sum_weights, max_weight, min_weight;
  char   found_better_team=0;
  
  if (order == IFT_INCREASING){ 

    max_weight = IFT_INFINITY_FLT_NEG; 
    for (t=0; t < vps->nteams; t++) { 
      if (vps->team[t].eliminated==0){
	weight[t]=vps->team[t].fitness;
	index[t]=t;
	if (weight[t]>max_weight){
	  max_weight = weight[t];
	}
      }else{
	weight[t]= IFT_INFINITY_FLT;
      }
    }


    iftFHeapSort(weight, index, vps->nteams, order); 

    nteams = 0;
    for (t=0; t < vps->nteams; t++) { 
      if (weight[t] != IFT_INFINITY_FLT){
	nteams++;
      } else{
	break;
      }
    }

    if (vps->team[index[0]].fitness < vps->best_team.fitness){        
      found_better_team=1;
      for (p=0; p < vps->nplayers; p++) {
	vps->best_team.player[p] = vps->team[index[0]].player[p];
      }
      vps->best_team.fitness   = vps->team[index[0]].fitness;
    }
     
    if (nteams == 1) {
      iftFree(index);
      iftFree(weight);
      return(0);
    }

    for (p=0; p < vps->nplayers; p++) {
      vps->Ps[p]=0.0; 
    }

    sum_weights=0.0; max_weight = vps->team[index[nteams-1]].fitness;    

    for (t=0; t < nteams-1; t++) {
      sum_weights += (max_weight - vps->team[index[t]].fitness);	
      for (p=0; p < vps->nplayers; p++) {
	if (vps->team[index[t]].player[p]==1){
	  vps->Ps[p]  +=  (max_weight - vps->team[index[t]].fitness);
	}
      }
    }

    if (sum_weights > 0.0){
      for (p=0; p < vps->nplayers; p++) {
	vps->Ps[p] = vps->Ps[p]/sum_weights;
      }
    }else{
      iftFree(index);
      iftFree(weight);
      return(0);
    }

  } else { // IFT_DECREASING

    min_weight = IFT_INFINITY_FLT; 
    for (t=0; t < vps->nteams; t++) { 
      if (vps->team[t].eliminated==0){
	weight[t]=vps->team[t].fitness;
	index[t]=t;
	if (weight[t] < min_weight){
	  min_weight = weight[t];
	}
      }else{
	weight[t]= IFT_INFINITY_FLT_NEG;
      }
    }


    iftFHeapSort(weight, index, vps->nteams, order); 

    nteams = 0;
    for (t=0; t < vps->nteams; t++) { 
      if (weight[t] != IFT_INFINITY_FLT_NEG){
	nteams++;
      } else{
	break;
      }
    }
    
    if (vps->team[index[0]].fitness > vps->best_team.fitness){        
      found_better_team=1;
      for (p=0; p < vps->nplayers; p++) {
	vps->best_team.player[p] = vps->team[index[0]].player[p];
      }
      vps->best_team.fitness   = vps->team[index[0]].fitness;
    }

   
    if (nteams == 1) {
      iftFree(index);
      iftFree(weight);
      return(0);
    }

    for (p=0; p < vps->nplayers; p++) {
      vps->Ps[p]=0.0; 
    }

    sum_weights=0.0; min_weight = vps->team[index[nteams-1]].fitness;    
    
    for (t=0; t < nteams-1; t++) {
      sum_weights += (vps->team[index[t]].fitness - min_weight);	
      for (p=0; p < vps->nplayers; p++) {
	if (vps->team[index[t]].player[p]==1){
	  vps->Ps[p]  +=  (vps->team[index[t]].fitness - min_weight);
	}
      }
    }
        
    if (sum_weights > 0.0){
      for (p=0; p < vps->nplayers; p++) {
	vps->Ps[p] = vps->Ps[p]/sum_weights;
      }
    }else{
      iftFree(index);
      iftFree(weight);
      return(0);
    }
  }

  iftFree(index);
  iftFree(weight);

  iftSetTeams(vps);

  return(found_better_team);
}

/*---------------------- Public Methods --------------------------------*/

iftVPS  *iftCreateVPS(int nplayers, int nteams, iftVPSFitnessFunc f, void *prob)
{
  iftVPS *vps = (iftVPS *)iftAlloc(1,sizeof(iftVPS));
  int i;

  if (nteams < 2*nplayers+1){
    iftWarning("Adjusting to have at least nteams=2*nplayers+1","iftCreateVPS");
    nteams = 2*nplayers+1;
  }

  vps->iftFitness = f;
  vps->problem    = prob;
  vps->nteams     = nteams;
  vps->team       = (iftTeam *)iftAlloc(vps->nteams,sizeof(iftTeam));
  vps->Ps         = iftAllocFloatArray(nplayers);
  vps->nplayers   = nplayers;
  for (i=0; i < vps->nteams; i++) {
    vps->team[i].player     = iftAllocUCharArray(nplayers);
  }
  vps->nrounds            = 100;
  vps->best_team.player   = iftAllocUCharArray(nplayers);
  

  return(vps);
}

void  iftDestroyVPS(iftVPS **vps)
{
  iftVPS *aux=*vps;
  int i;

  if (aux != NULL){
    for (i=0; i < aux->nteams; i++) 
      iftFree(aux->team[i].player);
    iftFree(aux->team);
    iftFree(aux->best_team.player);
    iftFree(aux->Ps);
    iftFree(aux);
    *vps = NULL;
  }
}

float  iftVPSMax(iftVPS *vps)
{
  int iter=1;
  char found_better_team;

  vps->best_team.fitness = IFT_INFINITY_FLT_NEG;
  iftSetInitTeams(vps);
 
  do {
    iftPlayTryouts(vps);
    found_better_team=iftSelectBestTeam(vps, IFT_DECREASING);
    iter++;
  } while ((found_better_team)&&(iter < vps->nrounds));

  if (iter > vps->nrounds)
    iftWarning("Reached maximum number of iterations","iftVPSMax");

  return(vps->best_team.fitness);
}


float  iftVPSMin(iftVPS *vps)
{
  int iter=1;
  char found_better_team;

  vps->best_team.fitness = IFT_INFINITY_FLT;
  iftSetInitTeams(vps);
 
  do {
    iftPlayTryouts(vps);
    found_better_team=iftSelectBestTeam(vps, IFT_INCREASING);
    int i;
    for (i=0; i < vps->nplayers; i++){
      printf("%d ",vps->best_team.player[i]);
    }
    printf("\n");
    printf("Fitness %f\n",vps->best_team.fitness);    
    iter++;
  } while ((found_better_team)&&(iter < vps->nrounds));

  if (iter > vps->nrounds)
    iftWarning("Reached maximum number of iterations","iftVPSMin");


  return(vps->best_team.fitness);
}

