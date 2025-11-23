#include "matrix.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

Matrix *matrix_create_zero(int n) {

    // Allocation de la structure Matrix
    Matrix *m = malloc(sizeof(Matrix));

    m->n = n;

    // Allocation des lignes
    m->val = malloc(n * sizeof(double *));

    // Allocation des colonnes et initialisation à 0
    for (int i = 0; i < n; i++) {
        m->val[i] = malloc(n * sizeof(double));  // allocation simple
        for (int j = 0; j < n; j++) {
            m->val[i][j] = 0.0;
        }
    }

    return m;
}

Matrix *matrix_from_adjacency(const t_list_adjacence *adj) {

    int n = adj->nbSommets;
    Matrix *m = matrix_create_zero(n);

    for (int i = 0; i < n; i++) {
        t_cell *cell = adj->adjLists[i].head;
        while (cell) {
            int j = cell->dest - 1;
            if (j >= 0 && j < n) {
                m->val[i][j] = cell->prob;
            }
            cell = cell->next;
        }
    }

    return m;
}

Matrix *matrix_copy(const Matrix *src) {

        Matrix *copy = matrix_create_zero(src->n);

        for (int i = 0; i < src->n; i++) {
            for (int j = 0; j < src->n; j++) {
                copy->val[i][j] = src->val[i][j];
            }
        }
        return copy;
}

Matrix *matrix_multiply(const Matrix *A, const Matrix *B) {

        int n = A->n;
        Matrix *C = matrix_create_zero(n);

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

        double diff = 0.0;
        int n = M->n;

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                diff += fabs(M->val[i][j] - N->val[i][j]);
            }
        }

        return diff;
    }

Matrix* subMatrix(Matrix *matrix, t_partition part, int compo_index) {
    if (compo_index < 0 || compo_index >= part.nbClasse) {
        fprintf(stderr, "Erreur: indice de composante invalide\n");
        return NULL;
    }

    t_classe classe = part.classes[compo_index];
    int n = classe.nbSommets;

    if (n == 0) return NULL;  // composante vide

    Matrix *sub = matrix_create_zero(n);

    // Remplissage de la sous-matrice
    for (int i = 0; i < n; i++) {
        int orig_i = classe.sommets[i] - 1; // conversion 1-based -> 0-based
        for (int j = 0; j < n; j++) {
            int orig_j = classe.sommets[j] - 1;
            sub->val[i][j] = matrix->val[orig_i][orig_j];
        }
    }

    return sub;
}

void printMatrix(const Matrix *M) {
    for (int i = 0; i < M->n; i++) {
        for (int j = 0; j < M->n; j++)
            printf("%0.4f ", M->val[i][j]);
        printf("\n");
    }
}

Matrix* matrix_power(const Matrix *M, int p) {
    Matrix *result = matrix_copy(M);
    for (int k = 1; k < p; k++) {
        Matrix *tmp = matrix_multiply(result, M);
        // libération de l'ancienne matrice
        for (int i = 0; i < result->n; i++) free(result->val[i]);
        free(result->val);
        free(result);
        result = tmp;
    }
    return result;
}

