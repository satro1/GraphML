CXX=g++
CXX_FLAGS = -fopenmp -O2 -Wall -lm
FILES = main.C similarity_calc.C eigen.C kmeans.C utils.C

all: combined

combined: $(FILES)
	$(CXX) $(CXX_FLAGS) $(FILES) -o spectral

run: all
	./spectral 0 5 3 ../TestInput/size512/input_512x512_1.input 1 output.txt

social_network: all
	./spectral 0 37000 100 ~/Downloads/git_web_ml/social_network.csv 1 output.txt

similarity_calc: similarity_calc.C
	$(CXX) $(CXX_FLAGS) similarity_calc.C $(FILES) -o similarity_calc

eigen: eigen.C
	$(CXX) $(CXX_FLAGS) eigen.C $(FILES) -o eigen

kmeans: kmeans.cpp
	$(CXX) kmeans.cpp -o kmeans

make test_sim: all
	./similarity_calc 0 5 3 ../SampleInput/small5x5.input 1 output.txt

make clean:
	rm similarity_calc output.txt
