#define class class_
extern "C" {
	#include "ift.h"
}
#undef class

#include "graph.h"

iftImage* graph_cut_pixel_segmentation(iftImage* basins, iftLabeledSet *seed, int beta){
	typedef Graph<double,double,double> GraphType;
	GraphType *graph = new GraphType(basins->n, basins->n);

	iftFImage *normalized_basins = iftCreateFImage(basins->xsize,basins->ysize,basins->zsize);

	
	iftAdjRel *adj = iftCircular(1.0);

	int p;
	float maxval = iftMaximumValue(basins);
	for(p = 0; p < basins->n; p++){
	  normalized_basins->val[p] = (float)basins->val[p]/maxval;
	  graph->add_node();
	}

	for(p = 0; p < basins->n; p++){
		iftVoxel u = iftGetVoxelCoord(basins,p);

		int i;
		for(i = 1; i < adj->n; i++){
			iftVoxel v = iftGetAdjacentVoxel(adj,u,i);

			if(iftValidVoxel(basins, v)){
				int q = iftGetVoxelIndex(basins,v);

				if(p < q){
				  double edge_weight = pow(1.0 - (normalized_basins->val[p] + normalized_basins->val[q])/2., beta);
				  graph->add_edge(p,q, edge_weight, edge_weight);
				}
			}
		}
	}

	iftLabeledSet *s = seed;
	while(s){
		p = s->elem;
		int label = s->label;

		if(label == 1)
			graph->add_tweights(p, IFT_INFINITY_FLT, 0);
		else
			graph->add_tweights(p, 0, IFT_INFINITY_FLT);

		s = s->next;
	}

	graph->maxflow();
	iftDestroyFImage(&normalized_basins);
	
	iftImage *segmentation = iftCreateImage(basins->xsize, basins->ysize, basins->zsize);

	for(p = 0; p < segmentation->n; p++){
		if (graph->what_segment(p) == GraphType::SOURCE)
			segmentation->val[p] = 255;
	}

	delete graph;
	iftDestroyAdjRel(&adj);

	return segmentation;
}

int main(int argc, char **argv) {
	if(argc != 6)
		iftError("Usage: gc_pixel [IMAGE_PATH] [SEEDS_PATH] [OUTPUT_PATH] [SPATIAL_RADIUS] [BETA]", "gc_pixel");

	float spatial_radius = atof(argv[4]);
	int beta = atoi(argv[5]);

	iftImage *image = iftReadImageByExt(argv[1]);
	iftLabeledSet *seeds = iftReadSeeds2D(argv[2], image);

	iftAdjRel *adj = iftCircular(spatial_radius);

	iftImage *basins = iftImageBasins(image, adj);
	iftImage *result = graph_cut_pixel_segmentation(basins, seeds, beta);

	iftWriteImageByExt(result, argv[3]);

	return 0;
}
