#include <stdio.h>
#include "utils.h"





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

        isMarkovGraph(adjList);

        char outputFilename[256];
        snprintf(outputFilename, sizeof(outputFilename), "%s.mmd", filenames[i]);
        generateMermaidFile(adjList, outputFilename);
        printf("Fichier Mermaid généré: %s\n", outputFilename);

        freeAdjList(&adjList);
    }
}
