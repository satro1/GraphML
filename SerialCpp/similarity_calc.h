#include "impl.h"

#include <vector>

struct QueueItem {
  int next_node;
  double distance;
};

#if ADJ_MATRIX
void build_epsilon_neighborhood(double **const matrix, double **const sim_matrix, int num_nodes, int epsilon, std::set<int> **visited_nodes_allocator, std::queue<struct QueueItem> **queue_allocator);
#else
void build_epsilon_neighborhood(std::vector<std::vector<int>> matrix, double **const sim_matrix, int num_nodes, int epsilon, std::set<int> **visited_nodes_allocator, std::queue<struct QueueItem> **queue_allocator);
#endif