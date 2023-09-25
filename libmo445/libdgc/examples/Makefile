CPP_FLAGS =  -w -fPIC -fopenmp -pthread -lsvm -llapack -lcblas -lblas
LIBDGC_DIR=$(NEWIFT_DIR)/libdgc/
LIBSVM=$(NEWIFT_DIR)/libsvm
LINK_OPTS=-L$(LIBSVM) -lsvm -lstdc++ -llapack -lcblas -lblas -fopenmp

example:
	g++ $(CPP_FLAGS) -c example.cpp -o obj/example.o -I $(LIBDGC_DIR)
	g++ -o $(NEWIFT_DIR)/bin/example obj/example.o -L $(LIBDGC_DIR)
	
example2:
	g++ $(CPP_FLAGS) -c example2.cpp -o obj/example2.o -I $(LIBDGC_DIR)
	g++ -o $(NEWIFT_DIR)/bin/example2 obj/example2.o -L $(LIBDGC_DIR)
	
libift:
	@echo Building libift
	cd $(NEWIFT_DIR); $(MAKE) -C $(NEWIFT_DIR); cd -\
	
libdgc:
	@echo Building libdgc
	cd $(LIBDGC_DIR); $(MAKE) -C $(LIBDGC_DIR); cd -\

dgc_super: libift libdgc
	g++ $(CPP_FLAGS) -c dgc_super.cpp -o obj/dgc_super.o -I $(NEWIFT_DIR)/include -I$(LIBDGC_DIR)
	g++ $(CPP_FLAGS) -o $(NEWIFT_DIR)/bin/dgc_super obj/dgc_super.o -L $(NEWIFT_DIR)/lib -L $(LIBDGC_DIR) -ldgc -lift -L $(LINK_OPTS)
	
dgc_pixel: libift libdgc
	g++ $(CPP_FLAGS) -c dgc_pixel.cpp -o obj/dgc_pixel.o -I $(NEWIFT_DIR)/include -I$(LIBDGC_DIR)
	g++ $(CPP_FLAGS) -o $(NEWIFT_DIR)/bin/dgc_pixel obj/dgc_pixel.o -L $(NEWIFT_DIR)/lib -L $(LIBDGC_DIR) -ldgc -lift -L $(LINK_OPTS)

clean:
	rm obj/*.o