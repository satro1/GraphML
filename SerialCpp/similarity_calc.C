/**
 * Contains serial representation to calculate the laplacian of a similarity
 * matrix of a given file
 *
 * A similarity matrix is a representation of another matrix where all nodes
 * that are less than or equal to epsilon distance away from eachother are
 * connected. This file contains driver code to load a given matrix, compute the
 * laplacian of the similarity matrix, and output the computed result to a file.
 *
 */

#include <assert.h>
#include <queue>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <set>

#include "utils.h"
#include "impl.h"
#include "omp.h"
#include "similarity_calc.h"

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
#if ADJ_MATRIX
void _recursive_laplacian_bfs(double **const matrix, double **sim_matrix,
                              int num_nodes, int curr_node, double epsilon,
                              std::set<int> *visited_nodes, std::queue<struct QueueItem> *queue) {
#else
void _recursive_laplacian_bfs(std::vector<std::vector<int>> matrix, double **sim_matrix,
                              int num_nodes, int curr_node, double epsilon,
                              std::set<int> *visited_nodes, std::queue<struct QueueItem> *queue) {
#endif
  // Add initial node.
  int num_visited = -1; // Don't count visit to self.
  queue->push((QueueItem){curr_node, epsilon});
  visited_nodes->clear();

  while (!queue->empty()) {
    // Get next node to visit.
    struct QueueItem *next_visit = &queue->front();
    queue->pop();

    // Only visit nodes that have not been visited.
    if (visited_nodes->find(next_visit->next_node) == visited_nodes->end()) {
      // Visit that node.
      num_visited += 1;
      visited_nodes->insert(next_visit->next_node);
      sim_matrix[curr_node][next_visit->next_node] = -1;

      // Visit the nodes neighbors.
      if (next_visit->distance != 0) {
        for (int n = 0; n < num_nodes; n++) {
          #if ADJ_MATRIX
          if (n != next_visit->next_node) {
            
            double next_distance = matrix[next_visit->next_node][n];
            // Can only make jumps if remaining distance > edge weight
            if (next_distance > 0 && next_visit->distance - next_distance >= 0) {
              // Traverse edge.
              queue->push((QueueItem){n, next_visit->distance - next_distance});
            }
            
          }
          #else
          if (next_visit->distance >= 1) {
            for (int n : matrix.at(next_visit->next_node)) {
              queue->push((QueueItem){n, next_visit->distance - 1});
            }
          }
          #endif
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
#if ADJ_MATRIX
void build_epsilon_neighborhood(double **const matrix, double **const sim_matrix, int num_nodes,
                                    int epsilon, std::set<int> **visited_nodes_allocator, std::queue<struct QueueItem> **queue_allocator) {
#else
void build_epsilon_neighborhood(std::vector<std::vector<int>> matrix, double **const sim_matrix, int num_nodes, int epsilon,
                                std::set<int> **visited_nodes_allocator, std::queue<struct QueueItem> **queue_allocator) {
#endif
  #pragma omp parallel for schedule(dynamic)
  for (int n = 0; n < num_nodes; n++) {
    int thread_num = omp_get_thread_num();
    _recursive_laplacian_bfs(matrix, sim_matrix, num_nodes, n, epsilon, visited_nodes_allocator[thread_num], queue_allocator[thread_num]);
  }
}