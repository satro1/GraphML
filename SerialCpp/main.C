/**
 * Usage: ./similarity_calc <is_sim_matrix> <num_nodes> <graph_input_file> <epsilon> <output_file>
 */ 
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <queue>
#include <vector>
#include <string>

#include <set>

#include "similarity_calc.h"
#include "utils.h"
#include "eigen.h"
#include "kmeans.h"

#define DEBUG true

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
    double **sim_laplacian = build_epsilon_neighborhood(matrix, num_nodes, epsilon);
    if (DEBUG) printf("Computed laplacian.\n");
    
    // Compute eigenvectors.
    double* evalues = (double*) calloc(num_nodes, sizeof(double));
    eigen(sim_laplacian, evalues, num_nodes);
    if (DEBUG) printf("Computed eigenvectors.\n");

    // Create n points of size k
    std::vector<std::vector<double>> eigenpoints;
    for (int n = 0; n < num_nodes; n++) {
        std::vector<double> data;
        for (int k = 0; k < num_clusters; k++) {
            data.push_back(sim_laplacian[n][k]);
        }
        eigenpoints.push_back(data);
    }
    if (DEBUG) printf("Computed points.\n");

    // Compute K-Means
    std::string output(output_filename);
    runKMeans(eigenpoints, num_clusters, num_clusters, false, output);
    if (DEBUG) printf("Computed clusters.\n");

    // Output clusters.
    // FILE *output = fopen(output_filename, "w");
    // for (int r = 0; r < num_nodes; r++) {
    //     for (int c = 0; c < num_nodes; c++) {
    //         fprintf(output, "%lf ", sim_laplacian[r][c]);
    //     }
    //     fprintf(output, "\n");
    // }
    // fclose(output);
}