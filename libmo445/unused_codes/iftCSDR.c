#include "iftCSDR.h"
/**
@file
@brief A description is missing here
*/
iftOPFBank *iftCreateOPFBank(int nclassifiers)
{
  iftOPFBank *opfbank=(iftOPFBank *)iftAlloc(1,sizeof(iftOPFBank));
  int i;

  opfbank->graph = (iftCplGraph **)iftAlloc(nclassifiers,sizeof(iftCplGraph *));
  if (opfbank->graph==NULL)
    iftError(MSG_MEMORY_ALLOC_ERROR, "iftCreateOPFBank");
  for (i=0; i < nclassifiers; i++) 
    opfbank->graph[i] = NULL;
  opfbank->nclassifiers = nclassifiers;
  return(opfbank);
}


void  iftDestroyOPFBank(iftOPFBank **opfbank)
{
  iftOPFBank *aux=*opfbank;
  int i;

  if (aux != NULL) {
    for (i=0; i < aux->nclassifiers; i++) 
      if (aux->graph[i] != NULL){ 
	if (aux->graph[i]->Z != NULL) 
	  iftDestroyDataSet(&aux->graph[i]->Z);
	iftDestroyCplGraph(&aux->graph[i]);
      }
    iftFree(aux->graph);
    iftFree(aux);
    *opfbank = NULL;
  }
}

iftOPFBank  *iftTrainOPFBankWithCSDRPLS(iftDataSet *Z, iftCSDRPLS *csdr)
{
  int c;
  iftDataSet *Z1;
  iftOPFBank *opfbank;
  iftMatrix *X;

  if ((!iftIsTrainingDataSet(Z))||(!iftIsSupervisedDataSet(Z))||
      (!iftIsNormalizedDataSet(Z)))
    iftError("It requires a supervised and normalized training set","iftTrainOPFBankWithCSDRPLS");

  opfbank = iftCreateOPFBank(csdr->nclasses);

  /* Create feature matrix */

  X = iftDataSetToFeatureMatrix(Z);

  /* Train the classfier of each class */

  for (c=1; c <= csdr->nclasses; c++) {
    Z1 = iftApplyCSDRPLS(Z,X,csdr,c);
    opfbank->graph[c-1] = iftCreateCplGraph(Z1);
    iftSupTrain(opfbank->graph[c-1]);    
  }

  iftDestroyMatrix(&X);


  return(opfbank);
}

void iftClassifyByOPFBankWithCSDRPLS(iftOPFBank *opfbank, iftCSDRPLS *csdr, iftDataSet *Z)
{  
  int c, s;
  iftDataSet *Z2;
  iftMatrix  *X;

  if ((!iftIsTestingDataSet(Z))||(!iftIsNormalizedDataSet(Z)))
    iftError("It requires a  normalized testing set","iftClassifyByOPFBankWithCSDRPLS");

  /* Create Feature Matrix */
 
  X = iftDataSetToFeatureMatrix(Z);

  /* Apply Classification */

  for (s=0; s < Z->nsamples; s++) {
    Z->sample[s].label  = 0;
    Z->sample[s].weight = IFT_INFINITY_FLT;
  }

  for (c=1; c <= csdr->nclasses; c++) {

    Z2 = iftApplyCSDRPLS(Z,X,csdr,c);

    iftBinClassify(opfbank->graph[c-1],Z2);    

    for (s=0; s < Z->nsamples; s++) {
      if (Z2->sample[s].label==1) { // choose the label with minimum cost
	if (Z2->sample[s].weight < Z->sample[s].weight){ 
	  Z->sample[s].weight = Z2->sample[s].weight;
	  Z->sample[s].label  = c;
	}
      }
    }

    iftDestroyDataSet(&Z2);
  }

  iftDestroyMatrix(&X);
}


iftOPFBank *iftTrainOPFBankBySupPCA(iftDataSet *Z, int num_of_comps)
{
    iftDataSet *Zt,*Zp;
    int s,t,c,a,b,ca,cb,i,j,*index,ncomps;
    typedef struct ift_class {
        float *fmin, *fmax;
        int size;
    } iftClass;
    iftClass *classes;
    float *tot_overlap, overlap;
    float fmean_a, fmean_b;
    iftOPFBank *opfbank;

    if ((!iftIsTrainingDataSet(Z))||(!iftIsNormalizedDataSet(Z)))
        iftError("It requires a normalized training set","iftTrainOPFBankBySupPCA");


    if ((num_of_comps > Z->nfeats)||(num_of_comps > Z->nsamples)) {
        iftError("There is no need to reduce feature space","iftTrainOPFBankBySupPCA");
    }

    /* Compute PCA with higher number of components */

    ncomps = iftMin(iftMin(Z->nfeats, Z->nsamples), num_of_comps * 2);

    Zp = iftTransFeatSpaceByPCA(Z,ncomps);

    /* Find minimum and maximum feature values of each class */

    classes = (iftClass *) iftAlloc(Zp->nclasses,sizeof(iftClass));
    for (c=1; c <= Zp->nclasses; c++) {
        classes[c-1].fmin = iftAllocFloatArray(Zp->nfeats);
        classes[c-1].fmax = iftAllocFloatArray(Zp->nfeats);
        classes[c-1].size = 0;
        for (i=0; i < Zp->nfeats; i++) {
            classes[c-1].fmin[i]    = IFT_INFINITY_FLT;
            classes[c-1].fmax[i]    = IFT_INFINITY_FLT_NEG;
        }
    }

    for (s=0; s < Zp->nsamples; s++) {
        classes[Zp->sample[s].truelabel-1].size++;
        for (i=0; i < Zp->nfeats; i++) {
            if (Zp->sample[s].feat[i] < classes[Zp->sample[s].truelabel-1].fmin[i])
                classes[Zp->sample[s].truelabel-1].fmin[i] = Zp->sample[s].feat[i];
            if (Zp->sample[s].feat[i] > classes[Zp->sample[s].truelabel-1].fmax[i])
                classes[Zp->sample[s].truelabel-1].fmax[i] = Zp->sample[s].feat[i];
        }
    }

    /* Compute the num_of_comps (features) that best separate each class from the others */

    tot_overlap = iftAllocFloatArray(Zp->nfeats);
    index       = iftAllocIntArray(Zp->nfeats);
    opfbank     = iftCreateOPFBank(Zp->nclasses);



    for (c=1; c <= Zp->nclasses; c++) {

        /* re-initialize auxiliary arrays */
        for (i=0; i < Zp->nfeats; i++) {
            tot_overlap[i]=0;
            index[i]=i;
        }

        /* use label map to simulate binary classification */
        for (s=0; s < Zp->nsamples; s++){
            if (Zp->sample[s].truelabel == c)
                Zp->sample[s].label = 1;
            else
                Zp->sample[s].label = 2;
        }

        /* compute total overlapping */

        for (s=0; s < Zp->nsamples; s++){
            a  = Zp->sample[s].label;
            for (t=s+1; t < Zp->nsamples; t++){
                b = Zp->sample[t].label;
                if (a != b) {
                    ca = Zp->sample[s].truelabel;
                    cb = Zp->sample[t].truelabel;
                    for (i=0; i < Zp->nfeats; i++) {
                        fmean_a = (classes[ca-1].fmin[i]+ classes[ca-1].fmax[i])/2.0;
                        fmean_b = (classes[cb-1].fmin[i]+ classes[cb-1].fmax[i])/2.0;
                        overlap = 0;
                        if (fmean_a <= fmean_b){
                            if ((Zp->sample[s].feat[i]>= classes[cb-1].fmin[i])&&
                                (Zp->sample[s].feat[i]<= classes[ca-1].fmax[i]))
                                overlap += 1;//(class[ca-1].fmax[i] - Zp->sample[s].feat[i])/(class[ca-1].fmax[i] - class[cb-1].fmin[i])*1.0/class[ca-1].size;
                            if ((Zp->sample[t].feat[i]>= classes[cb-1].fmin[i])&&
                                (Zp->sample[t].feat[i]<= classes[ca-1].fmax[i]))
                                overlap += 1;//(-class[cb-1].fmin[i] + Zp->sample[t].feat[i])/(class[ca-1].fmax[i] - class[cb-1].fmin[i])*1.0/class[cb-1].size;
                        }else{
                            if ((Zp->sample[s].feat[i]>= classes[ca-1].fmin[i])&&
                                (Zp->sample[s].feat[i]<= classes[cb-1].fmax[i]))
                                overlap += 1;//(Zp->sample[s].feat[i] - class[ca-1].fmin[i])/(class[cb-1].fmax[i] - class[ca-1].fmin[i])*1.0/class[ca-1].size;
                            if ((Zp->sample[t].feat[i]>= classes[ca-1].fmin[i])&&
                                (Zp->sample[t].feat[i]<= classes[cb-1].fmax[i]))
                                overlap += 1;//(-Zp->sample[s].feat[i] + class[cb-1].fmax[i])/(class[cb-1].fmax[i] - class[ca-1].fmin[i])*1.0/class[cb-1].size;
                        }
                        tot_overlap[i] += overlap;
                    }
                }
            }
        }


        /* Sort by total overlapping */

        iftFQuickSort(tot_overlap, index, 0, Zp->nfeats-1, IFT_INCREASING);

        /* Create new dataset with the reduced feature space */

        Zt = iftCreateDataSet(Zp->nsamples,num_of_comps);

        Zt->fsp    = iftCopyFeatSpaceParam(Zp);
        Zt->fsp.w  = iftAllocCharArray(Zp->fsp.ncomps);


        for (i=0; i < Zt->nfeats; i++){
            Zt->fsp.w[index[i]]=1; // select num_of_comps
        }

        for (i=0; i < Zt->nfeats; i++){
            Zt->alpha[i]=1.0;
        }

        Zt->iftArcWeight = Zp->iftArcWeight;
        Zt->function_number = Zp->function_number;
        Zt->nclasses     = 2;
        Zt->ref_data     = Zp->ref_data;
        for (s=0; s < Zp->nsamples; s++) {
            Zt->sample[s].truelabel  = Zp->sample[s].label;
            for (i=0, j=0; i < Zt->fsp.ncomps; i++)
                if (Zt->fsp.w[i]==1){
                    Zt->sample[s].feat[j]=Zp->sample[s].feat[i];
                    j++;
                }
            Zt->sample[s].status    = Zp->sample[s].status;
            Zt->sample[s].id        = Zp->sample[s].id;
        }
        Zt->ntrainsamples  = Zp->ntrainsamples;


        /* Train the binary classifier */

        opfbank->graph[c-1] = iftCreateCplGraph(Zt);
        iftSupTrain(opfbank->graph[c-1]);
    }

    /* Free memory */

    for (c=1; c <= Zp->nclasses; c++) {
        iftFree(classes[c-1].fmin);
        iftFree(classes[c-1].fmax);
    }
    iftFree(classes);
    iftFree(tot_overlap);
    iftFree(index);
    iftDestroyDataSet(&Zp);


    return(opfbank);
}

void iftClassifyByOPFBank(iftOPFBank *opfbank, iftDataSet *Z)
{
  int s;

  if ((!iftIsTestingDataSet(Z))||(!iftIsNormalizedDataSet(Z)))
    iftError("It requires a normalized testing set","iftClassifyByOPFBank");

  /* Apply Classification */

  for (s=0; s < Z->nsamples; s++) {
    Z->sample[s].label  = 0;
    Z->sample[s].weight = IFT_INFINITY_FLT;
  }

#pragma omp parallel for shared(Z)
  for (int c=1; c <= Z->nclasses; c++) {
    iftDataSet *Z1, *Z2;
    Z1    = opfbank->graph[c-1]->Z;
    Z2    = iftTransformTestDataSetByPCA(Z1,Z);

    iftBinClassify(opfbank->graph[c-1],Z2);    

    for (int s=0; s < Z->nsamples; s++) {
      if (Z2->sample[s].label==1) { // choose the label with minimum cost
	if (Z2->sample[s].weight < Z->sample[s].weight){ 
	  Z->sample[s].weight = Z2->sample[s].weight;
	  Z->sample[s].label  = c;
	}
      }
    }

    iftDestroyDataSet(&Z2);
  }

}

/*   Use iftNormalizeFeatures before you call this function */

iftCSDRPLS  *iftCreateCSDRPLS(iftDataSet *Z, int num_of_comps)
{
  iftCSDRPLS *csdr=(iftCSDRPLS *)iftAlloc(1,sizeof(iftCSDRPLS));
  iftMatrix  *X, *Xt, *Y, *W, *T, *P;
  int         row,col,s,i, classIdx,c;
  float       norm_factor;

  if ((!iftIsTrainingDataSet(Z))||(!iftIsSupervisedDataSet(Z))||
      (!iftIsNormalizedDataSet(Z)))
    iftError("It requires a supervised and normalized training set","iftCreateCSDRPLS");

  if (num_of_comps >= Z->nfeats) 
    iftError("No need for feature space reduction","iftCreateCSDRPLS");

  /* Allocate space for class specific data representation by PLS */

  csdr->w = (iftMatrix **) iftAlloc(Z->nclasses,sizeof(iftMatrix *));
  for (classIdx =1; classIdx <= Z->nclasses; classIdx++)
    csdr->w[classIdx -1] = iftCreateMatrix(num_of_comps,Z->nfeats);
  csdr->nclasses     = Z->nclasses;
  csdr->nfeats       = Z->nfeats;
  csdr->num_of_comps = num_of_comps;

  /* Create class matrix */

  Y = iftCreateMatrix(1,Z->nsamples);

  for (classIdx =1; classIdx <= Z->nclasses; classIdx++) {

    /* Create the feature matrix and its transpose */
  
    X  = iftCreateMatrix(Z->nfeats, Z->nsamples);
    Xt = iftCreateMatrix(Z->nsamples, Z->nfeats);
    for (row=0, s=0; row < X->nrows; row++,s++) {
      for (col=0,i=0; col < X->ncols; col++,i++) {
	X->val[iftGetMatrixIndex(X,col,row)]   = Z->sample[s].feat[i];
	Xt->val[iftGetMatrixIndex(Xt,row,col)] = Z->sample[s].feat[i];
      }
    }

    /* Create the binary class vector */

    for (row=0, s=0; row < Y->nrows; row++, s++) 
      if (Z->sample[s].truelabel== classIdx)
	Y->val[iftGetMatrixIndex(Y,0,row)] =  1;
      else
	Y->val[iftGetMatrixIndex(Y,0,row)] =  0;

    /* Asymmetric PLS-1 decomposition */

    for (c=1; c <= num_of_comps; c++) {
      
      /* compute a projection vector */

      W = iftMultMatrices(Xt,Y);
      iftNormalizeMatrix(W);

      /* copy the projection vector to the output CSDR projection matrix */

      for (row=0, col=c-1; row < csdr->w[classIdx -1]->nrows; row++)
	csdr->w[classIdx -1]->val[iftGetMatrixIndex(csdr->w[classIdx -1],col,row)] =
	  W->val[iftGetMatrixIndex(W,0,row)];

      /* compute score and loadings with respect to W => update matrices to compute a new projection vector orthogonal to the previous ones */

      T = iftMultMatrices(X,W);
      P = iftMultMatrices(Xt,T);
      norm_factor = iftFrobeniusNorm(T); 
      norm_factor = 1.0/(norm_factor*norm_factor); /* 1.0 / Tt * T */
      for (row=0; row < P->nrows; row++) /* P = Xt * T / (Tt * T) */ 
	P->val[iftGetMatrixIndex(P,0,row)] *= norm_factor;
      for (row=0; row < X->nrows; row++) { /* X = X - T*Pt */
	for (col=0; col < X->ncols; col++) {
	  X->val[iftGetMatrixIndex(X,col,row)]  += - (T->val[iftGetMatrixIndex(T,0,row)]*P->val[iftGetMatrixIndex(P,0,col)]);
	  Xt->val[iftGetMatrixIndex(Xt,row,col)] = X->val[iftGetMatrixIndex(X,col,row)];
	}
      }

      /* free memory for the next component iteration */
      
      iftDestroyMatrix(&W);
      iftDestroyMatrix(&T);
      iftDestroyMatrix(&P);
  
    }
    
    /* free memory for the next class iteration */

    iftDestroyMatrix(&X);
    iftDestroyMatrix(&Xt);
  }

  iftDestroyMatrix(&Y);

  return(csdr);
}


void iftDestroyCSDRPLS(iftCSDRPLS **csdr)
{
  iftCSDRPLS *aux=*csdr;
  int c;

  if (aux != NULL){
    for (c=1; c <= aux->nclasses; c++) {
      if (aux->w[c-1] != NULL) 
	iftDestroyMatrix(&aux->w[c-1]);
    }
    iftFree(aux->w);
    iftFree(aux);
    *csdr = NULL;
  }    
}


iftDataSet *iftApplyCSDRPLS(iftDataSet *Z, iftMatrix *X, iftCSDRPLS *csdr, int truelabel) {
    iftDataSet *Z1 = NULL;
    iftMatrix *Xp;
    int i, s, row, col;

    if (!iftIsNormalizedDataSet(Z))
        iftError("It requires a normalized dataset", "iftApplyCSDRPLS");

    /* Project features in Xp (nsamples x ncomps) */

    Xp = iftMultMatrices(X, csdr->w[truelabel - 1]);

    /* Create new dataset with the projected features */

    Z1 = iftCreateDataSet(Z->nsamples, csdr->num_of_comps);
    Z1->iftArcWeight = Z->iftArcWeight;
    Z1->function_number = Z->function_number;
    Z1->nlabels = 0;
    Z1->nclasses = 2;
    Z1->ref_data = Z->ref_data;
    for (row = 0, s = 0; row < Xp->nrows; row++, s++) {

        for (col = 0, i = 0; col < Xp->ncols; col++, i++) {
            Z1->sample[s].feat[i] = Xp->val[iftGetMatrixIndex(Xp, col, row)];
        }

        if (truelabel == Z->sample[s].truelabel)
            Z1->sample[s].truelabel = 1;
        else
            Z1->sample[s].truelabel = 2;

        Z1->sample[s].label = 0;
        Z1->sample[s].status = Z->sample[s].status;
        Z1->sample[s].weight = Z->sample[s].weight;
        Z1->sample[s].id = Z->sample[s].id;
    }

    Z1->ntrainsamples = Z->ntrainsamples;

    for (i = 0; i < Z1->nfeats; i++)
        Z1->alpha[i] = 1.0;

    iftDestroyMatrix(&Xp);

    return (Z1);
}
