#ifndef MATRIX_H
#define MATRIX_H
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

typedef struct {
    int n;          // dimension (n x n)
    double **val;   // tableau 2D : val[i][j]
} Matrix;

Matrix *matrix_from_adjacency(const t_list_adjacence *adj);

Matrix *matrix_create_zero(int n);

Matrix *matrix_copy(const Matrix *src);

Matrix *matrix_multiply(const Matrix *A, const Matrix *B);

double matrix_diff(const Matrix *M, const Matrix *N);

//Fonctions non demandé mais utile pour les validations

void printMatrix(const Matrix *M);

Matrix* matrix_power(const Matrix *M, int p);

//Fonction du bonus

int gcd(int *vals, int nbvals);

int getPeriodMatrix(Matrix *sub);

#endif //MATRIX_H
