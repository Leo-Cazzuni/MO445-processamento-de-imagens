CPP_FLAGS =  -w -fPIC -fopenmp -pthread -lsvm -llapack -lcblas -lblas

all:
	@echo Building libdgc
	g++ $(CPP_FLAGS) -c graph.cpp -o obj/graph.o
	ar rvs libdgc.a obj/graph.o 

clean:
	rm obj/graph.o
	rm libdgc.a