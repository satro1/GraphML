# GraphML
Implements serial (and paralell) implementation of spectral clustering algorithm.

## Spectral Clustering Algorithm
0. Choose K number of clusters to compute of a graph G.
1. Build a similarity matrix of G. A similarity matrix is a new matrix G'
s.t. nodes that are "close" in G are connected in G'. We use an epsilon neighborhood
definition of close, which defines two nodes to be close if they are less than
or equal to epsilon hops away from eachother in the network.
2. Compute the Laplacian of that similarity matrix. A Laplacian is defined to be
(x, y) = {-1 if (x != y and x is connected to y); 0 if (x != y and x not connected to y);
number_of_neighbors of x if (x == y)}
3. Compute first K eigenvectors (of length N) of the Laplacian.
4. Select first K eigenvectors. Create N points of dimensionality K from the
K eigenvectors. Point 0 (N0) = first point of each k eigenvector.
5. Compute K-Means Clustering Algorithm for these N points. The resulting
cluster is the cluster for the node N in the graph.

## SerialCpp
Contains the (partial) serial implementation of spectral clustering algorithm.

## SerialPython
Contains serial python implentation of spectral clustering algorith.
