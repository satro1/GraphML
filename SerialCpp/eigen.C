#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "utils.h"

/****************************************************************
 * Find largest off-diag. element matrix[k,l]
 * k - pointer to double, will be filled with column of max value
 * l - pointer to double, will be filled with row of max value
 * n - size of matrix, where matrix is n*n
 ****************************************************************/
double maxElem(double** matrix, int* k, int* l, int n) {
    double max = 0.0;

    #pragma omp parallel for
    for (int i=0; i<n-1; i++) {
        for (int j=i+1; j<n; j++) {
            if (abs(matrix[i][j]) >= max) {
                max = abs(matrix[i][j]);
                *k = i;
                *l = j;
            }
        }
    }

    return max;
}

/****************************************************************
 * Rotate matrix to get eigenvalues and eigenvectors
 * idty - identity matrix of size n*n
 * k - pointer to double, will be filled with column of max value
 * l - pointer to double, will be filled with row of max value
 * n - size of matrix, where matrix is n*n
 ****************************************************************/
void rotate(double** matrix, double** p, int k, int l, int n) {
    double s, c, tau, temp, phi, t = 0.0;
    double aDiff = matrix[l][l] - matrix[k][k];

    if (abs(matrix[k][l]) < abs(aDiff)*1.0e-36) {
        t = matrix[k][l]/aDiff;
    } else {
        phi = aDiff/(2.0*matrix[k][l]);
        t = 1.0/(abs(phi) + sqrt(phi*phi + 1.0));
        t = (phi < 0)? -t : t;
    }

    c = 1.0/sqrt(t*t + 1.0);
    s = t*c;
    tau = s/(1.0 + c);
    temp = matrix[k][l];
    matrix[k][l] = 0.0;
    matrix[k][k] = matrix[k][k] - t*temp;
    matrix[l][l] = matrix[l][l] + t*temp;

    #pragma omp parallel for
    for (int i=0; i<k; i++) {      // Case of i < k
        temp = matrix[i][k];
        matrix[i][k] = temp - s*(matrix[i][l] + tau*temp);
        matrix[i][l] = matrix[i][l] + s*(temp - tau*matrix[i][l]);
    }

    #pragma omp parallel for
    for (int i=k+1; i<l; i++) {  // Case of k < i < l
        temp = matrix[k][i];
        matrix[k][i] = temp - s*(matrix[i][l] + tau*matrix[k][i]);
        matrix[i][l] = matrix[i][l] + s*(temp - tau*matrix[i][l]);
    }

    #pragma omp parallel for
    for (int i=l+1; i<n; i++) {  // Case of i > l
        temp = matrix[k][i];
        matrix[k][i] = temp - s*(matrix[l][i] + tau*temp);
        matrix[l][i] = matrix[l][i] + s*(temp - tau*matrix[l][i]);
    }

    #pragma omp parallel for
    for (int i=0; i<n; i++) {     // Update transformation matrix
        temp = p[i][k];
        p[i][k] = temp - s*(p[i][l] + tau*p[i][k]);
        p[i][l] = p[i][l] + s*(temp - tau*p[i][l]);
    }

}

/****************************************************************
 * Computes eigenvalues and eigenvectors
 * matrix - input matrix of size n*n, also output of eigenvectors.
 * evalues - array of size n, will be filled with eigenvalues
 * n - size of matrix, where matrix is n*n
 ****************************************************************/
double** eigen(double** matrix, double ** p, double* evalues, int n) {
    int maxRot = 10*n;       // Set limit on number of rotations
    double tol = 0.001; // 1e-4

    // Initialize transformation matrix
    #pragma omp parallel for
    for (int i=0; i<n; i++) { p[i][i] = 1.0; } // make diagonals 1
    
    int k = 0;
    int l = 0;
    double aMax = 0.0;


    for (int i=0; i<maxRot; i++) { // Jacobi rotation loop
        aMax = maxElem(matrix, &k, &l, n);
        if (aMax < tol) {
            printf("Converged!\n");
            break; 
        }
        rotate(matrix, p, k, l, n);
    }

    #pragma omp parallel for
    for (int i=0; i<n; i++) { evalues[i] = matrix[i][i]; } // make diagonals 1
    return p;
}