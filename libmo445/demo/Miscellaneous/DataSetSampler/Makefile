BIN   =$(NEWIFT_DIR)/bin

IFTFLAGS = -I$(NEWIFT_DIR)/include

UNAME_S=$(shell uname -s)

FLAGS= -O3 -fopenmp -pthread -std=gnu11 -pedantic
DEBUG=0
ifeq ($(DEBUG), 1)
	FLAGS += -g
endif

ifeq ($(UNAME_S), Linux)
INCLUDES=-I$(NEWIFT_DIR)/include
IFTLIB=-L$(NEWIFT_DIR)/lib -lift -L/usr/lib/atlas-base -llapack -lblas -lcblas  -lpng -lm -lz
LIBSVM=-L$(NEWIFT_DIR)/libsvm -lsvm -lstdc++
CC=gcc
else
INCLUDES=-I$(NEWIFT_DIR)/include -I$(NEWIFT_DIR)/windows/libcblas/include
# Considering that IFT will be compiled using 64-bit MinGW. Also, cblas and lapack are provided inside IFT's directory
export CC=x86_64-w64-mingw32-gcc
IFTLIB    = -L$(NEWIFT_DIR)/lib -L$(NEWIFT_DIR)/windows/liblapack/lib -L$(NEWIFT_DIR)/windows/cblas/lib -lift -llapack -lblas -lz -lpng -lm
endif


libift:
	cd ../../; $(MAKE) -C $(NEWIFT_DIR); cd -\


$@.c: libift $@.c
	make
	$(CC) $(FLAGS) $(INCLUDES) $@.c -o $(BIN)/$@ $(IFTLIB) $(LIBSVM); \

clean:
	rm $(BIN)/*;










