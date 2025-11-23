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
    char nom_classe[10];
    t_tarjan_vertex **sommets;
    int nb_sommets;
    int capacite_max;
}t_classe;

typedef struct s_partition{
    t_classe *classes;
    int nb_classe;
    int taille_classe;  //allocation dynamique
}t_partition;

// Crée et initialise le tableau des sommets pour Tarjan
t_tarjan_vertex *initialiser_tarjan_vertices(t_list_adjacence graphe);


// Étape 2.2 : Structure de la pile (modifiée selon ta demande)
typedef struct {
    t_tarjan_vertex **tab; // Tableau de pointeurs
    int premier;           // Indice du sommet de pile (top)
    int capacite;          // Capacité de la pile
} t_pile;


// Prototypes des fonctions utilitaires (Noms en français)
t_tarjan_vertex* creer_tableau_tarjan(t_list_adjacence graphe);
t_pile creer_pile(int capacite);
void empiler(t_pile *p, t_tarjan_vertex *v);
t_tarjan_vertex* depiler(t_pile *p);
int pile_est_vide(t_pile p);


// Étape 3 : Prototypes pour l'algorithme de Tarjan
void tarjan_algo(t_list_adjacence graphe, t_partition *partition);
void afficher_partition(t_partition partition);


// Prototypes pour Hasse et Propriétés
void generer_diagramme_hasse(t_list_adjacence graphe, t_partition partition, const char *nom_fichier);

void liberer_partition(t_partition *partition);

void ajouter_lien_unique(t_link_array *tab_liens, int de, int vers);

int est_classe_absorbante(t_classe classe, t_list_adjacence graphe,t_partition partition);

int est_classe_transitoire(t_classe classe, t_list_adjacence graphe, t_partition partition);

int est_graphe_irreductible(t_partition partition);

void parcours(int u_idx, t_list_adjacence graphe, t_tarjan_vertex *tab_sommets,
              t_pile *p_pile, t_partition *partition, int *temps);

#endif