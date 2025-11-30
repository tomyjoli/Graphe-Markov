#include "tarjan.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Étape 2.1 : Initialise le tableau de sommets pour Tarjan
t_tarjan_vertex* initialiserTarjan(t_list_adjacence graphe) {
    int i, n = graphe.nbSommets;
    t_tarjan_vertex *tab = (t_tarjan_vertex*)malloc(n * sizeof(t_tarjan_vertex));

    if (tab == NULL) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; i++) {
        tab[i].id = i + 1; // A partir de 1
        tab[i].numero = -1;
        tab[i].nbAcces = -1;
        tab[i].booleen = 0; // Pas dans la pile
    }
    return tab;
}

// Étape 2.2 : Fonctions de gestion de la pile
t_pile creerPile(int capacite) {
    t_pile p;
    p.capacite = capacite;
    p.sommet = -1; // Pile vide
    p.tab = malloc(capacite * sizeof(t_tarjan_vertex*));

    if (p.tab == NULL) {
        perror("Erreur d'allocation de mémoire");
        exit(EXIT_FAILURE);
    }

    return p;
}

// Ajoute un sommet au sommet de la pile
void push(t_pile *p, t_tarjan_vertex *v) {
    if (p->sommet+1 >= p->capacite) {
        p->capacite *= 2;
        p->tab = realloc(p->tab, p->capacite * sizeof(t_tarjan_vertex*));
        if(p->tab == NULL) {
            perror("Erreur lors de l'agrandissement de la pile");
            exit(EXIT_FAILURE);
        }
    }
    p->tab[++(p->sommet)] = v;
}

// Retire et renvoie le sommet au sommet de la pile
t_tarjan_vertex* pop(t_pile *p) {
    if (p->sommet >= 0) {
        return p->tab[(p->sommet)--];
    }
    return NULL;
}

// Vérifie si la pile est vide (retourne 1 si vide, 0 sinon)
int estVide(t_pile p) {
    return (p.sommet == -1);
}


// Ajoute une nouvelle classe vide à la partition
void ajouterClassePartition(t_partition *p, t_classe *nouvelleClasse) {
    // Si le tableau de classes est plein, on l'agrandit
    if (p->nbClasse >= p->tailleClasse) {
        p->tailleClasse *= 2;
        p->classes = realloc(p->classes, p->tailleClasse * sizeof(t_classe));
    }

    p->classes[p->nbClasse++] = *nouvelleClasse;

}

// Ajoute un sommet à la dernière classe créée
void ajouterSommetClasse(t_classe *c, int id) {
    // Si la classe est pleine, on l'agrandit
    if (c->nbSommets >= c->capaciteMax) {
        c->capaciteMax *= 2;
        c->sommets = realloc(c->sommets, c->capaciteMax * sizeof(int));
    }

    c->sommets[c->nbSommets++] = id;
}



// Paramètres : indice du sommet v, graphe, tableau des infos, pile, partition, pointeur vers temps
void parcoursTarjan(int index, t_list_adjacence graphe, t_tarjan_vertex *tab, t_pile *pile, t_partition *partition, int *compteur) {
    t_tarjan_vertex *v = &tab[index];
    v->numero = *compteur;
    v->nbAcces = *compteur;
    (*compteur)++;

    push(pile, v);
    v->booleen = 1; // dans la pile

    t_cell *courant = graphe.adjLists[index].head;
    while (courant != NULL) {
        int indexCourant = courant->dest - 1;
        t_tarjan_vertex *w = &tab[indexCourant];

        if (w->numero == -1) {
            parcoursTarjan(indexCourant, graphe, tab, pile, partition, compteur);
            if (w->nbAcces < v->nbAcces) v->nbAcces = w->nbAcces;
        } else if (w->booleen == 1) {
            if (w->numero < v->nbAcces) v->nbAcces = w->numero;
        }
        courant = courant->next;
    }

    if (v->nbAcces == v->numero) {
        t_classe nouvelleClasse;
        snprintf(nouvelleClasse.nom_classe, sizeof(nouvelleClasse.nom_classe), "C%d", partition->nbClasse + 1);
        nouvelleClasse.nbSommets = 0;
        nouvelleClasse.capaciteMax = 4;
        nouvelleClasse.sommets = malloc(nouvelleClasse.capaciteMax * sizeof(int));
        if (!nouvelleClasse.sommets) { perror("malloc"); exit(EXIT_FAILURE); }

        t_tarjan_vertex *w;
        do {
            w = pop(pile);
            w->booleen = 0;
            ajouterSommetClasse(&nouvelleClasse, w->id);
        } while (w != v);

        ajouterClassePartition(partition, &nouvelleClasse);
    }
}


// Étape 3.2 : Fonction principale tarjan
void tarjanAlgo(t_list_adjacence graphe, t_partition *partition) {
    t_tarjan_vertex *tab = initialiserTarjan(graphe);
    int n = graphe.nbSommets;
    t_pile pile = creerPile(n);
    int compteur = 0;

    // Initialiser la partition
    partition->nbClasse = 0;
    partition->tailleClasse = 10;
    partition->classes = malloc(partition->tailleClasse * sizeof(t_classe));

    // Lancer le parcours pour chaque sommet non visité
    for (int i = 0; i < n; i++) {
        if (tab[i].numero == -1) {
            parcoursTarjan(i, graphe, tab, &pile, partition, &compteur);
        }
    }

    free(pile.tab);
    free(tab);
}

// Afficher la partition
void afficherPartition(t_partition partition) {
    for (int i = 0; i < partition.nbClasse; i++) {
        printf("Composante %s: {", partition.classes[i].nom_classe);
        for (int j = 0; j < partition.classes[i].nbSommets; j++) {
            printf("%d", partition.classes[i].sommets[j]);
            if (j < partition.classes[i].nbSommets - 1)
                printf(",");
        }
        printf("}\n");
    }
}



// Etape 2 - Diagramme de Hasse

// Remplire sommet2classe[i] = indice de la classe du sommet
void remplirAppartenance(int *sommet2classe, t_partition partition, int n) {
    for (int i = 0; i < partition.nbClasse; i++) {
        for (int j = 0; j < partition.classes[i].nbSommets; j++) {
            int sommet = partition.classes[i].sommets[j]; // Sommet 1-based (ex: 1,2,3..)
            sommet2classe[sommet - 1] = i; // Stocke l'indice de classe
        }
    }
}

// Vérifier si le lien (from -> to) existe déjà dans le t_link_array
int lienExiste(t_link_array *tab_liens, int from, int to) {
    for (int i = 0; i < tab_liens->log_size; i++) {
        if (tab_liens->links[i].from == from && tab_liens->links[i].to == to)
            return 1;
    }
    return 0;
}

// Ajoute un lien unique (from -> to) dans le t_link_array (avec realloc au besoin)
void ajouterLienUnique(t_link_array *tab_liens, int from, int to) {
    if (!lienExiste(tab_liens, from, to)) {
        if (tab_liens->log_size >= tab_liens->phy_size) {
            tab_liens->phy_size *= 2;
            tab_liens->links = realloc(tab_liens->links, tab_liens->phy_size * sizeof(t_link));
        }

        tab_liens->links[tab_liens->log_size].from = from;
        tab_liens->links[tab_liens->log_size].to = to;
        tab_liens->log_size++;
    }
}

// Recense tous les liens entre classes dans le diagramme de Hasse pour un graphe et une partition
void construireLiensHasse(t_list_adjacence graphe, t_partition partition, t_link_array *tab_liens) {
    int n = graphe.nbSommets;
    int *sommet2classe = malloc(n * sizeof(int));

    remplirAppartenance(sommet2classe, partition, n);

    // Initialiser du tableau de liens
    tab_liens->phy_size = (partition.nbClasse < 10 ? 10 : partition.nbClasse * 2);
    tab_liens->log_size = 0;
    tab_liens->links = malloc(tab_liens->phy_size * sizeof(t_link));

    // Parcourir tous les sommets et voisins
    for (int i = 0; i < n; i++) {
        int classe_src = sommet2classe[i];
        t_cell *succ = graphe.adjLists[i].head;
        while (succ) {
            int j = succ->dest - 1;
            int classe_dst = sommet2classe[j];
            if (classe_src != classe_dst) {
                ajouterLienUnique(tab_liens, classe_src, classe_dst);
            }
            succ = succ->next;
        }
    }

    free(sommet2classe);
}

// Génère le diagramme de Hasse au format Mermaid
void genererMermaidHasse(t_partition partition, t_link_array tab_liens, const char *filename) {
    FILE *f = fopen(filename, "w");

    fprintf(f, "flowchart LR\n");
    for (int i = 0; i < partition.nbClasse; i++) {
        fprintf(f, "C%d[\"Composante %s: {", i+1, partition.classes[i].nom_classe);
        for (int j = 0; j < partition.classes[i].nbSommets; j++) {
            fprintf(f, "%d", partition.classes[i].sommets[j]);
            if (j < partition.classes[i].nbSommets - 1) fprintf(f, ",");
        }
        fprintf(f, "}\"]\n");
    }
    for (int k = 0; k < tab_liens.log_size; k++) {
        fprintf(f, "C%d --> C%d\n", tab_liens.links[k].from+1, tab_liens.links[k].to+1);
    }
    fclose(f);
}


// Étape 3 : Caractéristiques du graphe

// Analyser et afficher les propriétés
void analyserProprietes(t_partition partition, t_link_array tab_liens) {
    printf("\n--- Caractéristiques du graphe ---\n");

    // Le graphe est irréductible si et seulement si une seule classe
    int est_irreductible = (partition.nbClasse == 1);

    for (int i = 0; i < partition.nbClasse; i++) {
        int est_transitoire = 0;

        // Recherche une flèche sortante
        for (int k = 0; k < tab_liens.log_size; k++) {
            if (tab_liens.links[k].from == i) {
                est_transitoire = 1;
                break;
            }
        }

        if (est_transitoire) {
            printf("Classe %s : Transitoire\n", partition.classes[i].nom_classe);
            // Tous les états de cette classe sont transitoires
        } else {
            printf("Classe %s : Persistante", partition.classes[i].nom_classe);
            // État absorbant ? Oui, si la classe persistante a un seul membre
            if (partition.classes[i].nbSommets == 1) {
                printf(" -> L'état %d est ABSORBANT", partition.classes[i].sommets[0]);
            }
            printf("\n");
        }
    }

    if (est_irreductible) {
        printf("Le graphe est IRRÉDUCTIBLE.\n");
    } else {
        printf("Le graphe n'est PAS irréductible.\n");
    }
}