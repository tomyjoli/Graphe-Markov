#include "matrix.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

Matrix *matrix_create_zero(int n) {
    if (n <= 0) return NULL; // dimension invalide

    // Allocation de la structure Matrix
    Matrix *m = malloc(sizeof(Matrix));
    if (!m) return NULL;

    m->n = n;

    // Allocation des lignes
    m->val = malloc(n * sizeof(double *));
    if (!m->val) {
        free(m);
        return NULL;
    }

    // Allocation des colonnes et initialisation à 0
    for (int i = 0; i < n; i++) {
        m->val[i] = calloc(n, sizeof(double)); // calloc initialise à 0
        if (!m->val[i]) {
            // En cas d'échec, libérer ce qui a déjà été alloué
            for (int k = 0; k < i; k++)
                free(m->val[k]);
            free(m->val);
            free(m);
            return NULL;
        }
    }

    return m;
}

Matrix *matrix_from_adjacency(const t_list_adjacence *adj) {
    if (!adj || adj->nbSommets <= 0) return NULL;

    int n = adj->nbSommets;
    Matrix *m = matrix_create_zero(n);
    if (!m) return NULL;

    for (int i = 0; i < n; i++) {
        t_cell *cell = adj->adjLists[i].head;
        while (cell) {
            int j = cell->dest;
            if (j >= 0 && j < n) {
                m->val[i][j] = cell->prob;
            }
            cell = cell->next;
        }
    }

    return m;
}

Matrix *matrix_copy(const Matrix *src) {
        if (!src) return NULL;

        Matrix *copy = matrix_create_zero(src->n);
        if (!copy) return NULL;

        for (int i = 0; i < src->n; i++) {
            for (int j = 0; j < src->n; j++) {
                copy->val[i][j] = src->val[i][j];
            }
        }
        return copy;
}

Matrix *matrix_multiply(const Matrix *A, const Matrix *B) {
        if (!A || !B || A->n != B->n) return NULL;

        int n = A->n;
        Matrix *C = matrix_create_zero(n);
        if (!C) return NULL;

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                for (int k = 0; k < n; k++) {
                    C->val[i][j] += A->val[i][k] * B->val[k][j];
                }
            }
        }
        return C;
}

double matrix_diff(const Matrix *M, const Matrix *N) {
        if (!M || !N || M->n != N->n) return -1.0;

        double diff = 0.0;
        int n = M->n;

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                diff += fabs(M->val[i][j] - N->val[i][j]);
            }
        }

        return diff;
    }