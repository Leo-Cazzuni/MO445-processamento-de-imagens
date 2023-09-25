BIN=./bin
CORE=./core
TEST_SUIT=./test_suit

LIBSVM_DIR = $(NEWIFT_DIR)/externals/libsvm
LIBNIFTI_DIR= $(NEWIFT_DIR)/externals/libnifti
LIBJPEG_DIR= $(NEWIFT_DIR)/externals/libjpeg
LIBPNG_DIR= $(NEWIFT_DIR)/externals/libpng
TSNE_DIR= $(NEWIFT_DIR)/externals/tsne
ZLIB=$(NEWIFT_DIR)/externals/zlib
#LIBSDL2_DIR=$(NEWIFT_DIR)/externals/libsdl2_minimal

LIBIFT_INC   = -I $(NEWIFT_DIR)/include
LIBIFT_LD    = -L $(NEWIFT_DIR)/lib -lift
LIBSVM_INC   = -I $(LIBSVM_DIR)/include 
LIBSVM_LD    = -L $(LIBSVM_DIR)/lib -lstdc++
LIBCBLAS_INC = -I /usr/local/opt/openblas/include
LIBCBLAS_LD  = -L /usr/local/opt/openblas/lib -L /lib64/atlas-sse3 -L /usr/lib/atlas-base -llapack -lblas -lcblas

LIBNIFTI_INC = -I $(LIBNIFTI_DIR)/include
LIBJPEG_INC = -I $(LIBJPEG_DIR)/include
TSNE_INC = -I $(TSNE_DIR)/include
ZLIB_INC = -I $(ZLIB)/include
#LIBSDL2_INC = -I $(LIBSDL2_DIR)/include
#LIBSDL2_LD = -L $(LIBSDL2_DIR) -lSDL2

EXTERNALS_LD = -fopenmp -lm

INCLUDES = $(LIBIFT_INC) $(LIBSVM_INC) $(LIBCBLAS_INC) $(LIBNIFTI_INC) $(LIBJPEG_INC) $(TSNE_INC) $(ZLIB_INC)
LIBS     = $(LIBIFT_LD) $(LIBSVM_LD) $(LIBCBLAS_LD) $(EXTERNALS_LD)

FLAGS= -O3 -Wall -Wno-unused-result -fopenmp -pthread -std=gnu11 -pedantic


#CUDA path in case IFT_GPU is enabled
export CUDA_DIR1=/usr/local/cuda
export CUDA_DIR2=/opt/cuda

IFT_GPU=0
ifeq ($(IFT_GPU), 1)
	export FLAGS += -DIFT_GPU=1
	INCLUDES += -I$(CUDA_DIR1)/include
	INCLUDES += -I$(CUDA_DIR2)/include
	LIBS   += -L$(CUDA_DIR1)/lib64 -L$(CUDA_DIR2)/lib64 -lcublas -lcudart
endif


TEST_SRCS = $(wildcard $(TEST_SUIT)/*.c)
TEST_PROGS = $(patsubst $(TEST_SUIT)/%.c,$(BIN)/%,$(TEST_SRCS))


all: libift core_test_suit test_suit

libift:
	cd $(NEWIFT_DIR); $(MAKE); cd -\


core_test_suit: $(CORE)/unitytest.o

test_suit: $(TEST_PROGS)


run:
	for prog in $(TEST_PROGS); do \
		./$${prog}; \
		echo ""; \
	done


$(CORE)/%.o: $(CORE)/%.c
	@echo ""
	gcc -c $< -o $@ $(FLAGS) -I$(CORE) $(INCLUDES) $(LIBS)

$(BIN)/%: $(TEST_SUIT)/%.c
	@echo ""
	gcc $< $(CORE)/*.o -o $@ $(FLAGS) -I$(CORE) $(INCLUDES) $(LIBS)  



clean:
	rm -rf $(BIN)/* $(CORE)/*.o