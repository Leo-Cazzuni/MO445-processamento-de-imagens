//This inclusion must precede ift.h because the latter defines the macros MAX and MIN.
#include "libdgc/graph.h"

#define class class_
extern "C" {
	#include "ift.h"
}
#undef class

typedef Graph<double,double,double> GraphType;

GraphType* create_graph(iftRegionGraph* region_graph, float beta){
	GraphType *graph = new GraphType(region_graph->nnodes, region_graph->nnodes);
	iftDataSet *dataset = region_graph->dataset;

	float max_dist = -INFINITY_FLT;
	float min_dist = INFINITY_FLT;
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

	return graph;
}

iftImage* graph_cut_diff_segmentation(GraphType *graph, int nnodes, iftImage *label_image, iftLabeledSet *new_seeds){
	iftBMap *labeled = iftCreateBMap(nnodes);
	int r;
	iftLabeledSet *s = new_seeds;
	while(s){
		int p = s->elem;

		r = label_image->val[p] - 1;

		if(!iftBMapValue(labeled, r)){
			int label = s->label;

			if(label == 1)
				graph->edit_tweights(r, INFINITY_FLT, 0);
			else
				graph->edit_tweights(r, 0, INFINITY_FLT);

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
			segmentation->val[p] = 1;
	}

	iftDestroyBMap(&labeled);

	return segmentation;
}

int main(int argc, char **argv) {
	if(argc != 9)
		iftError("Usage: gc_super [IMAGE_PATH] [SEEDS_PATH1] [SEEDS_PATH2] [OUTPUT_PATH1] [OUTPUT_PATH2] [SPATIAL_RADIUS] [VOLUME THRESHOLD] [BETA]", "gc_super");

	float spatial_radius = atof(argv[6]);
	int volume_threshold = atoi(argv[7]);
	float beta = atof(argv[8]);

	iftImage *image = iftReadImageP6(argv[1]);
	iftLabeledSet *seeds1 = iftReadSeeds2D(argv[2], image);
	iftLabeledSet *seeds2 = iftReadSeeds2D(argv[3], image);

	iftAdjRel *adj = iftCircular(spatial_radius);
	iftAdjRel *adj1 = iftCircular(1.0);

	iftImage *basins = iftImageBasins(image, adj);
	iftImage *marker = iftVolumeClose(basins, volume_threshold);
	iftImage *label = iftWaterGray(basins, marker, adj);

	iftDataSet *dataset = iftSupervoxelsToDataSet(image, label);
	dataset->alpha[0] = 0.2;

	iftRegionGraph *region_graph = iftRegionGraphFromLabelImage(label, dataset, adj1);

	GraphType* graph = create_graph(region_graph, beta);

	iftImage *result = graph_cut_diff_segmentation(graph, region_graph->nnodes, label, seeds1);

	//First result
	iftWriteImageP5(result, argv[4]);

	iftDestroyImage(&result);

	result = graph_cut_diff_segmentation(graph, region_graph->nnodes, label, seeds2);

	//Second result
	iftWriteImageP5(result, argv[5]);

	iftDestroyImage(&result);

	return 0;
}
