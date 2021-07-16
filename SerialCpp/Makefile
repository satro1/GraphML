CXX=/usr/local/opt/llvm/bin/clang++
CXX_FLAGS = -fopenmp -O3 -Wall -std=c++11 -I/usr/local/opt/llvm/include -L/usr/local/opt/llvm/lib
FILES = main.C similarity_calc.C eigen.C kmeans.C utils.C

all: combined

combined: $(FILES)
	$(CXX) $(CXX_FLAGS) $(FILES) -o spectral

size=512

run: all
	./spectral 0 $(size) 3 ../TestInput/sim_mtx/size${size}/input_${size}x${size}_1.input 3 output.txt

social_network_fb: all
	./spectral 1 4039 45 ../SampleInput/facebook_combined.adj 3 output.txt