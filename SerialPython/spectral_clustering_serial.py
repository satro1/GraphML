import numpy as np
import random
random.seed(498)

import networkx as nx
import matplotlib.pyplot as plt

### Sample Graph
# Set network
graph_adjacency_list = {
    0: [2, 5],
    1: [2, 4, 3],
    2: [0, 5, 1],
    3: [1, 6],
    4: [1, 6],
    5: [0, 2],
    6: [3, 4]
}
G = nx.Graph()
num_nodes = len(graph_adjacency_list)
graph = np.zeros((num_nodes, num_nodes))

for node in graph_adjacency_list.keys():
  G.add_node(node)

for node, edges in graph_adjacency_list.items():
  for edge in edges:
    G.add_edge(node, edge)
    graph[node, edge] = 1
    graph[edge, node] = 1

### Helper funtions
def visualize_numpy(graph):
  nx.draw(nx.from_numpy_matrix(graph), with_labels=True, font_weight='bold')
  plt.show()

def preserve_only_connections_in_graph(graph, clusters):
  """ Preserves only connections between noes in the same cluster. """
  s_graph = graph.copy()
  num_nodes = s_graph.shape[0]
  for node in range(num_nodes):
    node_cluster = clusters[node]
    for neighbor in range(num_nodes):
      if node_cluster != clusters[neighbor]:
        s_graph[node, neighbor] = 0
        s_graph[neighbor, node] = 0
  return s_graph

### Similarity Matrix
# A similarity matrix is a graph built from another graph that maintains
# connections to nodes considered "close"
def build_epsilon_neighborhood(graph, epsilon):
  """ Builds an epsilon neighbor graph for a given. Creates a connection between all 
  elements that have a pairwise distance <= episilon 

  Inputs:
    graph: An adjacency matrix representing an (un)directed, (un)weighted graph.
    epsilon: An integer cutoff for distance

  Returns:
    similarity_matrix: Laplacian of similarity matrix of above graph.
  """
  num_nodes = graph.shape[0]
  similarity_matrix = np.zeros(graph.shape)
  # Perform a BFS of depth epsilon. Marking all visited nodes.
  for node in range(num_nodes):
    # Keep track of node to visit
    queue = [(node, epsilon)]
    visited_nodes = [False] * num_nodes
    num_visited = -1

    while (len(queue) > 0):
      # Visit that node.
      next_node, dist = queue.pop(0)
      
      if not visited_nodes[next_node]:
        # Handle visit.
        num_visited += 1
        similarity_matrix[node, next_node] = -1
        visited_nodes[next_node] = True

        # Explore neighbors
        if dist != 0:        
          # Get all the neighbors of that node
          for neighbor in range(num_nodes):
            if neighbor != next_node and not visited_nodes[neighbor]:
              next_dist = graph[next_node][neighbor]
              if (next_dist > 0) and (dist - next_dist >= 0):
                # Visit edge
                queue.append((neighbor, dist - next_dist))

    similarity_matrix[node, node] = num_visited
  # TODO makesure distance bothways is valid
  return similarity_matrix

# Evaluate
visualize_numpy(graph)

# # Should match original graph
# visualize_numpy(build_epsilon_neighborhood(graph, 1))
# # All nodes should be connected to nodes distance 2
visualize_numpy(build_epsilon_neighborhood(graph, 2))
# # Should be a completely connected graph
# visualize_numpy(build_epsilon_neighborhood(graph, 6))

from sklearn.cluster import KMeans
def spectral_clustering(graph, clusters_to_create, epsilon_distance = 3):
  """ Runs the spectral clustering algorithm with a fixed number of clusters on the graph.
  See page 6 of https://people.csail.mit.edu/dsontag/courses/ml14/notes/Luxburg07_tutorial_spectral_clustering.pdf.

  Inputs:
    graph: Graph to cluster, as an adjacency matrix.
    clusters_to_create: Number of clusters to create an integer
    epsilon_distance: epsilon distance for similarity algorithm
  Returns:
    Array of adjacency matrixes
  """
  # Create adjacency matrix of similarity graph. 
  # Adjacency matrix: diagonal is number of edges out of node. [x,y] with x != y is 0 if no edge and 1 if edge between x and y
  # Laplacian = degree matrix - adjacency matrix
  laplacian_of_sim = build_epsilon_neighborhood(graph, epsilon_distance)

  # Compute eigen values and vectors
  _, eig_vectors = np.linalg.eig(laplacian_of_sim)

  # first eig (columns are eigenvectors)
  first_k_eig_vectors = eig_vectors[:, :clusters_to_create]

  # Cluster the N points of length K into K clusters
  kmeans = KMeans(n_clusters=3, random_state=0).fit(first_k_eig_vectors)
  return kmeans.labels_

# Test example.
clusters = spectral_clustering(graph, 2, epsilon_distance=2)
graph_only_clusters_preserved = preserve_only_connections_in_graph(graph, clusters)
visualize_numpy(graph_only_clusters_preserved)