#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// ETAPE 1 :

// Structure pour une cellule (arête)
typedef struct Cellule {
    int sommet_arrivee;      // Sommet d'arrivée
    float probabilite;       // Probabilité de transition
    struct Cellule *suivant; // Pointeur vers la cellule suivante
} Cellule;

// Structure pour une liste chaînée (liste d'arêtes sortantes d'un sommet)
typedef struct Liste {
    Cellule *head; // Pointeur vers la première cellule
} Liste;

// Structure pour la liste d'adjacence (tableau de listes)
typedef struct {
    Liste *tab;    // Tableau de listes
    int taille;    // Nombre de sommets
} ListeAdjacence;

// Créer une cellule
Cellule *creer_cellule(int arrivee, float proba) {
    Cellule *nouvelle = (Cellule *)malloc(sizeof(Cellule));
    if (nouvelle == NULL) {
        perror("Erreur allocation mémoire cellule");
        exit(EXIT_FAILURE);
    }
    nouvelle->sommet_arrivee = arrivee;
    nouvelle->probabilite = proba;
    nouvelle->suivant = NULL;
    return nouvelle;
}

// Fonction pour créer une liste vide
Liste creer_liste_vide() {
    Liste liste;
    liste.head = NULL;
    return liste;
}

// Fonction pour ajouter une cellule en tête de liste
void ajouter_en_tete(Liste *liste, Cellule *cellule) {
    cellule->suivant = liste->head;
    liste->head = cellule;
}

// Fonction pour afficher une liste
void afficher_liste(const Liste *liste) {
    Cellule *actuelle = liste->head;
    while (actuelle != NULL) {
        printf("-> (%d, %.2f) ", actuelle->sommet_arrivee, actuelle->probabilite);
        actuelle = actuelle->suivant;
    }
    printf("\n");
}

// Créer une liste d'adjacence vide
ListeAdjacence creer_liste_adjacence_vide(int taille) {
    ListeAdjacence la;
    la.taille = taille;
    la.tab = (Liste *)malloc(taille * sizeof(Liste));
    if (la.tab == NULL) {
        perror("Erreur allocation mémoire tableau");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < taille; i++) {
        la.tab[i] = creer_liste_vide();
    }
    return la;
}

// Afficher une liste d'adjacence
void afficher_liste_adjacence(const ListeAdjacence *la) {
    for (int i = 0; i < la->taille; i++) {
        printf("Liste pour le sommet %d: ", i + 1);
        afficher_liste(&la->tab[i]);
    }
}

// Libérer la mémoire d'une liste
void liberer_liste(Liste *liste) {
    Cellule *actuelle = liste->head;
    while (actuelle != NULL) {
        Cellule *suivante = actuelle->suivant;
        free(actuelle);
        actuelle = suivante;
    }
    liste->head = NULL;
}

// Libérer la mémoire d'une liste d'adjacence
void liberer_liste_adjacence(ListeAdjacence *la) {
    for (int i = 0; i < la->taille; i++) {
        liberer_liste(&la->tab[i]);
    }
    free(la->tab);
    la->tab = NULL;
    la->taille = 0;
}

ListeAdjacence readGraph(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (file == NULL) {
        perror("Could not open file for reading");
        exit(EXIT_FAILURE);
    }

    int nbvert;
    if (fscanf(file, "%d", &nbvert) != 1) {
        perror("Could not read number of vertices");
        exit(EXIT_FAILURE);
    }

    ListeAdjacence la = creer_liste_adjacence_vide(nbvert);

    int depart, arrivee;
    float proba;
    while (fscanf(file, "%d %d %f", &depart, &arrivee, &proba) == 3) {
        // Vérifier que les sommets sont valides
        if (depart < 1 || depart > nbvert || arrivee < 1 || arrivee > nbvert) {
            fprintf(stderr, "Sommet invalide dans le fichier: %d ou %d\n", depart, arrivee);
            exit(EXIT_FAILURE);
        }
        // Créer une cellule et l'ajouter à la liste du sommet 'depart'
        Cellule *nouvelle = creer_cellule(arrivee, proba);
        ajouter_en_tete(&la.tab[depart - 1], nouvelle);
    }

    fclose(file);
    return la;
}

// ETAPE 2 :

