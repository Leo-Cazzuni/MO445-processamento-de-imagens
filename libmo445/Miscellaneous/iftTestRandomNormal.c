#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ift.h"


int main(void)
{
  const int nrolls=100000;  // number of experiments
  const int nstars=100;    // maximum number of stars to distribute
  const int nbins=15;

  const float std = 2.;

  int p[nbins];
  for(int i=0;i<nbins;i++) p[i] = 0;

  iftRandomSeed(IFT_RANDOM_SEED);

  for (int i=0; i<nrolls; ++i) {
    float number = iftRandomNormalFloat();
    int dist = (int)((std*number));
    if ((dist>=-nbins/2.)&&(dist<+nbins/2.)) ++p[dist+(int)(nbins/2)];
  }

  fprintf(stdout,"normal_distribution (%f,%f):\n",(float)nbins/2,std);

  for (int i=0; i<nbins; ++i) {
    fprintf(stdout,"%3d - %3d : ",i,(i+1));
    for(int j=0;j<(int)((double)p[i]*nstars/nrolls);j++)
      fprintf(stdout,"*");
    fprintf(stdout,"\n");
  }

  return 0;
}
