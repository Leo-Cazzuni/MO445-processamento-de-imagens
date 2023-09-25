
%typemap(in) iftImage* {
    
    if(PyArray_TYPE($input) != NPY_INT){
        SWIG_exception_fail(12, "Input must be a numpy int array");
    }
    
    iftImage* img = (iftImage*) iftAlloc(1, sizeof(iftImage));
    
    img->xsize = PyArray_DIM($input, 0);
    img->ysize = PyArray_DIM($input, 1);
    img->zsize = PyArray_DIM($input, 2);
    img->n     = img->xsize * img->zsize * img->ysize;
    img->tby   = iftAllocIntArray(img->ysize);
    img->tbz   = iftAllocIntArray(img->zsize);
    img->dx = img->dy = img->dz = 1;

    if(PyArray_DIM($input, 3) == 3){
        
        img->val = (int*) iftAlloc(img->n, sizeof(int));
        img->Cb = (ushort*) iftAlloc(img->n, sizeof(ushort));
        img->Cr = (ushort*) iftAlloc(img->n, sizeof(ushort));
    
        int* data = (int*) PyArray_DATA($input);
        for(int i = 0; i < img->n; i++){
            img->val[i] = (int) data[i];
            img->Cb[i]  = (ushort) data[i + img->n];
            img->Cr[i]  = (ushort) data[i + img->n * 2];
        }
    
    } else if(PyArray_DIM($input, 3) == 1){
        
        img->val = (int*) iftAlloc(img->n, sizeof(int));
        img->Cb = NULL;
        img->Cr = NULL;
        
        int* data = (int*) PyArray_DATA($input);
        memcpy(img->val, data, img->n * sizeof(int));
        
    }

    $1 = img;
}

%typemap(out) iftImage* {
    
    if($1->Cb == NULL && $1->Cr == NULL){
        
        npy_intp dims[4] = {$1->xsize, $1->ysize, $1->zsize, 1};
        $result = PyArray_SimpleNewFromData(4, dims, NPY_INT, (int*) $1->val);
    
    } else {
        
        npy_intp dims[4] = {$1->xsize, $1->ysize, $1->zsize, 3};
        $result = PyArray_SimpleNew(4, dims, NPY_INT);
    
        int* p = (int*) PyArray_DATA($result);
        int* q = iftAlloc($1->n * 3, sizeof(int));
    
        for(int i = 0; i < $1->n; i++){
            q[i]            = (int) $1->val[i];
            q[i + $1->n]    = (int) $1->Cb[i];
            q[i + $1->n * 2]= (int) $1->Cr[i];
        }

        memcpy(p, q, sizeof(int) * $1->n * 3);
        iftFree(q);
    }
}

%typemap(freearg) iftImage* {
    if($1) iftFree($1);
}


%typemap(in) iftFImage* {
    
    if(PyArray_TYPE($input) != NPY_FLOAT){
        SWIG_exception_fail(12, "Input must be a numpy float array");
    }
    
    iftFImage* img = (iftFImage*) iftAlloc(1, sizeof(iftFImage));
    
    img->xsize = PyArray_DIM($input, 0);
    img->ysize = PyArray_DIM($input, 1);
    img->zsize = PyArray_DIM($input, 2);
    img->n     = img->xsize * img->zsize * img->ysize;
    img->tby   = iftAllocIntArray(img->ysize);
    img->tbz   = iftAllocIntArray(img->zsize);
    img->dx = img->dy = img->dz = 1;

    img->val = iftAllocFloatArray(img->n);
    float* data = (float*) PyArray_DATA($input);
    memcpy(img->val, data, img->n * sizeof(float));

    $1 = img;
}

%typemap(out) iftFImage* {
    
    npy_intp dims[4] = {$1->xsize, $1->ysize, $1->zsize, 1};
    $result = PyArray_SimpleNewFromData(4, dims, NPY_FLOAT, (float*) $1->val);
}

%typemap(freearg) iftFImage* {
    if($1) iftFree($1);
}


%typemap(in) iftHistogram* {
    if(PyArray_TYPE($input) != NPY_INT){
        SWIG_exception_fail(12, "Input must be a numpy int array");
    }

    iftHistogram* hist = (iftHistogram*) iftAlloc(1, sizeof(iftHistogram));

    hist->nbins = PyArray_DIM($input, 0);
    hist->val = iftAlloc(hist->nbins, sizeof(double));

    double* data = (double*) PyArray_DATA($input);
    memcpy(hist->val, data, hist->nbins * sizeof(double));

    $1 = hist;
}

%typemap(out) iftHistogram*{

    npy_intp dims[1] = {$1->nbins};
    $result = PyArray_SimpleNewFromData(1, dims, NPY_DOUBLE, (double*) $1->val);
}

%typemap(freearg) iftHistogram*{
    if($1) iftFree($1);
}


int iftValidVoxel(iftImage* img, iftVoxel v){  
    return ((v.x >= 0) && (v.x < img->xsize) && (v.y >= 0) && (v.y < img->ysize) && (v.z >= 0) && (v.z < img->zsize))
}


%typemap(in) iftVoxel {
    iftVoxel* p = iftAlloc(1, sizeof(iftVoxel));
    int* data = (int*) PyArray_DATA($input);
    
    p->x = data[0];
    p->y = data[1];
    p->z = data[2];

    memcpy(p, &$1, sizeof(iftVoxel));
    free(p);
}

%typemap(out) iftVoxel* {
    
    npy_intp dims[1] = {3};
    $result = PyArray_SimpleNew(1, dims, NPY_INT);
    
    int *p = (int*) PyArray_DATA($result);
    int *q = iftAlloc(3, sizeof(int));

    q[0] = (int) $1->x;
    q[1] = (int) $1->y;
    q[2] = (int) $1->z;
    memcpy(q, p, 3 * sizeof(int));
    free(q);
}   

%typemap(freearg) iftVoxel* {
    if($1) iftFree($1);
}
