


typedef struct {
    /** Number of rows */
    int nrows;
    /** Number of columns */
    int ncols;
    /** Total number of matrix elements: nrows * ncols */
    long n;
    /** 1D array with the matrix values */
    float *val;
    /** Look-up table to speed up index access */
    long *tbrow;
    bool allocated;
} iftMatrix;

%extend iftMatrix {

	iftMatrix* __add__(iftMatrix* m2){
	    iftMatrix* m1 = ($self);
	
	    if(m1->n != m2->n){
	        SWIG_Error(12, "IftMatrices of different lengths");
	    }
	
	    return iftSumMatrices(m1, m2);
	}
	
	iftMatrix* __sub__(iftMatrix* m2){
	    iftMatrix* m1 = ($self);
	
	    if(m1->n != m2->n){
	        SWIG_Error(12, "IftMatrices of different lengths");
	    }
	
	    return iftSubtractMatrices(m1, m2);
	}
	
	iftMatrix* __mul__(iftMatrix* m2){
	    iftMatrix* m1 = ($self);
	
	    if(m1->ncols != m2->nrows){
	        SWIG_Error(12, "Number of M1 columns different from M2 rows");
	    }
	
	    return iftMultMatrices(m1, m2);
	}
	
	PyObject* AsNumPy(void)
	{
	    iftMatrix *m = ($self);
	    npy_intp dims[2] = {m->nrows, m->ncols};
	    PyArrayObject* result = PyArray_SimpleNew(2, dims, NPY_FLOAT);
	
	    float *data = array_data(result);
	    memcpy(data, m->val, m->ncols * m->nrows * sizeof (*data));
	
	    return PyArray_Return(result);
	}
	
	void FromNumPy(PyObject* input) {
	
	    PyArrayObject *ary = obj_to_array_no_conversion(input, NPY_FLOAT32);
	
	    if (ary == NULL) return;
	
	    if (!require_contiguous(ary))
	        SWIG_Error(0, "Input numpy array is not contiguous");
	
	    iftMatrix *m = ($self);
	    float *ptr = array_data(ary);
	
	    int n_dims = array_numdims(ary);
	    if (n_dims != 2)
	        SWIG_Error(0, "Input is not a 2D array");
	
	    int ary_xsize = array_size(ary, 1);
	    int ary_ysize = array_size(ary, 0);
	
	    if (ary_ysize != m->nrows || ary_xsize == m->ncols || m->allocated == false)
	    {
	        m->nrows = ary_ysize;
	        m->ncols = ary_xsize;
	        m->n = ary_xsize * ary_ysize;
	        m->val = iftRealloc(m->val, m->n * sizeof *m->val);
	        if (m->val == NULL)
	            iftError(MSG_MEMORY_ALLOC_ERROR, "FromNumpy");
	    }
	    memcpy(m->val, ptr, m->n * sizeof *m->val);
	}
	
	

	~iftMatrix() {
		iftMatrix* ptr = ($self);
		iftDestroyMatrix(&ptr);
	}
};

%newobject iftCreateMatrix;
%feature("autodoc", "2");
iftMatrix *iftCreateMatrix(int ncols, int nrows);

%feature("autodoc", "2");
void iftPrintMatrix(const iftMatrix *M);

