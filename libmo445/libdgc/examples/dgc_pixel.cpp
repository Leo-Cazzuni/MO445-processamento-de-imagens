//This inclusion must precede ift.h because the latter defines the macros MAX and MIN.
#include "libdgc/graph.h"

#define class class_
extern "C" {
#include "ift.h"
}
#undef class

typedef Graph<double, double, double> GraphType;

GraphType *create_graph(iftImage *img, iftAdjRel *A, float beta)
{
    GraphType *graph = new GraphType(img->n, img->n);

    float max_dist = iftMaximumValue(img);
    float min_dist = iftMinimumValue(img);

    int p, q, i;
    iftVoxel v, u;
    for (p = 0; p < img->n; p++)
        graph->add_node();

    for (p = 0; p < img->n; p++)
    {
        u = iftGetVoxelCoord(img, p);
        for (i = 1; i < A->n; i++)
        {
            v = iftGetAdjacentVoxel(A, u, i);
            if (iftValidVoxel(img, v))
            {
                q = iftGetVoxelIndex(img, v);
                if (p < q)
                {
                    float dist = (img->val[q] - min_dist) / (max_dist - min_dist);
                    float similarity = exp(-dist / 0.5);
                    double edge_weight = pow(similarity, beta);
                    graph->add_edge(p, q, edge_weight, edge_weight);
                }
            }
        }
    }
    return graph;
}

iftImage *graph_cut_diff_segmentation(GraphType *graph, iftImage *basins, iftLabeledSet *new_seeds)
{
    iftLabeledSet *s = new_seeds;
    int p, label;
    while (s)
    {
        p = s->elem;
        label = s->label;

        if (label == 1)
            graph->edit_tweights(p, INFINITY_FLT, 0);
        else
            graph->edit_tweights(p, 0, INFINITY_FLT);

        s = s->next;
    }

    graph->maxflow();

    iftImage *segmentation = iftCreateImage(basins->xsize, basins->ysize, basins->zsize);

    for (p = 0; p < segmentation->n; p++)
    {
        if (graph->what_segment(p) == GraphType::SOURCE)
            segmentation->val[p] = 1;
    }

    return segmentation;
}

int main(int argc, char **argv)
{
    if (argc != 7)
        iftError("Usage: gc_super [IMAGE_PATH] [SEEDS_PATH1] [SEEDS_PATH2] [OUTPUT_PATH1] [OUTPUT_PATH2] [BETA]", "gc_super");

    float beta = atof(argv[6]);

    iftImage *image = iftReadImageP6(argv[1]);
    iftLabeledSet *seeds1 = iftReadSeeds2D(argv[2], image);
    iftLabeledSet *seeds2 = iftReadSeeds2D(argv[3], image);

    iftAdjRel *adj = iftCircular(sqrtf(2));

    iftImage *basins = iftImageBasins(image, adj);
    GraphType *graph = create_graph(basins, adj, beta);

    iftImage *result = graph_cut_diff_segmentation(graph, basins, seeds1);

    //First result
    iftWriteImageP5(iftNormalize(result, 0, 255), argv[4]);

    iftDestroyImage(&result);

    result = graph_cut_diff_segmentation(graph, basins, seeds2);

    //Second result
    iftWriteImageP5(iftNormalize(result, 0, 255), argv[5]);

    iftDestroyImage(&result);
    delete graph;

    return 0;
}
