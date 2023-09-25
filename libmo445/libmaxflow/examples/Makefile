LIBSVM=$(NEWIFT_DIR)/externals/libsvm
LIBMAXFLOW_DIR=$(NEWIFT_DIR)/libmaxflow
BIN=$(NEWIFT_DIR)/bin
LINK_OPTS=-L $(NEWIFT_DIR)/lib -L. -L$(LIBSVM)/lib -I$(LIBSVM)/include -L$(LIBMAXFLOW_DIR) -lmaxflow -lift -lsvm -lstdc++ -llapack -lcblas -lblas -fopenmp

libift:
	@echo Building libift
	cd $(NEWIFT_DIR); $(MAKE) -C $(NEWIFT_DIR); cd -\
	
libmaxflow:
	@echo Building libmaxflow
	cd $(LIBMAXFLOW_DIR); $(MAKE) -C $(LIBMAXFLOW_DIR); cd -\

gc_super: libift libmaxflow
	g++ $(CPP_FLAGS) -c gc_super.cpp -o obj/gc_super.o -I $(NEWIFT_DIR)/include -I$(LIBMAXFLOW_DIR) $(LINK_OPTS)
	g++ $(CPP_FLAGS) -o $(BIN)/gc_super obj/gc_super.o -I $(NEWIFT_DIR)/include -I$(LIBMAXFLOW_DIR) $(LINK_OPTS)
	
gc_pixel: libmaxflow
	g++ $(CPP_FLAGS) -c gc_pixel.cpp -o obj/gc_pixel.o -I $(NEWIFT_DIR)/include -I$(LIBMAXFLOW_DIR) $(LINK_OPTS)
	g++ $(CPP_FLAGS) -o $(BIN)/gc_pixel obj/gc_pixel.o  -I $(NEWIFT_DIR)/include -I$(LIBMAXFLOW_DIR) $(LINK_OPTS)
	
clean:
	rm obj/*.o
