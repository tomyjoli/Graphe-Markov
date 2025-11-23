#include <stdio.h>
#include "utils.h"
#include "matrix.h"
#include "tarjan.h"
#include "hasse.h"
int main() {
    system("chcp 65001 > nul"); // Active l'UTF-8 pour afficher les accents

    const char *filenames[] = {
        "../data/exemple1.txt",
        "../data/exemple_valid_step3.txt",
        "../data/exemple_scc1.txt",
        "../data/exemple3.txt",
        "../data/exemple4_2check.txt"
    }; // On utilise ../ pour passer du répertoire de travail au répertoire du projet afin d'accéder au répertoire 'data'
    const char *meteoFile = "../data/exemple_meteo.txt";

    int numFiles = sizeof(filenames) / sizeof(filenames[0]);

    for (int i = 0; i < numFiles; i++) {
        printf("\nTraitement du fichier : %s\n", filenames[i]);

        // PARTIE 1
        t_list_adjacence adjList = readGraph(filenames[i]);
        printf("Liste d'adjacence du graphe :\n");
        printAdjList(adjList);

        isMarkovGraph(adjList);

        char outputFilename[256];
        snprintf(outputFilename, sizeof(outputFilename), "%s.mmd", filenames[i]);
        generateMermaidFile(adjList, outputFilename);
        printf("Fichier Mermaid généré: %s\n", outputFilename);

        // PARTIE 2
        t_partition partition;
        tarjanAlgo(adjList, &partition);

        printf("\nComposantes fortement connexes trouvées :\n");
        afficherPartition(partition);

        t_link_array tab_liens;
        construireLiensHasse(adjList, partition, &tab_liens);

        removeTransitiveLinks(&tab_liens);

        char hasseFilename[256];
        snprintf(hasseFilename, sizeof(hasseFilename), "%s_hasse.mmd", filenames[i]);
        genererMermaidHasse(partition, tab_liens, hasseFilename);
        printf("Diagramme de Hasse généré: %s\n", hasseFilename);

        analyserProprietes(partition, tab_liens);

        free(tab_liens.links);
        freeAdjList(&adjList);
    }
    printf("\n=== TEST DES MATRICES ===\n");

    printf("\n=== Matrice M pour l'exemple météo ===\n");

    t_list_adjacence meteoAdj = readGraph(meteoFile);
    Matrix *M = matrix_from_adjacency(&meteoAdj);

    printMatrix(M);

    printf("\n=== M^3 ===\n");
    Matrix *M3 = matrix_power(M, 3);
    printMatrix(M3);

    printf("\n=== M^7 ===\n");
    Matrix *M7 = matrix_power(M, 7);
    printMatrix(M7);

    printf("\n===== TEST DE CONVERGENCE POUR LES EXEMPLES =====\n");

    const double EPS = 0.01;

    for (int f = 0; f < numFiles; f++) {

        printf("\n--- Fichier : %s ---\n", filenames[f]);

        t_list_adjacence adj = readGraph(filenames[f]);
        Matrix *A = matrix_from_adjacency(&adj);

        Matrix *prev = matrix_copy(A);
        Matrix *curr = matrix_multiply(A, A);

        int n = 2;
        while (1) {
            double d = matrix_diff(curr, prev);

            if (d < EPS) {
                printf("Convergence atteinte pour n = %d (diff = %.4f)\n", n, d);
                break;
            }

            if (n > 200) {
                printf("Pas de convergence pour ce fichier (jusqu'à n = 200)\n");
                break;
            }

            // prochaine puissance
            Matrix *next = matrix_multiply(curr, A);

            // libérer prev
            for (int i = 0; i < prev->n; i++) free(prev->val[i]);
            free(prev->val);
            free(prev);

            prev = curr;
            curr = next;
            n++;
        }

        // libération mémoire
        for (int i = 0; i < prev->n; i++) free(prev->val[i]);
        free(prev->val);
        free(prev);
        for (int i = 0; i < curr->n; i++) free(curr->val[i]);
        free(curr->val);
        free(curr);

        freeAdjList(&adj);
    }
    freeAdjList(&meteoAdj);
    return 0;
}
