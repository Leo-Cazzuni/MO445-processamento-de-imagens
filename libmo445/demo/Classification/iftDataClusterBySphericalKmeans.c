#include "ift.h"

int main(int argc, char *argv[]) {
	iftDataSet *Z = NULL, *Zk = NULL, *Zp = NULL;
	iftImage *img = NULL;
	timer *t1 = NULL, *t2 = NULL;

	/*--------------------------------------------------------*/

	int MemDinInicial, MemDinFinal;
	MemDinInicial = iftMemoryUsed();

	/*--------------------------------------------------------*/

	if ((argc != 6) && (argc != 5))
		iftError(
				"Usage: iftDataClusterByKmeans <dataset.data> <n_clusters> <whitening: 0|1> <MaxIterations> <minImprovement|[1E-5]>",
				"main");

	int n_clusters = atoi(argv[2]);
	int whitening = atoi(argv[3]);
	int max_iters = atoi(argv[4]);
	float min_improv;
	if (argc < 6)
		min_improv = 1E-5;
	else
		min_improv = atof(argv[5]);

//	iftRandomSeed(IFT_RANDOM_SEED);
	iftRandomSeed(time(NULL));

	printf("\n--------- Spherical Kmeans ----------\n");
	printf("- n_clusters = %d\n", n_clusters);
	printf("- Apply Whitening: %d\n", whitening);
	printf("- Max Iters: %d\n", max_iters);
	printf("- Min Improv: %f\n", min_improv);
	t1 = iftTic();

	Z = iftReadOPFDataSet(argv[1]);
	iftSetStatus(Z, IFT_TRAIN);
	printf("Z: (nsamples, nfeats): %d, %d\n\n", Z->nsamples, Z->nfeats);

	if (whitening) {
		puts("Applying Whitening Transform");
		Zp = iftWhiteningTransform(Z);
		Zk = iftKmeansInitCentroidsRandomNormal(Zp, n_clusters);
	}
	else {
		Zp = Z;
		Zk = iftKmeansInitCentroidsFromSamples(Zp, n_clusters);
	}

	puts("Running Spherical Kmeans\n");
	iftSphericalKmeansRun(Zp, &Zk, 50);
	t2 = iftToc();

	fprintf(stdout, "clustering in %f ms with %d groups\n", iftCompTime(t1, t2), Zp->nlabels);
	iftPrintNumberOfSamplesPerCluster(Zp);

	if (Zp->nfeats == 2) {
		img = iftDraw2DFeatureSpace(Zp, LABEL, 0);
		iftWriteImageP6(img, "labels.ppm");
		iftDestroyImage(&img);

		img = iftDraw2DFeatureSpace(Zp, STATUS, IFT_TRAIN);
		iftWriteImageP6(img, "train.ppm");
		iftDestroyImage(&img);

		img = iftDraw2DFeatureSpace(Zp, CLASS, 0);
		iftWriteImageP6(img, "classes.ppm");
		iftDestroyImage(&img);

		img = iftDraw2DFeatureSpace(Zp, WEIGHT, 0);
		iftWriteImageP6(img, "weight.ppm");
		iftDestroyImage(&img);
	}

	if (Zp != Z)
		iftDestroyDataSet(&Zp);
	iftDestroyDataSet(&Z);
	iftDestroyDataSet(&Zk);

	/* ---------------------------------------------------------- */

	MemDinFinal = iftMemoryUsed();
	if (MemDinInicial != MemDinFinal)
		printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
				MemDinInicial, MemDinFinal);

	return (0);
}
