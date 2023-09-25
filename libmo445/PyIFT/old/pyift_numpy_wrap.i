%module pyift

%{
#define SWIG_FILE_WITH_INIT
#include "ift.h"
%}

#ifndef PYIFT_DEBUG
#define PYIFT_DEBUG
#endif

%include "numpy.i"
%fragment("NumPy_Fragments");

%init %{
    import_array();
%}

%rename("%(strip:[ift])s") "";

#include <string.h>
#include <Python.h>


%fragment("PyUnicodeToString", "header") {
    char* PyUnicodeToString(PyObject *obj){
        PyObject *temp_bytes = PyUnicode_AsEncodedString(obj, "ASCII", "strict");
        return PyBytes_AS_STRING(temp_bytes);
    }
}

%typemap(in, fragment="PyUnicodeToString") (const char *filename) {
    $1 = PyUnicodeToString($input);
};

%typemap(in, fragment="PyUnicodeToString") (const char *filepath) {
    $1 = PyUnicodeToString($input);
};

%typemap(in, fragment="PyUnicodeToString") (const char *format) {
    $1 = PyUnicodeToString($input);
};


typedef struct {
    int *val;
    ushort *Cb;
    ushort *Cr;
    ushort *alpha;
    int xsize;
    int ysize;
    int zsize;
    float dx;
    float dy;
    float dz;
    int *tby, *tbz;
    int n;
    iftNIfTIHeader *nii_hdr;
} iftImage;

%extend iftImage{
    ~iftImage(){
        iftImage* ptr = ($self);
        iftDestroyImage(&ptr);
        #ifdef PYIFT_DEBUG
            printf("iftImage Deleted\n");
        #endif
    }
}

%typemap(in) iftImage* {
    
    if(PyArray_TYPE($input) != NPY_INT32){
        SWIG_exception_fail(12, "Input must be a numpy int array");
    }
    
    iftImage* img = (iftImage*) iftAlloc(1, sizeof(iftImage));
    
    img->xsize = PyArray_DIM($input, 3);
    img->ysize = PyArray_DIM($input, 2);
    img->zsize = PyArray_DIM($input, 1);
    img->n     = img->xsize * img->zsize * img->ysize;
    img->tby   = iftAllocIntArray(img->ysize);
    img->tbz   = iftAllocIntArray(img->zsize);
    img->dx = img->dy = img->dz = 0;

    if(PyArray_DIM($input, 0) == 3){
        
        img->val = (int*) iftAlloc(img->n, sizeof(int));
        img->Cb = (ushort*) iftAlloc(img->n, sizeof(ushort));
        img->Cr = (ushort*) iftAlloc(img->n, sizeof(ushort));
    
        int* data = (int*) PyArray_DATA($input);
        for(int i = 0; i < img->n; i++){
            img->val[i] = (int) data[i];
            img->Cb[i]  = (ushort) data[i + img->n];
            img->Cr[i]  = (ushort) data[i + img->n * 2];
        }
    
    } else if(PyArray_DIM($input, 0) == 1){
        
        img->val = (int*) iftAlloc(img->n, sizeof(int));
        img->Cb = NULL;
        img->Cr = NULL;
        
        int* data = (int*) PyArray_DATA($input);
        memcpy(img->val, data, img->n * sizeof(int));
        
    } else {
        SWIG_exception_fail(12, "Input must be array be a matrix ou hipermatrix with 3 or 1 dimension");
    }

    $1 = img;
}

%typemap(out) iftImage* {
    
    if($1->Cb == NULL && $1->Cr == NULL){
        
        npy_intp dims[4] = {1, $1->zsize, $1->ysize, $1->xsize};
        $result = PyArray_SimpleNewFromData(4, dims, NPY_INT32, (int*) $1->val);
    
    } else {
        
        npy_intp dims[4] = {3, $1->zsize, $1->ysize, $1->xsize};
        $result = PyArray_SimpleNew(4, dims, NPY_INT32);
    
        int* p = (int*) PyArray_DATA($result);
        for(int i = 0; i < $1->n; i++){
            p[i]            = (int) $1->val[i];
            p[i + $1->n]    = (int) $1->Cb[i];
            p[i + $1->n * 2]= (int) $1->Cr[i];
        }
    }
}

%typemap(freearg) iftImage* {
    if($1) iftFree($1);
}


typedef struct {
  float *val;
  int    xsize,ysize,zsize;
  float  dx,dy,dz;
  int   *tby, *tbz;
  int    n;
} iftFImage;

%extend iftFImage{
    ~iftFImage(){
        iftFImage* ptr = ($self);
        iftDestroyFImage(&ptr);
        #ifdef PYIFT_DEBUG
            printf("iftFImage Deleted\n");
        #endif
    }
};

%typemap(in) iftFImage* {
    
    if(PyArray_TYPE($input) != NPY_FLOAT){
        SWIG_exception_fail(12, "Input must be a numpy float array");
    }

    iftFImage* img = (iftFImage*) iftAlloc(1, sizeof(iftFImage));
    
    img->xsize = PyArray_DIM($input, 3);
    img->ysize = PyArray_DIM($input, 2);
    img->zsize = PyArray_DIM($input, 1);
    img->n     = img->xsize * img->zsize * img->ysize;
    img->tby   = iftAllocIntArray(img->ysize);
    img->tbz   = iftAllocIntArray(img->zsize);
    img->dx = img->dy = img->dz = 0;

    img->val = iftAllocFloatArray(img->n);
    float* data = (float*) PyArray_DATA($input);
    memcpy(img->val, data, img->n * sizeof(float));

    $1 = img;
}

%typemap(out) iftFImage* {
    
    npy_intp dims[4] = {1, $1->zsize, $1->ysize, $1->xsize};
    $result = PyArray_SimpleNewFromData(4, dims, NPY_FLOAT, (float*) $1->val);
}

%typemap(freearg) iftFImage* {
    if($1) iftFree($1);
}

typedef struct {
  double *val;
  int nbins;
} iftHistogram;

%extend iftHistogram{
    ~iftHistogram(){
        iftHistogram* ptr = ($self);
        iftDestroyHistogram(&ptr);
        #ifdef PYIFT_DEBUG
            printf("iftHistogram Deleted\n");
        #endif
    }
};

%typemap(in) iftHistogram* {
    
    if(PyArray_TYPE($input) != NPY_INT32){
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

typedef struct {
  int elem;
  int label;
  int marker;
  int handicap;
  struct ift_labeledset *next;
} iftLabeledSet;

%extend iftLabeledSet{
    ~iftLabeledSet(){
        iftLabeledSet* ptr = ($self);
        iftDestroyLabeledSet(&ptr);
        #ifdef PYIFT_DEBUG
            printf("iftLabeledSet Deleted\n");
        #endif
    }

    iftLabeledSet* __add__(iftLabeledSet* lset){
        iftLabeledSet *copy = iftCopyOrderedLabeledSet(($self));
        iftConcatLabeledSet(&copy,&lset);
        return copy;
    }

    iftLabeledSet* __sub__(iftLabeledSet* lset){
        iftLabeledSet *copy = iftCopyOrderedLabeledSet(($self));
        iftRemoveSubsetLabeledSet(&copy,&lset);
        return copy;
    }

    PyObject* to_dict(){
        iftLabeledSet* s = ($self);

        PyObject *dict = PyDict_New();
        while(s != NULL){
            PyDict_SetItem(dict, PyInt_FromLong(s->elem), PyInt_FromLong(s->label));
            s = s->next;
        }

        return dict;
    }

    void print_seeds(){
        iftLabeledSet *ptr = ($self);

        while(ptr != NULL){
            printf("%d,%d\n",ptr->elem,ptr->label);
            ptr = ptr->next;
        }
    }

};

typedef struct {
  int elem;
  struct ift_set *next;
} iftSet;

%extend iftSet{
    ~iftSet(){
        iftSet *ptr = ($self);

        iftDestroySet(&ptr);
        #ifdef PYIFT_DEBUG
            printf("iftSet Deleted\n");
        #endif
    }

    PyObject* to_list(){
        iftSet* s = ($self);

        PyObject *list = PyList_New(iftSetSize(s));
        int i = 0;
        while(s != NULL){
            PyList_SetItem(list, i, PyInt_FromLong(s->elem));
            s = s->next;
            i++;
        }

        return list;
    }
};

typedef struct {
    int *dx, *dy, *dz;
    int n;
} iftAdjRel;

%extend iftAdjRel{
    PyObject* __getitem__(int i){
        PyObject *displacement = PyTuple_New(3);

        PyTuple_SetItem(displacement,0, PyInt_FromLong(($self)->dx[i]));
        PyTuple_SetItem(displacement,1, PyInt_FromLong(($self)->dy[i]));
        PyTuple_SetItem(displacement,2, PyInt_FromLong(($self)->dz[i]));

        return displacement;
    }

    ~iftAdjRel(){
        iftAdjRel *ptr = ($self);
        iftDestroyAdjRel(&ptr);

        #ifdef PYIFT_DEBUG
            printf("iftAdjRel deleted\n");
        #endif
    }
}

// struct ift_voxel {
//     int x, y, z;
// };
// 
// %extend ift_voxel {
//     char *__str__() {
//         static char tmp[1024];
//         sprintf(tmp, "[%d, %d, %d]", $self->x, $self->y, $self->z);
//         return tmp;
//     }
//     ift_voxel(int x, int y, int z){
//         iftVoxel *v = (iftVoxel *) iftAlloc(1, sizeof(iftVoxel));
//         v->x = x;
//         v->y = y;
//         v->z = z;
//         return v;
//     }
// };

// IFTVOXEL AS LIST, KIND OF OBJECT ORIENTED
/*
%typemap(in) iftVoxel* {
    if(!PyList_Check($input)){
        int res = SWIG_ConvertPtr($input, (void**) &$1, $1_descriptor, 0);
        if(!SWIG_IsOK(res))
                SWIG_exception_fail(12, "Error in iftVoxel typemap");
    } else {
        size_t size = (size_t) PyList_Size($input);
        printf("Size %d\n", size);
        iftVoxel* v = iftAlloc(size, sizeof(iftVoxel));
        for (int i = 0; i < size; ++i){
            void*    up = 0;
            iftVoxel* u;
            PyObject* o = PyList_GetItem($input, i);
            int res = SWIG_ConvertPtr(o, &up, $1_descriptor, 0);
            // printf("iftVoid %d, SWIG %d\n", sizeof(iftVoxel*), sizeof($1_ltype));
            if(!SWIG_IsOK(res))
                SWIG_exception_fail(12, "Error in iftVoxel typemap");
            else u = (iftVoxel*) up;
            printf("x%d: %d\n", i, u->x);
            printf("y%d: %d\n", i, u->y);
            printf("z%d: %d\n", i, u->z);
            v[i].x = u->x;
            v[i].y = u->x;
            v[i].z = u->z;
        }
        $1 = v;
    }
}
*/

// IFTVOXEL AS NUMPY ARRAY

typedef struct {
    int x, y, z;
} iftVoxel, iftSize;

%typemap(in) iftVoxel* {
    
    if(PyArray_TYPE($input) != NPY_INT32 && PyArray_TYPE($input) != NPY_INT64){
        SWIG_exception_fail(12, "Input must be a numpy int array");
    }

    void* data = PyArray_DATA($input);
    
    if(PyArray_DIM($input, 0) == 3){
    
        iftVoxel* v = (iftVoxel*) iftAlloc(1, sizeof(iftVoxel));
        
        if(PyArray_TYPE($input) == NPY_INT32){
            void* data = PyArray_DATA($input);
            memcpy(v, data, 3 * sizeof(int));
        
        } else {
            int64_t* longPtr = data;
            v->x = (int) longPtr[0];
            v->y = (int) longPtr[1];
            v->z = (int) longPtr[2];
            // int64_t* x = data;
            // int64_t* y = data + PyArray_ITEMSIZE($input);
            // int64_t* z = data + 2 * PyArray_ITEMSIZE($input);
            // printf("0D iftVoxel\n");
            // printf("{%d, %d, %d}\n", *x, *y, *z);
            // printf("(%d, %d, %d)\n", v->x, v->y, v->z);
        }
        
        $1 = v;
    
    } else if(PyArray_DIM($input, 1) == 3){
        
        int n = PyArray_DIM($input, 0);
        iftVoxel* v = (iftVoxel*) iftAlloc(n, sizeof(iftVoxel));
        
        if(PyArray_TYPE($input) == NPY_INT32){
            data = (int*) PyArray_DATA($input);
            memcpy(v, data, n * 3 * sizeof(int));
            
            // printf("Memcpy iftVoxel\n");   
            // for(int i = 0; i < n; i++){
            //     int* x = (int*) PyArray_GETPTR2($input, i, 0);
            //     int* y = (int*) PyArray_GETPTR2($input, i, 1);
            //     int* z = (int*) PyArray_GETPTR2($input, i, 2);
            //     printf("{%d, %d, %d}\n", *x, *y, *z);
            // }

            // for(int i = 0; i < n; i++){
            //     printf("(%d, %d, %d)\n", v[i].x, v[i].y, v[i].z);
            // }
        } else {

            for(int i = 0; i < n; i++){
                int64_t* x = (int64_t*) PyArray_GETPTR2($input, i, 0);
                int64_t* y = (int64_t*) PyArray_GETPTR2($input, i, 1);
                int64_t* z = (int64_t*) PyArray_GETPTR2($input, i, 2);
                v[i].x = (int) *x;
                v[i].y = (int) *y;
                v[i].z = (int) *z;
            }
        }

        $1 = v;
        
        // // https://stackoverflow.com/questions/15576136/passing-3-dimensional-numpy-array-to-c

        // printf("Array created by PyArray_GETPTR\n");
        // for(int i = 0; i < n; i++){
        //     int* x = (int*) PyArray_GETPTR2($input, i, 0);
        //     int* y = (int*) PyArray_GETPTR2($input, i, 1);
        //     int* z = (int*) PyArray_GETPTR2($input, i, 2);
        //     printf("{%d, %d, %d}\n", *x, *y, *z);
        // }

        // printf("Array of iftVoxels\n");
        // for(int i = 0; i < n; i++){
        //     printf("(%d, %d, %d)\n", v[i].x, v[i].y, v[i].z);
        // }

    } else {
        SWIG_exception_fail(12, "One of the array dimensions must be 3");

    }
}

%typemap(out) iftVoxel* {
    // works only for one voxel, doesnt work for a array of voxels
    npy_intp dims[1] = {3};
    $result = PyArray_SimpleNew(1, dims, NPY_INT32);
    int* p = (int*) PyArray_DATA($result);
    memcpy(p, $1, sizeof(int) * 3);
}

%typemap(freearg) iftVoxel* {
    if($1) iftFree($1);
}

%typemap(out) iftVoxel { /* Not iftVoxel pointer */
    iftVoxel* v = &$1;
    npy_intp dims[1] = {3};
    $result = PyArray_SimpleNew(1, dims, NPY_INT32);
    int* p = (int*) PyArray_DATA($result);
    memcpy(p, v, sizeof(int) * 3);
}

// ##### FUNCTION DECLARATION ##### Dont delete this line

%newobject VoxelNew;
iftVoxel* VoxelNew();
void VoxelPrint(iftVoxel* v);
void VoxelDelete(iftVoxel* v);
void VoxelSetX(iftVoxel* v, int i);
void VoxelSetY(iftVoxel* v, int i);
void VoxelSetZ(iftVoxel* v, int i);
int VoxelGetX(iftVoxel* v);
int VoxelGetY(iftVoxel* v);
int VoxelGetZ(iftVoxel* v);
int ValidVoxel(iftImage* img, iftVoxel* v);

%newobject iftFCopyImage;
iftFImage *iftFCopyImage(const iftFImage *img);

%newobject iftCreateFImage;
iftFImage  *iftCreateFImage(int xsize,int ysize,int zsize);

%newobject iftImageToFImage;
iftFImage  *iftImageToFImage(const iftImage *img);

%newobject iftFImageToImage;
iftImage   *iftFImageToImage(const iftFImage *img, int Imax);

%newobject iftFReadImage;
iftFImage  *iftFReadImage(const char *filename);
void iftFWriteImage(iftFImage *img, const char *filename, ...);

%newobject iftCreateHistogram;
iftHistogram *iftCreateHistogram(int nbins);

%newobject iftReadHistogram;
iftHistogram *iftReadHistogram(char *filename);
void          iftWriteHistogram(iftHistogram *hist, char *filename);

%newobject iftGrayHistogram;
iftHistogram *iftGrayHistogram(const iftImage *img, int nbins, bool normalize);

%newobject iftReadImageByExt;
iftImage *iftReadImageByExt(const char *filename, ...);
void iftWriteImageByExt(const iftImage *img, const char *filename, ...);

%newobject iftIntegralImage;
iftFImage      *iftIntegralImage(iftImage *img);

%newobject iftGetIntegralValueInRegion;
float           iftGetIntegralValueInRegion(iftFImage *integ, iftVoxel *v, int npts);

%newobject iftReadSeeds;
iftLabeledSet *iftReadSeeds(char *filename, iftImage *img);

%newobject iftReadSeedsComplete;
iftLabeledSet *iftReadSeedsComplete(const char *filename, const iftImage *img);

%newobject iftReadSeeds2D;
iftLabeledSet *iftReadSeeds2D(const char *filename, const iftImage *img);

%newobject iftExtractRemovalMarkers;
iftSet        *iftExtractRemovalMarkers(iftLabeledSet **s);

%newobject iftLabelObjBorderSet;
iftLabeledSet *iftLabelObjBorderSet(iftImage *bin, iftAdjRel *A);

%newobject iftImageBorderLabeledSet;
iftLabeledSet *iftImageBorderLabeledSet(iftImage *img);

%newobject iftLabelCompSet;
iftLabeledSet *iftLabelCompSet(iftImage *bin, iftAdjRel *A);

%newobject iftFuzzyModelToLabeledSet;
iftLabeledSet *iftFuzzyModelToLabeledSet(iftImage *model);

%newobject iftMAdjustSeedCoordinates;
iftLabeledSet *iftMAdjustSeedCoordinates(iftLabeledSet *S, iftMImage *input, iftMImage *output);

%newobject iftAdjustSeedCoordinates;
iftLabeledSet *iftAdjustSeedCoordinates(iftLabeledSet *Sin, iftImage *orig, iftMImage *output);

%newobject iftImageBorderSet;
iftSet        *iftImageBorderSet(iftImage *img);

%newobject iftMultiObjectBorderLabeledSet;
iftLabeledSet *iftMultiObjectBorderLabeledSet(iftImage *img, iftAdjRel *A);

%newobject iftLabeledBorders;
iftImage *iftLabeledBorders(iftImage *label, iftAdjRel *A);

%newobject iftCreateAdjRel;
iftAdjRel *iftCreateAdjRel(int n);

%newobject iftSpheric;
iftAdjRel *iftSpheric(float r);

%newobject iftHemispheric;
iftAdjRel *iftHemispheric(float r, char axis, int direction);

%newobject iftSphericEdges;
iftAdjRel *iftSphericEdges(float r);

%newobject iftCircular;
iftAdjRel *iftCircular(float r);

%newobject iftCircularEdges;
iftAdjRel *iftCircularEdges(float r);

%newobject iftClockCircular;
iftAdjRel *iftClockCircular(float r);

%newobject iftRightSide;
iftAdjRel *iftRightSide(iftAdjRel *A, float r);

%newobject iftLeftSide;
iftAdjRel *iftLeftSide(iftAdjRel *A, float r);

%newobject iftRectangular;
iftAdjRel *iftRectangular(int xsize, int ysize);

%newobject iftCuboid;
iftAdjRel *iftCuboid(int xsize, int ysize, int zsize);

%newobject iftCopyAdjacency;
iftAdjRel *iftCopyAdjacency(iftAdjRel *A);

void iftMaxAdjShifts(const iftAdjRel *A, int *dx, int *dy, int *dz);
void iftWriteAdjRel(iftAdjRel *A, char *filename);

%newobject iftReadAdjRel;
iftAdjRel *iftReadAdjRel(char *filename);

%newobject iftReadAdjRelBinFile;
iftAdjRel *iftReadAdjRelBinFile(const char *path);
void iftWriteAdjRelBinFile(const iftAdjRel *A, const char *path);

%newobject iftGetAdjacentVoxel;
iftVoxel iftGetAdjacentVoxel(const iftAdjRel *A, iftVoxel u, int adj);

int iftIsAdjRel3D(iftAdjRel *A);
