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

t_list_adjacence createAdjList(int nbSommets) {
    t_list_adjacence adjList;
    adjList.nbSommets = nbSommets;
    adjList.adjLists = (t_list*)malloc(nbSommets * sizeof(t_list));

    for (int i = 0; i < nbSommets; i++) {
        adjList.adjLists[i] = createEmptyList();
    }
    return adjList;
}

void printAdjList(t_list_adjacence adjList) {
    for (int i = 0; i < adjList.nbSommets; i++) {
        printf("Sommet %d:", i + 1);
        printList(adjList.adjLists[i]);
    }
}

void freeAdjList(t_list_adjacence *adjList) {
    for (int i = 0; i < adjList->nbSommets; i++) {
        freeList(&adjList->adjLists[i]);
    }
    free(adjList->adjLists);
    adjList->nbSommets = 0;
}

t_list_adjacence readGraph(const char *filename) {
    FILE *file = fopen(filename, "r");
    t_list_adjacence adjList;

    if (file == NULL) {
        perror("Erreur à l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    int nbSommets;
    if (fscanf(file, "%d", &nbSommets) != 1) {
        perror("Erreur de lecture du nombre de sommets");
        exit(EXIT_FAILURE);
    }

    adjList = createAdjList(nbSommets);

    int depart, arrivee;
    float prob;

    while (fscanf(file, "%d %d %f", &depart, &arrivee, &prob) == 3) {
        addCell(&adjList.adjLists[depart - 1], arrivee, prob);
    }

    fclose(file);
    return adjList;
}

void isMarkovGraph(t_list_adjacence adjList) {
    for (int i = 0; i < adjList.nbSommets; i++) {
        float somme = 0.0f;
        t_cell *cur = adjList.adjLists[i].head;

        while (cur != NULL) {
            somme += cur->prob;
            cur = cur->next;
        }

        if (somme < 0.99f || somme > 1.01f) {  // Tolérance pour les flottants parce qu'on est sympa :)
            printf("Le graphe n'est pas un graphe de Markov car la somme des probabilités pour le sommet %d = %.2f (attendu : 1.00)\n", i + 1, somme);
            return;
        }
    }
    printf("Le graphe est un graphe de Markov \n");
}

void generateMermaidFile(t_list_adjacence adjList, const char *filename) {
    FILE *file = fopen(filename, "w");

    fprintf(file, "---\n    layout: elk\n    theme: neo\n    look: neo\n---\n\nflowchart LR\n");

    for (int i = 0; i < adjList.nbSommets; i++) {
        char id[10];
        strcpy(id, getID(i+1));
        fprintf(file, "%s((%d))\n", id, i+1);
    }
    fprintf(file, "\n");

    for (int i = 0; i < adjList.nbSommets; i++) {
        t_cell *current = adjList.adjLists[i].head;
        while (current != NULL) {
            char src[10], dest[10];
            strcpy(src, getID(i+1));
            strcpy(dest, getID(current->dest));
            fprintf(file, "    %s -->|%.2f| %s\n", src, current->prob, dest);
            current = current->next;
        }
    }

    fclose(file);
}