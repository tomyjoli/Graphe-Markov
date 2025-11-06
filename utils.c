#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

static char *getID(int i)
{
    // translate from 1,2,3, .. ,500+ to A,B,C,..,Z,AA,AB,...
    static char buffer[10];
    char temp[10];
    int index = 0;

    i--; // Adjust to 0-based index
    while (i >= 0)
    {
        temp[index++] = 'A' + (i % 26);
        i = (i / 26) - 1;
    }

    // Reverse the string to get the correct order
    for (int j = 0; j < index; j++)
    {
        buffer[j] = temp[index - j - 1];
    }
    buffer[index] = '\0';

    return buffer;
}

t_cell* createCell(int dest, float prob) {
    t_cell *newCell = (t_cell*)malloc(sizeof(t_cell));
    if (newCell == NULL) {
        perror("Erreur d'allocation pour une cellule");
        exit(EXIT_FAILURE);
    }
    newCell->dest = dest;
    newCell->prob = prob;
    newCell->next = NULL;
    return newCell;
}

t_list createEmptyList() {
    t_list list;
    list.head = NULL;
    return list;
}

void addCell(t_list *list, int dest, float prob) {
    t_cell *newCell = createCell(dest, prob);
    newCell->next = list->head;
    list->head = newCell;
}

void printList(t_list list) {
    t_cell *current = list.head;
    while (current != NULL) {
        printf(" -> (%d, %.2f)", current->dest, current->prob);
        current = current->next;
    }
    printf("\n");
}

void freeList(t_list *list) {
    t_cell *current = list->head;
    while (current != NULL) {
        t_cell *temp = current;
        current = current->next;
        free(temp);
    }
    list->head = NULL;
}

t_list_adjacence createAdjacencyList(int nbSommets) {
    t_list_adjacence adjList;
    adjList.nbSommets = nbSommets;
    adjList.adjLists = (t_list*)malloc(nbSommets * sizeof(t_list));
    if (adjList.adjLists == NULL) {
        perror("Erreur d'allocation pour la liste d'adjacence");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < nbSommets; i++) {
        adjList.adjLists[i] = createEmptyList();
    }
    return adjList;
}

void printAdjacencyList(t_list_adjacence adjList) {
    for (int i = 0; i < adjList.nbSommets; i++) {
        printf("Sommet %d:", i + 1);
        printList(adjList.adjLists[i]);
    }
}

void freeAdjacencyList(t_list_adjacence *adjList) {
    for (int i = 0; i < adjList->nbSommets; i++) {
        freeList(&adjList->adjLists[i]);
    }
    free(adjList->adjLists);
    adjList->nbSommets = 0;
}

t_list_adjacence readGraph(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Erreur à l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    int nbSommets;
    if (fscanf(file, "%d", &nbSommets) != 1) {
        perror("Erreur de lecture du nombre de sommets");
        exit(EXIT_FAILURE);
    }

    t_list_adjacence adjList = createAdjacencyList(nbSommets);

    int from, to;
    float prob;
    while (fscanf(file, "%d %d %f", &from, &to, &prob) == 3) {
        addCell(&adjList.adjLists[from - 1], to, prob);
    }

    fclose(file);
    return adjList;
}

int isMarkovGraph(t_list_adjacence adjList) {
    for (int i = 0; i < adjList.nbSommets; i++) {
        float sum = 0.0f;
        t_cell *current = adjList.adjLists[i].head;
        while (current != NULL) {
            sum += current->prob;
            current = current->next;
        }
        if (sum < 0.99f || sum > 1.01f) {  // Tolérance pour les flottants parce qu'on est sympa :)
            printf("Erreur : Somme des probabilités pour le sommet %d = %.2f (attendu : 1.00)\n", i + 1, sum);
            return 0;
        }
    }
    return 1;
}

void generateMermaidFile(t_list_adjacence adjList, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur à la création du fichier Mermaid");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "graph LR\n");
    for (int i = 0; i < adjList.nbSommets; i++) {
        t_cell *current = adjList.adjLists[i].head;
        while (current != NULL) {
            fprintf(file, "    %d -->|%.2f| %d\n", i + 1, current->prob, current->dest);
            current = current->next;
        }
    }

    fclose(file);
}
