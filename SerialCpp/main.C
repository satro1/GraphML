/**
 * Usage: ./similarity_calc <is_sim_matrix> <num_nodes> <graph_input_file> <epsilon> <output_file>
 */ 
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <queue>

#include <set>

#include "similarity_calc.h"
#include "utils.h"

/**
 * Sample code that reads in matrix, computes laplacian of similarity matrix
 * then outputs to a file.
 */ 
int main(int argc, char **argv) {
    // Check args.
    if (argc != 6) {
        printf("Usage ./sim_calc <is_sim_matrix = 0> <num_nodes> <filename of adj list>");
        exit(1);
    }
    bool is_sim_matrix = atoi(argv[1]) == 0;
    int num_nodes = atoi(argv[2]);
    char *input_filename = argv[3];
    int epsilon = atoi(argv[4]);
    char *output_filename = argv[5];

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

    // Create Laplacian.  
    double **sim_laplacian = build_epsilon_neighborhood(matrix, num_nodes, epsilon);

    // Output to adjacency matrix as file.
    FILE *output = fopen(output_filename, "w");
    for (int r = 0; r < num_nodes; r++) {
        for (int c = 0; c < num_nodes; c++) {
            fprintf(output, "%lf ", sim_laplacian[r][c]);
        }
        fprintf(output, "\n");
    }
    fclose(output);
}