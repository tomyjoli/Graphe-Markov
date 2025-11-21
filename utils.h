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

//PARTIE 2

typedef struct s_tarjan_vertex{
    int id;
    int number;
    int nb_access;
    int flag;        //0 ou 1
}t_tarjan_vertex;

typedef struct s_classe{
    char nom_classe;
    t_tarjan_vertex *sommet;
    int nb_sommet;
    int taille_tab;  //allocation dynamique
}t_classe;

typedef struct s_partition{
    t_classe *classe;
    int nb_classe;
    int taille_classe;  //allocation dynamique
}t_partition;


t_tarjan_vertex graph_to_tab(int id, int num, int num_access, int flag);



#endif