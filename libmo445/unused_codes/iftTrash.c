void iftUnsupFeatSelecByVPS(iftDataSet *Z, int kmax)
{
  iftUnsupFeatSelProb *prob;
  iftVPS              *vps;
  int i, nteams = (int)(2*Z->nfeats+1);

  prob = iftCreateUnsupFeatSelProb(Z,kmax);
  vps  = iftCreateVPS(Z->nfeats, nteams, (iftVPSFitnessFunc)iftUnsupFeatSelecVPSFitness, prob);
  iftVPSMin(vps);
  iftDestroyUnsupFeatSelProb(&prob);

  for (i=0; i < Z->nfeats; i++){
    Z->alpha[i]=vps->best_team.player[i];
  }

  iftDestroyVPS(&vps);
}


void iftSupFeatSelection(iftDataSet *Z, float train_perc, int nteams)
{
  iftVPS *vps;
  iftSupFeatSelecProblem *prob;
  int i, iter, nfeats=0;
  float *best_alpha=iftAllocFloatArray(Z->nfeats), best_fitness= IFT_INFINITY_FLT_NEG;

  
  if ((train_perc >= 1.0)||(train_perc <= 0.0))
    iftError("Invalid percentage of training samples","iftSupFeatSelection");

  for (iter=0; iter < 10; iter ++) {
    prob  = iftCreateSupFeatSelecProblem(Z, train_perc);
    vps   = iftCreateVPS(Z->nfeats,nteams,iftVPSFitnessForSupFeatSelec,prob);
    iftVPSMax(vps);
    if (vps->best_team.fitness > best_fitness){
      for (i=0; i < Z->nfeats; i++) {
  best_alpha[i]  = vps->best_team.player[i];
      }
      best_fitness = vps->best_team.fitness; 
    }
    iftDestroyVPS(&vps);
    iftDestroySupFeatSelecProblem(&prob);
  }

  for (i=0; i < Z->nfeats; i++) {
    Z->alpha[i]  = best_alpha[i];
    if (best_alpha[i]==1.0)
      nfeats++;
  }
  if (nfeats < Z->nfeats)
    printf("Feature Space Reduced to %f %%\n",(float)nfeats/Z->nfeats*100.0);
  else
    printf("Feature space remained the same\n");
  printf("Best features\n");
  for (i=0; i < Z->nfeats; i++) {
    printf("%d ",(int)best_alpha[i]);
  }
  printf("\n Best Fitness: %f\n",best_fitness);

  iftFree(best_alpha);
}