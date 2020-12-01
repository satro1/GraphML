#include <vector>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <fstream>

using namespace std;

#define NUM_CLUSTERS 5
#define DIM 2
#include <math.h>

/**
 * Method to find random centroisd initially.
 * Goes through all the elements and 
 * */
vector<vector<double>> getRandomCentroids(vector<vector<double>> nodes, int dim) {
    vector<double> min(dim, 0);
    vector<double> max(dim, 0);

    for (int i = 0; i < dim; i++) {
        min[i] = nodes[0][i];
        max[i] = nodes[0][i];
    }

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

    srand(0);

    vector<vector<double>> ret(NUM_CLUSTERS, vector<double>(dim, 0));
    for (int i = 0; i < NUM_CLUSTERS; i++) {
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

    for (int i = 0; i < cluster.size(); i++) {
        numElem++;
        for (int j = 0; j < dim; j++) {
            mean[j] += cluster[i][j];
        }
    }

    for (int i = 0; i < dim; i++) {
        mean[i] /= numElem;
    }

    return mean;
}

double distanceBetweenVectors(vector<double> a, vector<double> b, int dim) {
    double dist = 0;
    
    for (int i = 0; i < dim; i++) {
        dist += pow((a[i] - b[i]), 2);
    }

    return pow(dist, 0.5);
}

int getIndexOfClosestCentroid(vector<vector<double>> centroids, vector<double> element, int dim) {
    int indexOfClosest = -1;
    double minDistance = 0;
    for (int i = 0; i < centroids.size(); i++) {


        double tempDistance = distanceBetweenVectors(centroids[i], element, dim);
        if (tempDistance < minDistance || indexOfClosest == -1) {
            minDistance = tempDistance;
            indexOfClosest = i;
        }
    }

    return indexOfClosest;
}

int main() {
    // assume we have vectors here
    // vector<double> v1 {0, 0};
    // vector<double> v2 {1, 1};
    // vector<double> v3 {0.1, 0.1};
    // vector<double> v4 {0, .5};
    // vector<double> v5 {.2, 1};
    // vector<vector<double>> elems {v1, v2, v3, v4, v5};
    vector<vector<double>> elems;

    for (int i = 0; i < 1000; i++) {
        vector<double> addition(DIM, 0);
        for (int j = 0; j < DIM; j++) {
            addition[j] = ((double) rand() / (RAND_MAX)); 
        }
        elems.push_back(addition);
    }


    // generate random centroids here
    vector<vector<double>> centroids = getRandomCentroids(elems, DIM);
    cout << "Random centroids: \n";
    for (int i = 0; i < centroids.size(); i++) {
        for (auto j = centroids[i].begin(); j != centroids[i].end(); ++j)
            cout << *j << ' ';
        cout << "\n";
    }


    // create the clusters now

    vector<vector<vector<double>>> clusters(NUM_CLUSTERS, vector<vector<double>>(elems.size(), vector<double>(DIM, 0)));
    vector<int> sizes(NUM_CLUSTERS, 0);
    map<vector<double>, int> elementToClusterIndex;     // map(vector<double> => int index of cluster in vector)

    // place the elements into their initial clusters
    for (int i = 0; i < elems.size(); i++) {
        // find which centroid its closest to
        int indexOfClosestCentroid = getIndexOfClosestCentroid(centroids, elems[i], DIM);
        clusters[indexOfClosestCentroid][sizes[indexOfClosestCentroid]++] = elems[i];
        elementToClusterIndex.insert(pair<vector<double>, int>(elems[i], indexOfClosestCentroid));

        cout << "Element " << i << " was found to be closest to index " << indexOfClosestCentroid;
        cout << "\nELEMENT: ";
        for (int j = 0; j < DIM; j++) {
            cout << elems[i][j] << ",";
        }
        cout << "\nThis index is mean ";
        for (int j = 0; j < DIM; j++) {
            cout << centroids[indexOfClosestCentroid][j] << ",";
        }
        cout << "with a distance of " << distanceBetweenVectors(centroids[indexOfClosestCentroid], elems[i], DIM) << "\n";
    }

    bool convergence = false;
    while (!convergence) {
        cout << "Iteration\n";
        convergence = true;
        // regenerate the centroids so they are the new means
        for (int i = 0; i < NUM_CLUSTERS; i++) {
            centroids[i] = getMean(clusters[i], DIM);
        }
        // reset sizes
        for (int i = 0; i < sizes.size(); i++) {
            sizes[i] = 0;
        }

        // loop through every element and put them their new respective cluster
        for (int i = 0; i < elems.size(); i++) {
            int newIndex = getIndexOfClosestCentroid(centroids, elems[i], DIM);
            int oldIndex = elementToClusterIndex[elems[i]];
            if (newIndex != oldIndex) {
                // separate
                convergence = false;
            }

            // either way we add the element in to the "new" cluster
            clusters[newIndex][sizes[newIndex]++] = elems[i];
            // elementToClusterIndex.insert(pair<vector<double>, int>(elems[i], newIndex));
            elementToClusterIndex[elems[i]] = newIndex;
        }
    }

    cout << "Converged!\n";
    // now we want to print the clusters
    for (int i = 0; i < NUM_CLUSTERS; i++) {
        cout << "Cluster " << i << "\n";
        for (int j = 0; j < sizes[i]; j++) {
            for (auto k = clusters[i][j].begin(); k != clusters[i][j].end(); ++k)
                cout << *k << ' ';
            cout << '\n';
        }
    }

    // write the clusters to a file
    ofstream output;
    output.open("output.txt");

    output << "x,y,cluster\n";
  
    for (int i = 0; i < NUM_CLUSTERS; i++) {
        for (int j = 0; j < sizes[i]; j++) {
            for (auto k = clusters[i][j].begin(); k != clusters[i][j].end(); ++k)
                output << *k << ',';
            output << i << '\n';
        }
    }  

    output.close();


    // test to see this map makes sense
    for (int i = 0; i < elems.size(); i++) {
        // find min distance 
        if (getIndexOfClosestCentroid(centroids, elems[i], DIM) != elementToClusterIndex[elems[i]]) {
            cout << "Mismatch at elem " << i << "\n";
        }
    }



    return 0;
}

