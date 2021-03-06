#include <vector>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <fstream>
#include <ctime>
#include <chrono>

using namespace std;

#define NUM_CLUSTERS 5
#define DIM 5
#include <math.h>

/**
 * Method to find random centroisd initially.
 * Goes through all the elements and 
 * */
vector<vector<double>> getRandomCentroids(vector<vector<double>> nodes, int dim, int numClusters) {
    vector<double> min(dim, 0);
    vector<double> max(dim, 0);

    #pragma omp parallel for
    for (int i = 0; i < dim; i++) {
        min[i] = nodes[0][i];
        max[i] = nodes[0][i];
    }

    #pragma omp parallel for collapse(2) shared(min, max)
    for (int i = 0; i < nodes.size(); i++) {
        for (int j = 0; j < dim; j++) {
            if (nodes[i][j] < min[j]) {
                min[j] = nodes[i][j];
            }
            if (nodes[i][j] > max[j]) {
                max[j] = nodes[i][j];
            }
        }
    }

    // sede
    srand(0);

    vector<vector<double>> ret(numClusters, vector<double>(dim, 0));
    
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < numClusters; i++) {
        for (int j = 0; j < dim; j++) {
            // set this to a random value between min[j] and max[j]
            ret[i][j] = ((double) rand() / (RAND_MAX)) * (max[j] - min[j]) + min[j];
        }
    }

    return ret;
}

/**
 * Method to get a new mean given a cluster
 * */
vector<double> getMean(vector<vector<double>> cluster, int dim) {
    int numElem = 0;
    vector<double> mean(dim, 0);

    #pragma omp parallel for shared(mean)
    for (int i = 0; i < cluster.size(); i++) {
        for (int j = 0; j < dim; j++) {
            mean[j] += cluster[i][j];
        }
        numElem++;
    }

    #pragma omp parallel for
    for (int i = 0; i < dim; i++) {
        mean[i] /= numElem;
    }

    return mean;
}

double distanceBetweenVectors(vector<double> a, vector<double> b, int dim) {
    double dist = 0;
    
    #pragma omp parallel for shared(dist)
    for (int i = 0; i < dim; i++) {
        dist += pow((a[i] - b[i]), 2);
    }

    return pow(dist, 0.5);
}

int getIndexOfClosestCentroid(vector<vector<double>> centroids, vector<double> element, int dim) {
    int indexOfClosest = -1;
    double minDistance = 0;
    
    #pragma omp parallel for shared(minDistance, indexOfClosest)
    for (int i = 0; i < centroids.size(); i++) {
        double tempDistance = distanceBetweenVectors(centroids[i], element, dim);
        if (tempDistance < minDistance || indexOfClosest == -1) {
            minDistance = tempDistance;
            indexOfClosest = i;
        }
    }

    return indexOfClosest;
}

vector<vector<vector<double>>> runKMeans(vector<vector<double>> elements, int dimensionOfVectors, int numClusters, bool verbose, string fileOut) {
    // generate random centroids here

    // auto t_start = std::chrono::high_resolution_clock::now();
    
    vector<vector<double>> centroids = getRandomCentroids(elements, dimensionOfVectors, numClusters);

    // auto t_end = std::chrono::high_resolution_clock::now();
    // cout << "Time to generate the random centroids: "
    //           << std::chrono::duration<double, std::milli>(t_end-t_start).count()
    //           << " ms\n";

    if (verbose) {
        cout << "Random centroids: \n";
        for (int i = 0; i < centroids.size(); i++) {
            for (auto j = centroids[i].begin(); j != centroids[i].end(); ++j)
                cout << *j << ' ';
            cout << "\n";
        }
    }

    // create the clusters now
    vector<vector<vector<double>>> clusters(numClusters, vector<vector<double>>(elements.size(), vector<double>(dimensionOfVectors, 0)));
    vector<int> sizes(numClusters, 0);
    map<vector<double>, int> elementToClusterIndex;     // map(vector<double> => int index of cluster in vector)
    

    // t_start = std::chrono::high_resolution_clock::now();

    // place the elements into their initial clusters
    #pragma omp parallel for shared(clusters, elementToClusterIndex, sizes)
    for (int i = 0; i < elements.size(); i++) {
        // find which centroid its closest to
        int indexOfClosestCentroid = getIndexOfClosestCentroid(centroids, elements[i], dimensionOfVectors);
        clusters[indexOfClosestCentroid][sizes[indexOfClosestCentroid]++] = elements[i];
        elementToClusterIndex.insert(pair<vector<double>, int>(elements[i], indexOfClosestCentroid));

        if (verbose) {
            cout << "Element " << i << " was found to be closest to index " << indexOfClosestCentroid;
            cout << "\nELEMENT: ";
            for (int j = 0; j < dimensionOfVectors; j++) {
                cout << elements[i][j] << ",";
            }
            cout << "\nThis index is mean ";
            for (int j = 0; j < dimensionOfVectors; j++) {
                cout << centroids[indexOfClosestCentroid][j] << ",";
            }
            cout << "with a distance of " << distanceBetweenVectors(centroids[indexOfClosestCentroid], elements[i], dimensionOfVectors) << "\n";
        }
    }

    // t_end = chrono::high_resolution_clock::now();
 
    // if (verbose) {
    //     cout << "Wall clock time For finding initial clusters: "
    //           << std::chrono::duration<double, std::milli>(t_end-t_start).count()
    //           << " ms\n";
    // }

    // t_start = chrono::high_resolution_clock::now();

    bool convergence = false;
    while (!convergence) {
        if (verbose) cout << "Iteration\n";
        convergence = true;
        // regenerate the centroids so they are the new means
        #pragma omp parallel for
        for (int i = 0; i < numClusters; i++) {
            centroids[i] = getMean(clusters[i], dimensionOfVectors);
        }
        // reset sizes
        for (int i = 0; i < sizes.size(); i++) {
            sizes[i] = 0;
        }

        // loop through every element and put them their new respective cluster
        #pragma omp parallel for shared(sizes, elementToClusterIndex)
        for (int i = 0; i < elements.size(); i++) {
            int newIndex = getIndexOfClosestCentroid(centroids, elements[i], dimensionOfVectors);
            int oldIndex = elementToClusterIndex[elements[i]];
            if (newIndex != oldIndex) {
                // separate
                convergence = false;
            }

            // either way we add the element in to the "new" cluster
            clusters[newIndex][sizes[newIndex]++] = elements[i];
            // elementToClusterIndex.insert(pair<vector<double>, int>(elems[i], newIndex));
            elementToClusterIndex[elements[i]] = newIndex;
        }
    }

    // t_end = chrono::high_resolution_clock::now();

    // if (verbose) {
    //     cout << "Wall clock time For iterating through and finding clusters: "
    //         << std::chrono::duration<double, std::milli>(t_end-t_start).count()
    //         << " ms\n";
    // }


    if (verbose) {
        cout << "Converged!\n";
        // now we want to print the clusters
        for (int i = 0; i < numClusters; i++) {
            cout << "Cluster " << i << "\n";
            for (int j = 0; j < sizes[i]; j++) {
                for (auto k = clusters[i][j].begin(); k != clusters[i][j].end(); ++k)
                    cout << *k << ' ';
                cout << '\n';
            }
        }    
    }

    // if (fileOut.length() > 0) {
    //     // write the clusters to a file
    //     ofstream output;
    //     output.open(fileOut);
    //     for (int i = 0; i < dimensionOfVectors; i++) output << "x" << i << ",";
    //     output << "cluster\n";
    //     for (int i = 0; i < numClusters; i++) {
    //         for (int j = 0; j < sizes[i]; j++) {
    //             for (auto k = clusters[i][j].begin(); k != clusters[i][j].end(); ++k)
    //                 output << *k << ',';
    //             output << i << '\n';
    //         }
    //     }  
    //     output.close();
    // }

    // test to see this map makes sense
    if (verbose) {
        for (int i = 0; i < elements.size(); i++) {
            // find min distance 
            if (getIndexOfClosestCentroid(centroids, elements[i], dimensionOfVectors) != elementToClusterIndex[elements[i]]) {
                cout << "Mismatch at elem " << i << "\n";
            }
        }
    }
    

    return clusters;
}

// int main() {
//     // assume we have vectors here
 //    vector<vector<double>> elems;
  //   int dim = 100;
  //   int numClusters = 5;
    // for (int i = 0; i < 100000; i++) {
      //   vector<double> addition(dim, 0);
        // for (int j = 0; j < dim; j++) {
          //   addition[j] = ((double) rand() / (RAND_MAX)); 
        // }
        // elems.push_back(addition);
//     }

//     runKMeans(elems, dim, numClusters, false, "kmeans_output.txt");
  //   return 0;
// }

