CPP_FLAGS =  -w -fPIC -fopenmp -pthread -lsvm -llapack -lcblas -lblas

all:
	@echo Building libmaxflow
	g++ $(CPP_FLAGS) -c graph.cpp -o obj/graph.o
	g++ $(CPP_FLAGS) -c maxflow.cpp -o obj/maxflow.o
	ar rvs libmaxflow.a obj/graph.o obj/maxflow.o

clean:
	rm obj/graph.o obj/maxflow.o
	rm libmaxflow.a
