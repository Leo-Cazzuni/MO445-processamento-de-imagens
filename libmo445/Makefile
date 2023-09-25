BIN=./bin
BUILD=./build
DEMO=./demo
DOCS=./docs
INCLUDE=./include
LIB=./lib
OBJ=./obj
SRC=./src
TEST=./test

# external libs
EXTERNALS_DIR = ./externals
LIBNIFTI=$(EXTERNALS_DIR)/libnifti
LIBPNG=$(EXTERNALS_DIR)/libpng
LIBSVM=$(EXTERNALS_DIR)/libsvm
LIBTIFF=$(EXTERNALS_DIR)/libtiff
TSNE = $(EXTERNALS_DIR)/tsne
ZLIB=$(EXTERNALS_DIR)/zlib
TSNE=$(EXTERNALS_DIR)/tsne
LIBJPEG=$(EXTERNALS_DIR)/libjpeg

NVCC = /usr/local/cuda/bin/nvcc #nvcc

#FOR GPU support uncomment the next line (in all the Makefiles that will use GPU)
#IFT_GPU=1

#FOR Debug mode uncomment the next line
#IFT_DEBUG=1

#FOR For python error handling uncomment the next line
#IFT_SWIG=1

#CUDA path in case IFT_GPU is enabled
export CUDA_DIR1=/usr/local/cuda
export CUDA_DIR2=/opt/cuda

#OTHERS
FLAGS=  -Wall -fPIC -std=gnu11 -pedantic

LIBJPEG_INCLUDES    = -I $(LIBJPEG)/include
LIBNIFTI_INCLUDES   = -I $(LIBNIFTI)/include
LIBPNG_INCLUDES     = -I $(LIBPNG)/include
LIBSVM_INCLUDES     = -I $(LIBSVM)/include
TSNE_INCLUDES 	    = -I $(TSNE)/include
ZLIB_INCLUDES	    = -I $(ZLIB)/include
#LIBTIFF_INCLUDES    = -I $(LIBTIFF)/include

INCLUDES = -I$(INCLUDE) $(LIBJPEG_INCLUDES) $(LIBNIFTI_INCLUDES) $(ZLIB_INCLUDES) $(LIBPNG_INCLUDES) $(LIBSVM_INCLUDES) $(TSNE_INCLUDES)


# including flag for Python Error Handling
ifeq ($(IFT_SWIG), 1)
	FLAGS += -DSWIGPYTHON	
	PYTHON_LIB = $(shell python-config --includes --libs 2>/dev/null)
	# including the Python Dev Library
	ifdef PYTHON_LIB
		FLAGS += $(PYTHON_LIB)
	endif
endif


ifeq ($(IFT_DEBUG), 1)
    export FLAGS += -pg -g -fsanitize=address -fsanitize=leak -DIFT_DEBUG=1
else
    export FLAGS += -O3
endif

OS=$(shell uname -s)


# For Mac OS, append the openblas dir, and add a special flag to enable the openmp.  Install: brew install libomp
# Otherwise, add the openmp flag commonly 
ifeq ($(OS), Darwin)
	INCLUDES += -I/usr/local/opt/openblas/include
	FLAGS += -Xpreprocessor -fopenmp
else
	FLAGS += -fopenmp
endif

ifeq ($(OS), Windows_NT)
INCLUDES=-I$(INCLUDE) $(NEWIFTFLAGS)
# Considering that IFT will be compiled using 32/64-bit MinGW. Also, blas and lapack are provided inside IFT's directory
ifeq (${WIN32},yes)
	export CC=x86_64-w64-mingw32-gcc -m32
else
	export CC=x86_64-w64-mingw32-gcc
endif
endif

libs = libift

$(info $$libs is [${libs}])

ifeq ($(IFT_GPU), 1)
    export FLAGS += -DIFT_GPU=1
	INCLUDES += -I$(CUDA_DIR1)/include
	INCLUDES += -I$(CUDA_DIR2)/include
	libs += libiftcuda
	#export CC=gcc
endif


all: external_libs ${libs}
	$(eval ALL_OBJS := $(wildcard $(OBJ)/*.o))
	$(eval ALL_OBJS := $(ALL_OBJS) $(wildcard $(OBJ)/ift/**/*.o))
	$(eval ALL_OBJS := $(ALL_OBJS) $(wildcard $(OBJ)/ift/**/**/*.o))
	ar csr $(LIB)/libift.a $(ALL_OBJS) $(LIBJPEG)/obj/*.o $(LIBPNG)/obj/*.o $(LIBSVM)/obj/*.o $(TSNE)/obj/*.o $(ZLIB)/obj/*.o 
	@echo
	@echo "libift.a built..."
	@echo "DONE."


########### EXTERNAL LIBS ###########

libpng: FORCE
	cd $(LIBPNG); $(MAKE) ; cd -\

libsvm: FORCE
	cd $(LIBSVM); $(MAKE) -C .; cd -\

libtiff: FORCE
	cd $(LIBTIFF); $(MAKE) ; cd -\

tsne: FORCE
	cd $(TSNE); $(MAKE) ; cd -\

libjpeg: FORCE
	cd $(LIBJPEG); $(MAKE) ; cd -\

#force to enter and compile external libs
FORCE:

external_libs: libjpeg libsvm libpng tsne

libift: \
$(OBJ)/iftMSPS.o \
$(OBJ)/iftSaliency.o \
$(OBJ)/iftSaliencyPriors.o \
$(OBJ)/iftFLIM.o \
$(OBJ)/iftMemory.o \
$(OBJ)/iftCommon.o \
$(OBJ)/iftGenericLinkedList.o \
$(OBJ)/iftGenericVector.o \
$(OBJ)/iftGenericMatrix.o \
$(OBJ)/iftArgumentList.o \
$(OBJ)/iftDistanceFunctions.o \
$(OBJ)/iftSort.o \
$(OBJ)/iftCurve.o \
$(OBJ)/iftPlane.o \
$(OBJ)/iftGraphics.o \
$(OBJ)/iftMatrix.o \
$(OBJ)/iftTensor.o \
$(OBJ)/iftInterpolation.o \
$(OBJ)/iftAdjacency.o \
$(OBJ)/iftImage.o \
$(OBJ)/iftFImage.o \
$(OBJ)/iftMImage.o \
$(OBJ)/iftImageMath.o \
$(OBJ)/iftImageForest.o \
$(OBJ)/iftFImageForest.o \
$(OBJ)/iftDataSet.o \
$(OBJ)/iftMetrics.o \
$(OBJ)/iftDescriptors.o \
$(OBJ)/iftClustering.o \
$(OBJ)/iftClassification.o \
$(OBJ)/iftActiveLearning.o \
$(OBJ)/iftMST.o \
$(OBJ)/iftCompTree.o \
$(OBJ)/iftSeeds.o \
$(OBJ)/iftAdjSet.o \
$(OBJ)/iftRadiometric.o \
$(OBJ)/iftMathMorph.o \
$(OBJ)/iftSegmentation.o \
$(OBJ)/iftRepresentation.o \
$(OBJ)/iftKernel.o \
$(OBJ)/iftFiltering.o \
$(OBJ)/iftDeepLearning.o \
$(OBJ)/iftGeometric.o \
$(OBJ)/iftSVM.o \
$(OBJ)/iftRegion.o \
$(OBJ)/iftReconstruction.o \
$(OBJ)/iftBrainAffineRegistration.o \
$(OBJ)/iftKmeans.o \
$(OBJ)/iftSlic.o \
$(OBJ)/iftDisjointSet.o \
$(OBJ)/iftRegistration.o \
$(OBJ)/iftSimilarity.o \
$(OBJ)/iftHierarchicCluster.o \
$(OBJ)/iftInpainting.o \
$(OBJ)/iftVideo.o \
$(OBJ)/iftIGraph.o \
$(OBJ)/iftDicom.o \
$(OBJ)/iftBagOfFeatures.o \
$(OBJ)/iftBagOfVisualWords.o \
$(OBJ)/iftFunctions.o \
$(OBJ)/iftCompression.o \
$(OBJ)/iftParamOptimizer.o \
$(OBJ)/iftParamOptimizationProblems.o \
$(OBJ)/iftSegmentationResuming.o \
$(OBJ)/iftFunctions.o \
$(OBJ)/iftManifold.o \
$(OBJ)/iftMaxflow.o \
$(OBJ)/iftRobot.o \
$(OBJ)/iftRISF.o \
$(OBJ)/iftIterativeOPF.o\
$(OBJ)/iftKmeans_v2.o\
$(OBJ)/iftSupervoxelAtlas.o\
$(OBJ)/iftImageSequence.o\
$(OBJ)/iftSpectrum.o\
$(OBJ)/iftMesothelioma.o\
ift_modules # compile all old ift files and the new modules

libiftcuda: libift \
        $(OBJ)/iftDeepLearning.cu.o \
        $(OBJ)/iftMatrix.cu.o \
        $(OBJ)/iftMemory.cu.o \
        $(OBJ)/iftFLIM.cu.o 




# getting and compiling all modules and submodules automatically
ift_modules:
	$(eval MODULES := $(wildcard $(SRC)/ift/*/.))
	$(eval MODULES_SRC_FILES = $(wildcard $(MODULES:=/*.c)))
	$(eval MODULES_OBJ_FILES = $(subst .c,.o,$(MODULES_SRC_FILES)))
	$(eval MODULES_OBJ_FILES = $(subst $(SRC),$(OBJ),$(MODULES_OBJ_FILES)))
	$(eval SUBMODULES = $(wildcard $(MODULES:=/*/.)))
	$(eval SUBMODULES_SRC_FILES = $(wildcard $(SUBMODULES:=/*.c)))
	$(eval SUBMODULES_OBJ_FILES = $(subst .c,.o,$(SUBMODULES_SRC_FILES)))
	$(eval SUBMODULES_OBJ_FILES = $(subst $(SRC),$(OBJ),$(SUBMODULES_OBJ_FILES)))
	$(eval ALL_OBJS = $(MODULES_OBJ_FILES) $(SUBMODULES_OBJ_FILES))
	make $(ALL_OBJS)


# # OLD generic src compilation
# $(OBJ)/%.o: $(SRC)/%.c $(INCLUDE)/%.h
# 	$(CC) $(FLAGS) -c $< $(INCLUDES) -o $@ 

# generic src compilation
$(OBJ)/%.o: $(SRC)/%.c $(INCLUDE)/%.h
	$(eval PARENT_DIR := $(shell dirname $@))
	mkdir -p $(PARENT_DIR)
	$(CC) $(FLAGS) -c $< $(INCLUDES) -o $@ 

# generic src compilation
$(OBJ)/%.cu.o: $(SRC)/%.cu $(INCLUDE)/%.h
	$(NVCC) -DIFT_GPU=1 -c $< $(INCLUDES) -o $@ --compiler-options '-fPIC' 

clean:
	rm -f $(LIB)/lib*.a; rm -rf $(OBJ)/*; rm -rf $(BIN)/*; rm -rf $(BUILD); rm -rf $(DOCS)/html; cd $(TEST); $(MAKE) clean;
	cd $(LIBANALYZE); $(MAKE) clean; cd -;
	cd $(LIBJPEG); $(MAKE) clean; cd -;
	cd $(LIBPNG); $(MAKE) clean; cd -;
	cd $(LIBSVM); $(MAKE) clean; cd -;
	cd $(LIBTIFF); $(MAKE) clean; cd -;
	cd $(TSNE); $(MAKE) clean; cd -;
	cd $(ZLIB); $(MAKE) clean; cd -;

tests:
	cd $(TEST); $(MAKE) clean; $(MAKE);

run_tests:
	cd $(TEST); $(MAKE) run;

doc:
	cd $(DOCS); doxygen; cp doxy-boot.js html; cd -;

