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
        // ERROR USING QUEUES, CHECK Support loading from -*similarity*- ADJACENCY list.
        // double temp;
        for (int r = 0; r < num_nodes; r++) {
            for (int c = 0; c < num_nodes; c++) {
                fscanf(fp, "%lf%*c", &matrix[r][c]);
            }
        }
    }
    fclose(fp);
    if (DEBUG) printf("Read in matrix.\n");

    // Create Laplacian.  
    double start_simulation = omp_get_wtime(); 
    double **sim_laplacian = build_epsilon_neighborhood(matrix, num_nodes, epsilon);
    if (DEBUG) printf("Computed laplacian.\n");
    double end_laplacian = omp_get_wtime();

    // Compute eigenvectors.
    double* evalues = (double*) calloc(num_nodes, sizeof(double));
    eigen(sim_laplacian, evalues, num_nodes);
    if (DEBUG) printf("Computed eigenvectors.\n");
    double end_eigen = omp_get_wtime();

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
    double end_kmeans = omp_get_wtime();
    if (DEBUG) printf("Computed clusters.\n");
    

    // Output Timing
    double total_time = end_kmeans-start_simulation;
    double sim_time = end_laplacian-start_simulation;
    double eig_tmime = end_eigen-end_laplacian;
    double kmeans_time = end_kmeans-end_eigen;
    std::cout << "Timing for " << total_processes << " threads...\n";
    std::cout << "Total Execution Time: " << 1000 * total_time << "ms\n";
    std::cout << "Laplacian Time: " << 1000 * sim_time << "ms\n";
    std::cout << "Eigen Time: " << 1000 * eig_tmime << "ms\n";
    std::cout << "k-Means Time: " << 1000 * kmeans_time << "ms\n";
    // Output clusters.
    // FILE *output = fopen(output_filename, "w");
    // for (int cluster = 0; cluster < num_clusters; c++) {

    // }
}
