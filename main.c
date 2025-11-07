#include <stdio.h>
#include "utils.h"

void processFile(const char *filename) {
    printf("\n=== Traitement du fichier : %s ===\n", filename);

    t_list_adjacence adjList = readGraph(filename);
    printf("Liste d'adjacence du graphe :\n");
    printAdjacencyList(adjList);

    if (isMarkovGraph(adjList)) {
        printf("Le graphe est un graphe de Markov valide.\n");
    } else {
        printf("Le graphe n'est PAS un graphe de Markov.\n");
    }

    char outputFilename[256];
    snprintf(outputFilename, sizeof(outputFilename), "%s.mmd", filename);
    generateMermaidFile(adjList, outputFilename);
    printf("Fichier Mermaid généré : %s\n", outputFilename);

    freeAdjacencyList(&adjList);
}



int main() {
    const char *filenames[] = {
        "../data/exemple1.txt",
        "../data/exemple_valid_step3.txt",
        "../data/exemple_scc1.txt",
        "../data/exemple3.txt",
        "../data/exemple4_2check.txt"
    }; // On utilise ../ pour passer du répertoire de travail au répertoire du projet afin d'accéder au répertoire 'data'
    int numFiles = sizeof(filenames) / sizeof(filenames[0]);

    for (int i = 0; i < numFiles; i++) {
        processFile(filenames[i]);
    }

    return 0;
}
