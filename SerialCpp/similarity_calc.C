/**
 * Contains serial representation to calculate the laplacian of a similarity matrix of a given file
 * 
 * Usage: ./similarity_calc <is_sim_matrix> <num_nodes> <graph_input_file> <epsilon> <output_file>
 * 
 * A similarity matrix is a representation of another matrix where all nodes that are
 * less than or equal to epsilon distance away from eachother are connected. This file
 * contains driver code to load a given matrix, compute the laplacian of the 
 * similarity matrix, and output the computed result to a file.
 * 
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <queue>

#include <set>

#include "utils.h"

struct QueueItem {
    int next_node;
    int distance;
};

/**
 * Traverses a graph for a given node marking nodes that it visits with a
 * negative 1 and its own value on diagonal with the number of nodes it visited.
 * 
 * Args:
 *  matrix: Adjacency matrix (NxN) of a graph with N nodes.
 *  sim_matrix: NxN graph to be filled in. Only row curr_node is modified.
 *  num_nodes: Total N nodes in the graph.
 *  curr_node: Current node to run BFS on.
 *  episilon: Max distance to traverse.
 */ 
void _recursive_laplacian_bfs(double **const matrix, double **sim_matrix, int num_nodes, int curr_node, int epsilon) {
    // Setup BFS data structures.
    std::set<int> visited_nodes; 
    std::queue<struct QueueItem> queue;

    // Add initial node.
    int num_visited = -1; // Don't count visit to self.
    queue.push((QueueItem){curr_node, epsilon});

    while (!queue.empty()) {
        // Get next node to visit.
        struct QueueItem next_visit = queue.front();
        queue.pop();
        
        // Only visit nodes that have not been visited.
        if (visited_nodes.find(next_visit.next_node) == visited_nodes.end()) {
            // Visit that node.
            num_visited += 1;
            visited_nodes.insert(next_visit.next_node);
            sim_matrix[curr_node][next_visit.next_node] = -1;

            // Visit the nodes neighbors.
            if (next_visit.distance != 0) {
                for (int n = 0; n < num_nodes; n++) {
                    if (n != next_visit.next_node) {
                        double next_distance = matrix[next_visit.next_node][n];
                        // Can only make jumps if remaining distance > edge weight
                        if (next_distance > 0 && next_visit.distance - next_distance >= 0) {
                            // Traverse edge.
                            queue.push((QueueItem){n, next_visit.distance - next_distance});
                        }
                    }
                } 
            }
        }
    }
    // Reference to self is the number of neighbors.
    sim_matrix[curr_node][curr_node] = num_visited;
}

/**
 * Builds an epsilon neighbor graph for a given matrix. Creates a connection
 * between all nodes that have a pairwise distance <= episilon.
 * 
 * Args:
 *  matrix: Adjacency matrix (NxN) of a graph with N nodes.
 *  num_nodes: Total N nodes in the graph.
 *  curr_node: Current node to run BFS on.
 *  episilon: Max distance to traverse.
 *  
 * Returns:
 *  sim_matrix: Filled laplacian similarity matrix of provided graph.
 */ 
double **build_epsilon_neighborhood(double **const matrix, int num_nodes, int epsilon) {
    double **sim_matrix = alloc_2d_array(num_nodes, num_nodes);
    for (int n = 0; n < num_nodes; n++) {
        _recursive_laplacian_bfs(matrix, sim_matrix, num_nodes, n, epsilon);
    }
    return sim_matrix;
}

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