#define class class_
extern "C" {
	#include "ift.h"
}
#undef class

#include "graph.h"

iftImage* graph_cut_superpixel_segmentation(iftRegionGraph* region_graph, iftImage* label_image, iftLabeledSet *seed, int beta){
	typedef Graph<double,double,double> GraphType;
	GraphType *graph = new GraphType(region_graph->nnodes, region_graph->nnodes);
	iftDataSet *dataset = region_graph->dataset;

	float max_dist = IFT_INFINITY_FLT_NEG;
	float min_dist = IFT_INFINITY_FLT;
	int r;
	for(r = 0; r < region_graph->nnodes; r++){
		graph->add_node();

		iftSet *adj = region_graph->node[r].adjacent;
		while(adj){
			int v = adj->elem;
			float dist = dataset->iftArcWeight(dataset->sample[r].feat,dataset->sample[v].feat, dataset->alpha,dataset->nfeats);
			if(dist > max_dist)
				max_dist = dist;
			if(dist < min_dist)
				min_dist = dist;
			adj = adj->next;
		}
	}

	for(r = 0; r < region_graph->nnodes; r++){
		iftSet *adj = region_graph->node[r].adjacent;
		while(adj){
			int v = adj->elem;
			if(r < v){
				float dist = dataset->iftArcWeight(dataset->sample[r].feat,dataset->sample[v].feat, dataset->alpha,dataset->nfeats);
				dist = (dist - min_dist)/(max_dist - min_dist);
				float similarity = exp(-dist/0.5);

				double edge_weight = pow(similarity, beta);
				graph->add_edge(r,v, edge_weight, edge_weight);
			}

			adj = adj->next;
		}
	}

	iftBMap *labeled = iftCreateBMap(region_graph->nnodes);

	iftLabeledSet *s = seed;
	while(s){
		int p = s->elem;

		r = label_image->val[p] - 1;

		if(!iftBMapValue(labeled, r)){
			int label = s->label;

			if(label == 1)
				graph->add_tweights(r, IFT_INFINITY_FLT, 0);
			else
				graph->add_tweights(r, 0, IFT_INFINITY_FLT);

			iftBMapSet1(labeled,r);
		}

		s = s->next;
	}

	graph->maxflow();

	iftImage *segmentation = iftCreateImage(label_image->xsize, label_image->ysize, label_image->zsize);

	int p;
	for(p = 0; p < segmentation->n; p++){
		r = label_image->val[p] - 1;

		if (graph->what_segment(r) == GraphType::SOURCE)
			segmentation->val[p] = 255;
	}

	delete graph;
	iftDestroyBMap(&labeled);

	return segmentation;
}

int main(int argc, char **argv) {
	if(argc != 7)
		iftError("Usage: gc_super [IMAGE_PATH] [SEEDS_PATH] [OUTPUT_PATH] [SPATIAL_RADIUS] [VOLUME THRESHOLD] [BETA]", "gc_super");

	float spatial_radius = atof(argv[4]);
	int volume_threshold = atoi(argv[5]);
	int beta = atoi(argv[6]);

	iftImage *image = iftReadImageP6(argv[1]);
	iftLabeledSet *seeds = iftReadSeeds2D(argv[2], image);

	iftAdjRel *adj = iftCircular(spatial_radius);
	iftAdjRel *adj1 = iftCircular(1.0);

	iftImage *basins = iftImageBasins(image, adj);
	iftImage *marker = iftVolumeClose(basins, volume_threshold, NULL);
	iftImage *label = iftWaterGray(basins, marker, adj);

	iftDataSet *dataset = iftSupervoxelsToDataSet(image, label, NULL);
	dataset->alpha[0] = 0.2;

	iftRegionGraph *region_graph = iftRegionGraphFromLabelImage(label, dataset, adj1);

	iftImage *result = graph_cut_superpixel_segmentation(region_graph, label, seeds, beta);

	iftWriteImageP5(result, argv[3]);

	return 0;
}
