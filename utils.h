#ifndef __UTILS_H__
#define __UTILS_H__
#include <stdio.h>
#include <stdlib.h>

// Structure d'une cellule
typedef struct Cell {
    int dest;
    float prob;
    struct Cell *next;
} t_cell;

// Structure d'une liste head
typedef struct {
    t_cell *head;
} t_list;

// Structure d'une liste d'adjacence
typedef struct {
    t_list *adjLists;
    int nbSommets;
} t_list_adjacence;

//Lien entre deux sommets
typedef struct {
    int from;
    int to;
} t_link;

//Tableau dynamique de liens
typedef struct {
    t_link *links;
    int log_size;
    int phy_size;
} t_link_array;


t_cell* createCell(int dest, float prob);

t_list createEmptyList();

t_list_adjacence createAdjList(int nbSommets);

t_list_adjacence readGraph(const char *filename);

void addCell(t_list *list, int dest, float prob);

void printList(t_list list);

void freeList(t_list *list);

void printAdjList(t_list_adjacence adjList);

void freeAdjList(t_list_adjacence *adjList);

void isMarkovGraph(t_list_adjacence adjList);

void generateMermaidFile(t_list_adjacence adjList, const char *filename);


#endif