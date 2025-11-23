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

    // Lecture du graphe
    t_list_adjacence adjList = readGraph(filenames[i]);
    printf("Liste d'adjacence du graphe :\n");
    printAdjList(adjList);

    isMarkovGraph(adjList);

    char outputFilename[256];
    snprintf(outputFilename, sizeof(outputFilename), "%s.mmd", filenames[i]);
    generateMermaidFile(adjList, outputFilename);
    printf("Fichier Mermaid généré: %s\n", outputFilename);

    // Composantes fortement connexes
    t_partition partition;
    tarjanAlgo(adjList, &partition);
    printf("\nComposantes fortement connexes trouvées :\n");
    afficherPartition(partition);

    // Hasse
    t_link_array tab_liens;
    construireLiensHasse(adjList, partition, &tab_liens);
    removeTransitiveLinks(&tab_liens);

    char hasseFilename[256];
    snprintf(hasseFilename, sizeof(hasseFilename), "%s_hasse.mmd", filenames[i]);
    genererMermaidHasse(partition, tab_liens, hasseFilename);
    printf("Diagramme de Hasse généré: %s\n", hasseFilename);

    analyserProprietes(partition, tab_liens);

    // Distribution stationnaire etape 2 et 3 partie 3 utilisation de l'ia pour intégrer dans le main
    printf("\n=== Distributions stationnaires par composante ===\n");
    Matrix *A = matrix_from_adjacency(&adjList);

    for (int c = 0; c < partition.nbClasse; c++) {
        printf("\n--- Composante %s ---\n", partition.classes[c].nom_classe);

        Matrix *sub = subMatrix(A, partition, c);
        if (!sub || sub->n == 0) { free(sub); continue; }

        // Calcul de la période (etape 3 partie 3)
        int period = getPeriodMatrix(sub);
        printf("Période de la composante: %d\n", period);

        // Distribution stationnaire (approximation)
        Matrix *subStationnaire;
        if (period == 1) {
            // convergence normale
            subStationnaire = matrix_power(sub, 50);
        } else {
            // cas périodique : moyenne sur les puissances jusqu'à period
            subStationnaire = matrix_create_zero(sub->n);
            for (int k = 0; k < period; k++) {
                Matrix *tmp = matrix_power(sub, k+1);
                for (int i = 0; i < sub->n; i++)
                    for (int j = 0; j < sub->n; j++)
                        subStationnaire->val[i][j] += tmp->val[i][j] / period;

                // libération tmp
                for (int i = 0; i < tmp->n; i++) free(tmp->val[i]);
                free(tmp->val); free(tmp);
            }
        }
        printf("Matrice après convergence :\n");
        printMatrix(subStationnaire);

        // Libération mémoire sous-matrices
        for (int j = 0; j < sub->n; j++) free(sub->val[j]);
        free(sub->val); free(sub);

        for (int j = 0; j < subStationnaire->n; j++) free(subStationnaire->val[j]);
        free(subStationnaire->val); free(subStationnaire);
    }

    // Libération matrice globale
    for (int j = 0; j < A->n; j++) free(A->val[j]);
    free(A->val); free(A);

    // Libération Hasse et graphe
    free(tab_liens.links);
    freeAdjList(&adjList);
}
    //Etape 1 partie 3
    printf("\nTEST DES MATRICES\n");

    printf("\nMatrice M pour l'exemple météo\n");

    t_list_adjacence meteoAdj = readGraph(meteoFile);
    Matrix *M = matrix_from_adjacency(&meteoAdj);

    printMatrix(M);

    printf("\nM^3\n");
    Matrix *M3 = matrix_power(M, 3);
    printMatrix(M3);

    printf("\nM^7\n");
    Matrix *M7 = matrix_power(M, 7);
    printMatrix(M7);

    printf("\nTEST DE CONVERGENCE POUR LES EXEMPLES\n");

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
            prev = curr;
            curr = next;
            n++;
        }

        freeAdjList(&adj);
    }
    freeAdjList(&meteoAdj);

    //Défi 2 : exemple_defi.txt
    printf("\nDéfi 2 : Calcul des périodes et distributions stationnaires\n");
    const char *defiFile = "../data/exemple_defi.txt";
    t_list_adjacence defiAdj = readGraph(defiFile);
    printf("Liste d'adjacence du graphe :\n");
    printAdjList(defiAdj);

    t_partition defiPartition;
    tarjanAlgo(defiAdj, &defiPartition);
    printf("\nComposantes fortement connexes trouvées :\n");
    afficherPartition(defiPartition);

    Matrix *A_defi = matrix_from_adjacency(&defiAdj);
    for (int c = 0; c < defiPartition.nbClasse; c++) {
        printf("\n--- Composante %s ---\n", defiPartition.classes[c].nom_classe);

        Matrix *sub = subMatrix(A_defi, defiPartition, c);
        if (!sub || sub->n == 0) { free(sub); continue; }

        int period = getPeriodMatrix(sub);
        printf("Période de la composante : %d\n", period);

        Matrix *subStationnaire;
        if (period == 1) {
            subStationnaire = matrix_power(sub, 50);
        } else {
            subStationnaire = matrix_create_zero(sub->n);
            for (int k = 0; k < period; k++) {
                Matrix *tmp = matrix_power(sub, k+1);
                for (int i = 0; i < sub->n; i++)
                    for (int j = 0; j < sub->n; j++)
                        subStationnaire->val[i][j] += tmp->val[i][j] / period;

                for (int i = 0; i < tmp->n; i++) free(tmp->val[i]);
                free(tmp->val); free(tmp);
            }
        }
        printf("Matrice après convergence :\n");
        printMatrix(subStationnaire);

        for (int j = 0; j < sub->n; j++) free(sub->val[j]);
        free(sub->val); free(sub);

        for (int j = 0; j < subStationnaire->n; j++) free(subStationnaire->val[j]);
        free(subStationnaire->val); free(subStationnaire);
    }
    for (int j = 0; j < A_defi->n; j++) free(A_defi->val[j]);
    free(A_defi->val); free(A_defi);
    freeAdjList(&defiAdj);

    // au niveau du défi 2 il y'a un soucis et le sommet 8 ne semble pas être détecté alors qu'il serait que la période de sa composante serait également 3 j'ai essayé de demandé à chat mais ça n'a pas vrm fonctionné

    return 0;
}
