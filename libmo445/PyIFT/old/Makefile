FLAGS= -fPIC -O2 -std=gnu11 -w -Wno-unused-result -pedantic -Wall 
PYCFLAGS= `python3-config --cflags` $(FLAGS)
PYLDFLAGS= `python3-config --ldflags` $(FLAGS)

BIN=$(NEWIFT_DIR)/bin
BUILD=$(NEWIFT_DIR)/build
DEMO=$(NEWIFT_DIR)/demo
DOCS=$(NEWIFT_DIR)/docs
INCLUDE=$(NEWIFT_DIR)/include
LIB=$(NEWIFT_DIR)/lib
OBJ=$(NEWIFT_DIR)/obj
SRC=$(NEWIFT_DIR)/src
TEST=$(NEWIFT_DIR)/test

BIN = $(NEWIFT_DIR)/bin

LIBSVM_DIR = $(NEWIFT_DIR)/externals/libsvm
LIBNIFTI_DIR= $(NEWIFT_DIR)/externals/libnifti
LIBJPEG_DIR= $(NEWIFT_DIR)/externals/libjpeg
LIBPNG_DIR= $(NEWIFT_DIR)/externals/libpng
ZLIB_DIR= $(NEWIFT_DIR)/externals/zlib
TSNE_DIR = $(NEWIFT_DIR)/externals/tsne

NUMPY_INC    = -I `python3 -c "import numpy; print(numpy.get_include())"` 

LIBIFT_INC   = -I $(NEWIFT_DIR)/include
LIBIFT_LD    = -L $(NEWIFT_DIR)/lib -lift
LIBSVM_INC   = -I $(LIBSVM_DIR)/include 
LIBSVM_LD    = -L $(LIBSVM_DIR)/lib -lsvm -lstdc++
LIBCBLAS_INC = -I /usr/local/opt/openblas/include
LIBCBLAS_LD  = -L /usr/local/opt/openblas/lib -L /lib64/atlas-sse3 -L /usr/lib/atlas-base -llapack -lblas -lcblas

LIBNIFTI_INC = -I $(LIBNIFTI_DIR)/include
LIBJPEG_INC = -I $(LIBJPEG_DIR)/include
LIBPNG_INC = -I $(LIBPNG_DIR)/include
ZLIB_INC = -I $(ZLIB_DIR)/include
TSNE_INC = -I $(TSNE_DIR)/include

EXTERNALS_LD = -fopenmp -lm 

INCLUDES = $(LIBIFT_INC) $(LIBSVM_INC) $(LIBCBLAS_INC) $(LIBNIFTI_INC) $(LIBJPEG_INC) $(LIBPNG_INC) $(ZLIB_INC) $(NUMPY_INC) $(TSNE_INC)
LIBS     = $(LIBIFT_LD) $(LIBSVM_LD) $(LIBCBLAS_LD) $(EXTERNALS_LD)

all: libift swig wrap shared

libift:
	cd $(NEWIFT_DIR); $(MAKE); cd -;

swig:
	swig -python -py3 pyift.i
	# swig -python -py3 -debug-tmsearch pyift.i

wrap:
	$(CC) $(PYCFLAGS) $(INCLUDES) -c pyift_wrap.c 

shared:
	$(CC) $(PYLDFLAGS) -shared -o _pyift.so pyift_wrap.o $(LIBS)

clean:
	rm -rf pyift.py pyift_wrap.o pyift_wrap.c _pyift.so __pycache__
