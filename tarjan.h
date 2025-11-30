#ifndef TARJAN_H
#define TARJAN_H
#include "utils.h"

// Étape 1
typedef struct {
    int id;
    int numero;
    int nbAcces;
    int booleen;
} t_tarjan_vertex;

typedef struct s_classe {
    char nom_classe[10];
    int *sommets;
    int nbSommets;
    int capaciteMax;
} t_classe;

typedef struct s_partition {
    t_classe *classes;
    int nbClasse;
    int tailleClasse;
} t_partition;


// Étape 2

typedef struct {
    t_tarjan_vertex **tab;
    int sommet;
    int capacite;
} t_pile;

t_tarjan_vertex* initialiserTarjan(t_list_adjacence graphe);
t_pile creerPile(int capacite);
void push(t_pile *p, t_tarjan_vertex *v);
t_tarjan_vertex* pop(t_pile *p);
int estVide(t_pile p);


void parcoursTarjan(int index, t_list_adjacence graphe, t_tarjan_vertex *tab, t_pile *pile, t_partition *partition, int *compteur);
void tarjanAlgo(t_list_adjacence graphe, t_partition *partition);
void afficherPartition(t_partition partition);
void ajouterClassePartition(t_partition *p, t_classe *nouvelleClasse);

void remplirAppartenance(int *sommet2classe, t_partition partition, int n);
int lienExiste(t_link_array *tab_liens, int from, int to);
void ajouterLienUnique(t_link_array *tab_liens, int from, int to);
void construireLiensHasse(t_list_adjacence graphe, t_partition partition, t_link_array *tab_liens);
void genererMermaidHasse(t_partition partition, t_link_array tab_liens, const char *filename);

void analyserProprietes(t_partition partition, t_link_array tab_liens);
#endif //TARJAN_H
