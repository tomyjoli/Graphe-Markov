#include <stdio.h>
#include "utils.h"
#include "matrix.h"

int main() {
    system("chcp 65001 > nul"); // Active l'UTF-8 pour afficher les accents

    const char *filenames[] = {
        "../data/exemple1.txt",
        "../data/exemple_valid_step3.txt",
        "../data/exemple_scc1.txt",
        "../data/exemple3.txt",
        "../data/exemple4_2check.txt"
    }; // On utilise ../ pour passer du répertoire de travail au répertoire du projet afin d'accéder au répertoire 'data'
    int numFiles = sizeof(filenames) / sizeof(filenames[0]);

    for (int i = 0; i < numFiles; i++) {
        printf("\nTraitement du fichier : %s\n", filenames[i]);

        t_list_adjacence adjList = readGraph(filenames[i]);
        printf("Liste d'adjacence du graphe :\n");
        printAdjList(adjList);

        t_partition partition;
        tarjan_algo(adjList, &partition);
        afficher_partition(partition);

        char hasseFilename[256];
        snprintf(hasseFilename, sizeof(hasseFilename), "%s_hasse.mmd", filenames[i]);
        generer_diagramme_hasse(adjList, partition, hasseFilename);
        liberer_partition(&partition);

        isMarkovGraph(adjList);

        char outputFilename[256];
        snprintf(outputFilename, sizeof(outputFilename), "%s.mmd", filenames[i]);
        generateMermaidFile(adjList, outputFilename);
        printf("Fichier Mermaid généré: %s\n", outputFilename);

        freeAdjList(&adjList);
    }
    printf("\n=== TEST DES MATRICES ===\n");

    // Création d'un petit graphe de 3 sommets
    t_list_adjacence testGraph;
    testGraph.nbSommets = 3;
    testGraph.adjLists = malloc(3 * sizeof(t_list));
    for (int i = 0; i < 3; i++) {
        testGraph.adjLists[i].head = NULL;
    }

    // Ajout de quelques arcs avec probabilités
    t_cell *c1 = malloc(sizeof(t_cell)); c1->dest = 1; c1->prob = 0.5; c1->next = NULL;
    t_cell *c2 = malloc(sizeof(t_cell)); c2->dest = 2; c2->prob = 0.5; c2->next = NULL;
    testGraph.adjLists[0].head = c1; c1->next = c2;

    t_cell *c3 = malloc(sizeof(t_cell)); c3->dest = 0; c3->prob = 1.0; c3->next = NULL;
    testGraph.adjLists[1].head = c3;

    t_cell *c4 = malloc(sizeof(t_cell)); c4->dest = 2; c4->prob = 1.0; c4->next = NULL;
    testGraph.adjLists[2].head = c4;

    // Création de la matrice de probabilités
    Matrix *mat = matrix_from_adjacency(&testGraph);
    printf("Matrice de probabilités :\n");
    for (int i = 0; i < mat->n; i++) {
        for (int j = 0; j < mat->n; j++)
            printf("%0.2f ", mat->val[i][j]);
        printf("\n");
    }

    // Test de la copie
    Matrix *copy = matrix_copy(mat);
    printf("\nMatrice copiée :\n");
    for (int i = 0; i < copy->n; i++) {
        for (int j = 0; j < copy->n; j++)
            printf("%0.2f ", copy->val[i][j]);
        printf("\n");
    }

    // Test de multiplication
    Matrix *mult = matrix_multiply(mat, copy);
    printf("\nMatrice multipliée (mat * copy) :\n");
    for (int i = 0; i < mult->n; i++) {
        for (int j = 0; j < mult->n; j++)
            printf("%0.2f ", mult->val[i][j]);
        printf("\n");
    }

    // Test de différence
    double diff = matrix_diff(mat, copy);
    printf("\nDifférence entre mat et copy : %0.2f\n", diff);
}


//printf("\nFin du programme. Appuyez sur Entree pour quitter.\n");
//getchar(); // Pause pour voir le résultat si lancé hors terminal
