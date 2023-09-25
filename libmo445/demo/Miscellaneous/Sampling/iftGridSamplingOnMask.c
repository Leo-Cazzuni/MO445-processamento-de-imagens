#include "ift.h"


/************************** HEADERS **************************/
iftDict *iftGetArgs(int argc, const char *argv[]);
void iftGetRequiredArgs(const iftDict *args, char **mask_path, float *radius, long *n_samples, char **out_grid_path);
void iftWriteGrid(const iftIntArray *grid, char *path);
iftIntArray *_iftGeodesicGridSamplingOnMask(const iftImage *bin_mask, int radius, long n_samples);
/*************************************************************/


int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArgs(argc, argv);

    char *mask_path = NULL;
    float radius = 0.0;
    long n_samples = 0;
    char *out_grid_path = NULL;

    iftGetRequiredArgs(args, &mask_path, &radius, &n_samples, &out_grid_path);
    iftRandomSeed(time(NULL));

    timer *t1 = iftTic();

    iftImage *mask = iftReadImageByExt(mask_path);
    iftIntArray *grid = iftGridSamplingOnMask(mask, radius, -1, n_samples);
    printf("grid->n: %ld\n", grid->n);
    iftWriteGrid(grid, out_grid_path);

    if (iftDictContainKey("--output-grid-image", args, NULL)) {
        iftImage *out_grid_img = iftCreateImageFromImage(mask);
        iftIntArrayToImage(grid, out_grid_img, 255);
        iftWriteImageByExt(out_grid_img, iftGetConstStrValFromDict("--output-grid-image", args));
        iftDestroyImage(&out_grid_img);
    }

    puts("- Checking");
    for (int i = 0; i < grid->n; i++) {
        iftVoxel u = iftGetVoxelCoord(mask, grid->val[i]);
        
        for (int j = i+1; j < grid->n; j++) {
            iftVoxel v = iftGetVoxelCoord(mask, grid->val[j]);
            
            float dist = iftVoxelDistance(u, v);
            if (dist < radius)
                iftError("u: (%d, %d, %d)\nv: (%d, %d, %d)\ndist = %f > %f\n", "main",
                         u.x, u.y, u.z, v.x, v.y, v.z, dist, radius);
        }
    }

    puts("\nDone...");
    puts(iftFormattedTime(iftCompTime(t1, iftToc())));
    

    // DESTROYERS
    iftDestroyDict(&args);
    iftDestroyImage(&mask);
    iftDestroyIntArray(&grid);

    return 0;
}


/************************** SOURCES **************************/
iftDict *iftGetArgs(int argc, const char *argv[]) {
    char program_description[2048] = \
        "- Apply a grid sampling on a mask image according to a radius, selecting randomly k samples from the grid.\n" \
        "- The grid does not have actually the regular 'shape' of a grid. The found samples of this grid respect the " \
        "topology of a grid with a stride of <radius/stride>, but it can have an irregular shape.\n\n" \
        "- This program only works with a binary mask with a single connect component.";

    iftCmdLineOpt cmd_line_opts[] = {
        {.short_name = "-i", .long_name = "--input-binary-mask", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Binary mask where the topological grid sampling is applied."},
        {.short_name = "-r", .long_name = "--radius", .has_arg=true, .arg_type=IFT_DBL_TYPE,
         .required=true, .help="Radius (stride) between the samples of the grid."},
        {.short_name = "-n", .long_name = "--num-of-samples", .has_arg=true, .arg_type=IFT_LONG_TYPE,
         .required=true, .help="Number of samples randomly selected from the grid. If -1, all samples are considered."},
        {.short_name = "-o", .long_name = "--output-chosen-samples", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=true, .help="Output filename to save the indices of the chosen samples. " \
                                "If *.npy, a numpy file is saved. If *.csv, a CSV file is saved."},
        {.short_name = "-l", .long_name = "--output-grid-image", .has_arg=true, .arg_type=IFT_STR_TYPE,
         .required=false, .help="Output image where each chosen points/voxels has label 1."},
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}



void iftGetRequiredArgs(const iftDict *args, char **mask_path, float *radius, long *n_samples, char **out_grid_path) {
    *mask_path = iftGetStrValFromDict("--input-binary-mask", args);
    *radius = iftGetDblValFromDict("--radius", args);
    *n_samples = iftGetLongValFromDict("--num-of-samples", args);
    *out_grid_path = iftGetStrValFromDict("--output-chosen-samples", args);

    if (*radius <= 0)
        iftError("Radius %f is <= 0.0", "iftGetRequiredArgs", *radius);

    if (!iftRegexMatch(*out_grid_path, "^.*\\.(npy|csv)$"))
        iftError("Invalid extension for output grid file %s\nTry *.npy or *.csv", "iftGetRequiredArgs", iftFileExt(*out_grid_path));

    puts("-----------------------");
    printf("- Mask Path: %s\n", *mask_path);
    printf("- Radius: %f\n", *radius);
    printf("- Number of Samples: %ld\n", *n_samples);
    printf("- Output grid file: %s\n", *out_grid_path);
    puts("-----------------------");
}


void iftWriteGrid(const iftIntArray *grid, char *path) {
    char *parent_dir = iftParentDir(path);
    if (!iftDirExists(parent_dir))
        iftMakeDir(parent_dir);
    iftFree(parent_dir);

    if (iftEndsWith(path, ".npy"))
        iftWriteIntArray(grid, path);
    else if (iftEndsWith(path, ".csv")) {
        iftCSV *csv = iftCreateCSV(grid->n, 1);
        for (int i = 0; i < grid->n; i++)
            sprintf(csv->data[i][0], "%d", grid->val[i]);
        iftWriteCSV(csv, path, ',');
        iftDestroyCSV(&csv);
    }
}

iftIntArray *_iftGeodesicGridSamplingOnMask(const iftImage *bin_mask, int radius, long n_samples) {
    // finds the first object voxel from the binary mask
    int p = 0;
    for (p = 0; p < bin_mask->n && bin_mask->val[p] == 0; p++) {}
    int first_obj_voxel = p;
    
    iftAdjRel *A = (iftIs3DImage(bin_mask)) ? iftSpheric(sqrtf(3.0)) : iftCircular(sqrtf(2.0));
    
    iftImage *prior = iftCreateImageFromImage(bin_mask);
    iftImage *label_img = iftCreateImageFromImage(bin_mask);
    iftGQueue *Qext = iftCreateGQueue(IFT_QSIZE, prior->n, prior->val);
    iftSetRemovalPolicy(Qext, MAXVALUE);
    
    iftFImage *dist = iftCreateFImage(bin_mask->xsize, bin_mask->ysize, bin_mask->zsize);
    iftFHeap *Qint = iftCreateFHeap(dist->n, dist->val);

    iftImage *root = iftCreateImageFromImage(bin_mask);
    iftImage *is_border = iftCreateImageFromImage(bin_mask); // all voxels start as false

    // initialize the costs/distances
    for (int p = 0; p < bin_mask->n; p++)
        if (bin_mask->val[p] != 0) {
            dist->val[p] = IFT_INFINITY_FLT;
            root->val[p] = p;
        }

    prior->val[first_obj_voxel] = 1;
    iftInsertGQueue(&Qext, first_obj_voxel);

    iftList *grid = iftCreateList();

    int label = 0;

    while (!iftEmptyGQueue(Qext)) {
        int po = iftRemoveGQueue(Qext);
        iftInsertListIntoTail(grid, po);

        label++;
        label_img->val[po] = label;

        dist->val[po] = 0.0;
        root->val[po] = po;
        iftInsertFHeap(Qint, po);

        iftList *ball_borders = iftCreateList();
        iftList *all_ball_voxels = iftCreateList();

        // propagates until voxels inside a sphere of radius <radius> around the voxel po
        while (!iftEmptyFHeap(Qint)) {
            int p = iftRemoveFHeap(Qint);
            iftVoxel u = iftGetVoxelCoord(bin_mask, p);
            iftInsertListIntoTail(all_ball_voxels, p);

            // don't propagate from voxels on the ball border or out of it
            if (dist->val[p] >= radius) {
                iftInsertListIntoTail(ball_borders, p);
                is_border->val[p] = true;
                continue;
            }

            for (int i = 1; i < A->n; i++) {
                iftVoxel v = iftGetAdjacentVoxel(A, u, i);

                if (iftValidVoxel(bin_mask, v)) {
                    int q = iftGetVoxelIndex(bin_mask, v);

                    if (bin_mask->val[q] != 0) {
                        // float tmp = dist->val[p] + iftSmoothEuclideanDistance(iftSquaredVoxelDistance(u, v));
                        float tmp = dist->val[p] + iftVoxelDistance(u, v);

                        if (tmp < dist->val[q]) {
                            dist->val[q] = tmp;
                            root->val[q] = root->val[p];
                            is_border->val[q] = false;
                            label_img->val[q] = label_img->val[p];

                            // q was a border of another ball, and it is inside the ball of center po
                            if (Qext->L.elem[q].color == IFT_GRAY) {
                                iftRemoveGQueueElem(Qext, q);
                                prior->val[q] = 0;
                            }

                            if (Qint->color[q] == IFT_WHITE)
                                iftInsertFHeap(Qint, q);
                            else iftGoUpFHeap(Qint, Qint->pos[q]);
                        }
                        // if q in on the border of other sphere, with root different from the current sphere of root po,
                        // and the path from po to q is
                        else if ((is_border->val[q]) && (root->val[q] != po)) {
                            root->val[q] = po; // avoid to sum up the intersection through multiple paths
                            iftInsertListIntoTail(ball_borders, q);
                        }
                    }
                }
            }
        }
        while (!iftIsEmptyList(all_ball_voxels)) {
            int r = iftRemoveListHead(all_ball_voxels);
            Qint->color[r] = IFT_WHITE;
        }
        iftDestroyList(&all_ball_voxels);


        while (!iftIsEmptyList(ball_borders)) {
            int r = iftRemoveListHead(ball_borders);

            if (Qext->L.elem[r].color == IFT_WHITE) {
                prior->val[r]++;
                iftInsertGQueue(&Qext, r);
            }
            // intersection of balls
            else if (Qext->L.elem[r].color == IFT_GRAY) {
                iftRemoveGQueueElem(Qext, r);
                prior->val[r]++;
                iftInsertGQueue(&Qext, r);
            }

        }
        iftDestroyList(&ball_borders);
    }
    iftWriteImageByExt(label_img, "label_img.nii.gz");



    iftIntArray *grid_all = iftListToIntArray(grid);
    iftDestroyList(&grid);    

    iftIntArray *grid_chosen = NULL;
    if (n_samples <= 0)
        grid_chosen = grid_all;
    else if (grid_all->n <= n_samples) {
        printf("Warning: Number of required samples %ld is <= total number of sampling voxels %ld\n" \
               "All sampling points will be considered\n", n_samples, grid_all->n);
        grid_chosen = grid_all;
    }
    else {
        grid_chosen = iftCreateIntArray(n_samples);
        iftShuffleIntArray(grid_all->val, grid_all->n);

        #pragma omp parallel for
        for (long i = 0; i < n_samples; i++)
            grid_chosen->val[i] = grid_all->val[i];
        iftDestroyIntArray(&grid_all);
    }


    iftDestroyAdjRel(&A);
    iftDestroyGQueue(&Qext);
    iftDestroyImage(&prior);
    iftDestroyFHeap(&Qint);
    iftDestroyFImage(&dist);
    iftDestroyImage(&root);
    iftDestroyImage(&is_border);


    return grid_all;
}
/*************************************************************/









