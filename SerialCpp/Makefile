CXX=c++
CXX_FLAGS = -Wall -std=c++11
FILES = utils.C

all: similarity_calc

similarity_calc: similarity_calc.C
	$(CXX) $(CXX_FLAGS) similarity_calc.C $(FILES) -o similarity_calc

kmeans: kmeans.cpp
	$(CXX) kmeans.cpp -o kmeans

make test_sim: all
	./similarity_calc 0 5 ../SampleInput/small5x5.input 1 output.txt

make clean:
	rm similarity_calc output.txt