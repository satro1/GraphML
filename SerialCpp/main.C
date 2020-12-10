/**
 * Usage: ./similarity_calc <is_sim_matrix> <num_nodes> <graph_input_file> <epsilon> <output_file>
 */ 
#include <stdlib.h>
#include <iostream>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <queue>
#include <vector>
#include <string>
#include <chrono>
#include <set>

#include "omp.h"


#include "similarity_calc.h"
#include "utils.h"
#include "eigen.h"
#include "kmeans.h"

#define DEBUG false

/**
 * Sample code that reads in matrix, computes laplacian of similarity matrix
 * then outputs to a file.
 */ 
int main(int argc, char **argv) {
    // Check args.
    if (argc != 7) {
        printf("Usage ./sim_calc <is_sim_matrix = 0> <num_nodes> <num_clusters> <filename of adj list>");
        exit(1);
    }
    bool is_sim_matrix = atoi(argv[1]) == 0;
    int num_nodes = atoi(argv[2]);
    int num_clusters = atoi(argv[3]);
    char *input_filename = argv[4];
    double epsilon = (double) atoi(argv[5]);
    char *output_filename = argv[6];
    if (DEBUG) printf("Parsed args.\n");

    int total_processes = omp_get_max_threads();
    if (DEBUG) printf("Running on %d threads\n", total_processes);

    // Read in file.
    double **matrix = alloc_2d_array(num_nodes, num_nodes);
    FILE *fp = fopen(input_filename, "r");
    if (fp == NULL) {
        printf("Failed to open provided file.\n");
        exit(1);
    }
    if (is_sim_matrix) {
        for (int r = 0; r < num_nodes; r++) {
            for (int c = 0; c < num_nodes; c++) {
                fscanf(fp, "%lf%*c", &matrix[r][c]);
            }
        }  
    } else {
        // TODO(iancostello): Support loading from similarity list.
        printf("Adjacency list not yet supported.");
        exit(0);
    }
    fclose(fp);
    if (DEBUG) printf("Read in matrix.\n");

    // Create Laplacian.  
    auto start_simulation = std::chrono::high_resolution_clock::now();
    double **sim_laplacian = build_epsilon_neighborhood(matrix, num_nodes, epsilon);
    if (DEBUG) printf("Computed laplacian.\n");
    auto end_laplacian = std::chrono::high_resolution_clock::now();

    // Compute eigenvectors.
    double* evalues = (double*) calloc(num_nodes, sizeof(double));
    eigen(sim_laplacian, evalues, num_nodes);
    if (DEBUG) printf("Computed eigenvectors.\n");
    auto end_eigen = std::chrono::high_resolution_clock::now();

    // Create n points of size k
    std::vector<std::vector<double>> eigenpoints;
    for (int n = 0; n < num_nodes; n++) {
        std::vector<double> data;
        for (int k = 0; k < num_clusters; k++) {
            data.push_back(sim_laplacian[n][k]);
        }
        eigenpoints.push_back(data);
    }

    // Compute K-Means
    std::string km_output(output_filename);
    std::vector<std::vector<std::vector<double>>> clusters = 
        runKMeans(eigenpoints, num_clusters, num_clusters, false, km_output);
    auto end_kmeans = std::chrono::high_resolution_clock::now();
    if (DEBUG) printf("Computed clusters.\n");
    

    // Output Timing
    double total_time = std::chrono::duration<double, std::milli>(end_kmeans-start_simulation).count();
    double sim_time = std::chrono::duration<double, std::milli>(end_laplacian-start_simulation).count();
    double eig_tmime = std::chrono::duration<double, std::milli>(end_eigen-end_laplacian).count();
    double kmeans_time = std::chrono::duration<double, std::milli>(end_kmeans-end_eigen).count();
    std::cout << "Timing for " << total_processes << " threads...\n";
    std::cout << "Total Execution Time: " << total_time << "ms\n";
    std::cout << "Laplacian Time: " << sim_time << "ms\n";
    std::cout << "Eigen Time: " << eig_tmime << "ms\n";
    std::cout << "k-Means Time: " << kmeans_time << "ms\n";
    // Output clusters.
    // FILE *output = fopen(output_filename, "w");
    // for (int cluster = 0; cluster < num_clusters; c++) {

    // }
}