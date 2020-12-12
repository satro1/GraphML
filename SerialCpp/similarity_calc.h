#include "impl.h"

#include <vector>

#if ADJ_MATRIX
void build_epsilon_neighborhood(double **const matrix, double **const sim_matrix, int num_nodes, int epsilon);
#else
void build_epsilon_neighborhood(std::vector<std::vector<int>> matrix, double **const sim_matrix, int num_nodes, int epsilon);
#endif