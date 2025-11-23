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


//PARTIE 2
// --- Fonctions d'Utilitaires pour la Partie 2 ---


// Étape 2.1 : Fonction qui initialise le tableau de sommets pour Tarjan
t_tarjan_vertex* creer_tableau_tarjan(t_list_adjacence graphe) {
    // Allocation du tableau pour tous les sommets du graphe
    t_tarjan_vertex* tab_sommets = (t_tarjan_vertex*)malloc(graphe.nbSommets * sizeof(t_tarjan_vertex));

    if (tab_sommets == NULL) {
        perror("Erreur d'allocation mémoire pour le tableau Tarjan");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < graphe.nbSommets; i++) {
        tab_sommets[i].id = i + 1;       // Identifiant réel (1, 2, 3...)
        tab_sommets[i].number = -1;      // Numérotation temporaire (non visité)
        tab_sommets[i].nb_access = -1;   // Numéro accessible (low-link)
        tab_sommets[i].flag = 0;         // 0 = Pas dans la pile
    }
    return tab_sommets;
}

// Étape 2.2 : Fonctions de gestion de la pile

// Crée une pile vide avec une capacité donnée
t_pile creer_pile(int capacite) {
    t_pile p;
    p.capacite = capacite;
    p.premier = -1; // -1 indique que la pile est vide
    p.tab = (t_tarjan_vertex*)malloc(capacite * sizeof(t_tarjan_vertex));

    if (p.tab == NULL) {
        perror("Erreur d'allocation mémoire pour la pile");
        exit(EXIT_FAILURE);
    }

    return p;
}

// Ajoute un sommet au sommet de la pile
void empiler(t_pile *p, t_tarjan_vertex *v) {
    // Vérification pour ne pas dépasser la capacité
    if (p->premier < p->capacite - 1) {
        p->premier++;             // On monte l'indice
        p->tab[p->premier] = v;   // On stocke le sommet
    } else {
        printf("Erreur : Pile pleine (Stack Overflow)\n");
    }
}

// Retire et renvoie le sommet au sommet de la pile
t_tarjan_vertex* depiler(t_pile *p) {
    if (p->premier >= 0) {
        t_tarjan_vertex *v = p->tab[p->premier];
        p->premier--;             // On descend l'indice
        return v;
    }
    return NULL; // Retourne NULL si la pile était vide
}

// Vérifie si la pile est vide (retourne 1 si vide, 0 sinon)
int pile_est_vide(t_pile p) {
    return (p.premier == -1);
}






// --- Fonctions utilitaires internes pour gérer la Partition ---

// Ajoute une nouvelle classe vide à la partition
void ajouter_classe(t_partition *p) {
    // Si le tableau de classes est plein, on l'agrandit
    if (p->nb_classe >= p->taille_classe) {
        p->taille_classe *= 2;
        p->classes = (t_classe*)realloc(p->classes, p->taille_classe * sizeof(t_classe));
    }

    // Initialisation de la nouvelle classe
    t_classe *c = &p->classes[p->nb_classe];
    sprintf(c->nom_classe, "C%d", p->nb_classe + 1); // Nomme la classe C1, C2...
    c->nb_sommets = 0;
    c->capacite_max = 10; // Capacité initiale arbitraire
    c->sommets = (t_tarjan_vertex*)malloc(c->capacite_max * sizeof(t_tarjan_vertex));

    p->nb_classe++;
}

// Ajoute un sommet à la dernière classe créée
void ajouter_sommet_a_classe(t_classe *c, t_tarjan_vertex *v) {
    // Si la classe est pleine, on l'agrandit
    if (c->nb_sommets >= c->capacite_max) {
        c->capacite_max *= 2;
        c->sommets = (t_tarjan_vertex*)realloc(c->sommets, c->capacite_max * sizeof(t_tarjan_vertex));
    }
    c->sommets[c->nb_sommets++] = v;
}

// --- Algorithme de Tarjan ---

// Étape 3.1 : La sous-fonction 'parcours'
// Paramètres : indice du sommet u, graphe, tableau des infos, pile, partition, pointeur vers temps
void parcours(int u_idx, t_list_adjacence graphe, t_tarjan_vertex *tab_sommets,
              t_pile *p_pile, t_partition *partition, int *temps) {
    t_tarjan_vertex *u = &tab_sommets[u_idx];
    u->number = *temps;
    u->nb_access = *temps;
    (*temps)++;
    empiler(p_pile, u);
    u->flag = 1;

    t_cell *courant = graphe.adjLists[u_idx].head;
    while (courant != NULL) {
        int v_idx = courant->dest - 1;
        t_tarjan_vertex *v = &tab_sommets[v_idx];

        if (v->number == -1) {
            parcours(v_idx, graphe, tab_sommets, p_pile, partition, temps);
            if (v->nb_access < u->nb_access) {
                u->nb_access = v->nb_access;
            }
        } else if (v->flag == 1) {
            if (v->number < u->nb_access) {
                u->nb_access = v->number;
            }
        }
        courant = courant->next;
    }

    if (u->number == u->nb_access) {
        ajouter_classe(partition);
        t_classe *classe_courante = &partition->classes[partition->nb_classe - 1];
        t_tarjan_vertex *v = NULL;

        do {
            v = depiler(p_pile);
            v->flag = 0;
            ajouter_sommet_a_classe(classe_courante, v);
        } while (u != v);
    }
}


// Étape 3.2 : Fonction principale 'tarjan'
void tarjan_algo(t_list_adjacence graphe, t_partition *partition) {
    // Initialisation de la partition
    partition->nb_classe = 0;
    partition->taille_classe = 10;
    partition->classes = (t_classe*)malloc(partition->taille_classe * sizeof(t_classe));

    t_tarjan_vertex *tab_sommets = creer_tableau_tarjan(graphe);
    t_pile pile = creer_pile(graphe.nbSommets);
    int temps = 0;

    // Initialiser tous les sommets
    for (int i = 0; i < graphe.nbSommets; i++) {
        if (tab_sommets[i].number == -1) {
            parcours(i, graphe, tab_sommets, &pile, partition, &temps);
        }
    }

    free(tab_sommets);
    free(pile.tab);
}

// Fonction d'affichage pour valider l'étape 1 (format demandé page 5)
void afficher_partition(t_partition partition) {
    for (int i = 0; i < partition.nb_classe; i++) {
        printf("Composante %s:\n", partition.classes[i].nom_classe);
        printf("{");
        for (int j = 0; j < partition.classes[i].nb_sommets; j++) {
            printf("%d", partition.classes[i].sommets[j]->id);
            if (j < partition.classes[i].nb_sommets - 1) {
                printf(",");
            }
        }
        printf("}\n");
    }
}


// --- Outils pour le Diagramme de Hasse ---

// Trouve l'indice de la classe (0 à N-1) à laquelle appartient un sommet donné par son ID
int trouver_index_classe(int id_sommet, t_partition partition) {
    for (int i = 0; i < partition.nb_classe; i++) {
        for (int j = 0; j < partition.classes[i].nb_sommets; j++) {
            if (partition.classes[i].sommets[j]->id == id_sommet) {
                return i;
            }
        }
    }
    return -1; // Ne devrait pas arriver si la partition est complète
}

// Vérifie si un lien entre deux classes existe déjà
int lien_existe(t_link_array *tab_liens, int de, int vers) {
    for (int i = 0; i < tab_liens->log_size; i++) {
        if (tab_liens->links[i].from == de && tab_liens->links[i].to == vers) {
            return 1; // Vrai
        }
    }
    return 0; // Faux
}

// Ajoute un lien unique (sans doublon) dans tab_liens
void ajouter_lien_unique(t_link_array *tab_liens, int de, int vers) {
    if (!lien_existe(tab_liens, de, vers)) {
        if (tab_liens->log_size >= tab_liens->phy_size) {
            tab_liens->phy_size *= 2;
            tab_liens->links = (t_link*)realloc(tab_liens->links, tab_liens->phy_size * sizeof(t_link));
        }
        tab_liens->links[tab_liens->log_size].from = de;
        tab_liens->links[tab_liens->log_size].to = vers;
        tab_liens->log_size++;
    }
}

// Vérifie si une classe est absorbante (boucle sur elle-même)
int est_classe_absorbante(t_classe classe, t_list_adjacence graphe, t_partition partition) {
    int classe_courante = trouver_index_classe(classe.sommets[0]->id, partition);

    for (int i = 0; i < classe.nb_sommets; i++) {
        int id_sommet = classe.sommets[i]->id;
        t_cell *courant = graphe.adjLists[id_sommet - 1].head;
        int a_boucle = 0;

        while (courant != NULL) {
            int classe_dest = trouver_index_classe(courant->dest, partition);

            if (classe_dest != classe_courante) {
                return 0; // Lien externe trouvé
            }

            if (courant->dest == id_sommet) {
                a_boucle = 1; // Boucle interne trouvée
            }

            courant = courant->next;
        }

        if (!a_boucle) {
            return 0; // Pas de boucle interne pour ce sommet
        }
    }

    return 1; // Classe absorbante
}

// Vérifie si une classe est transitoire (pas de boucle)
int est_classe_transitoire(t_classe classe, t_list_adjacence graphe, t_partition partition) {
    // Pré-calculer l'index de la classe une seule fois
    int classe_courante = trouver_index_classe(classe.sommets[0]->id, partition);

    for (int i = 0; i < classe.nb_sommets; i++) {
        int id_sommet = classe.sommets[i]->id;
        t_cell *courant = graphe.adjLists[id_sommet - 1].head;

        while (courant != NULL) {
            int id_dest = courant->dest;
            int classe_dest = trouver_index_classe(id_dest, partition);

            if (classe_dest == classe_courante) {
                return 0; // Lien interne trouvé
            }
            courant = courant->next;
        }
    }
    return 1; // Aucun lien interne
}

// Vérifie si le graphe est irréductible (une seule classe)
int est_graphe_irreductible(t_partition partition) {
    return (partition.nb_classe == 1);
}

// --- Étape 3 : Caractéristiques du graphe ---

// Analyse et affiche les propriétés (Transitoire, Persistante, Absorbant, Irréductible)
void analyser_proprietes(t_partition partition, t_link_array tab_liens) {
    printf("\n--- Caracteristiques du graphe ---\n");

    int est_irreductible = (partition.nb_classe == 1);

    for (int i = 0; i < partition.nb_classe; i++) {
        int est_transitoire = 0;

        for (int k = 0; k < tab_liens.log_size; k++) {
            if (tab_liens.links[k].from == i) {
                est_transitoire = 1;
                break;
            }
        }

        if (est_transitoire) {
            printf("Classe %s : Transitoire\n", partition.classes[i].nom_classe);
        } else {
            printf("Classe %s : Persistante", partition.classes[i].nom_classe);

            if (partition.classes[i].nb_sommets == 1) {
                printf(" -> L'etat %d est ABSORBANT", partition.classes[i].sommets[0]->id);
            }
            printf("\n");
        }
    }

    if (est_irreductible) {
        printf("Le graphe est IRREDUCTIBLE.\n");
    } else {
        printf("Le graphe n'est PAS irreductible.\n");
    }
}

// --- Étape 2 : Diagramme de Hasse ---

// Génère le fichier Mermaid et lance l'analyse des propriétés
void generer_diagramme_hasse(t_list_adjacence graphe, t_partition partition, const char *nom_fichier) {
    FILE *file = fopen(nom_fichier, "w");
    if (!file) {
        perror("Erreur à l'ouverture du fichier");
        return;
    }

    fprintf(file, "---\n    layout: elk\n    theme: neo\n---\n\nflowchart TD\n");

    // 1. Écrire les classes (nœuds)
    for (int i = 0; i < partition.nb_classe; i++) {
        fprintf(file, "    %s[\"%s: ", partition.classes[i].nom_classe, partition.classes[i].nom_classe);
        for (int j = 0; j < partition.classes[i].nb_sommets; j++) {
            fprintf(file, "%d", partition.classes[i].sommets[j]->id);
            if (j < partition.classes[i].nb_sommets - 1) fprintf(file, ",");
        }
        fprintf(file, "\"]\n");
    }

    // 2. Écrire les liens entre classes
    t_link_array tab_liens;
    tab_liens.phy_size = 20;
    tab_liens.log_size = 0;
    tab_liens.links = (t_link*)malloc(tab_liens.phy_size * sizeof(t_link));

    for (int i = 0; i < graphe.nbSommets; i++) {
        int id_u = i + 1;
        int index_classe_u = trouver_index_classe(id_u, partition);
        t_cell *courant = graphe.adjLists[i].head;

        while (courant != NULL) {
            int id_v = courant->dest;
            int index_classe_v = trouver_index_classe(id_v, partition);
            if (index_classe_u != index_classe_v) {
                ajouter_lien_unique(&tab_liens, index_classe_u, index_classe_v);
            }
            courant = courant->next;
        }
    }

    // 3. Générer les liens dans le fichier
    for (int i = 0; i < tab_liens.log_size; i++) {
        fprintf(file, "    %s --> %s\n",
                partition.classes[tab_liens.links[i].from].nom_classe,
                partition.classes[tab_liens.links[i].to].nom_classe);
    }

    // 4. Analyse des propriétés
    fprintf(file, "\n%% Propriétés du graphe\n");
    for (int i = 0; i < partition.nb_classe; i++) {
        if (est_classe_absorbante(partition.classes[i],graphe,partition)) {
            fprintf(file, "%% Classe %s: ABSORBANTE\n", partition.classes[i].nom_classe);
        } else if (est_classe_transitoire(partition.classes[i], graphe, partition)) {
            fprintf(file, "%% Classe %s: TRANSITOIRE\n", partition.classes[i].nom_classe);
        } else {
            fprintf(file, "%% Classe %s: PERSISTANTE\n", partition.classes[i].nom_classe);
        }
    }

    if (est_graphe_irreductible(partition)) {
        fprintf(file, "%% Graphe: IRREDUCTIBLE\n");
    } else {
        fprintf(file, "%% Graphe: REDUCTIBLE\n");
    }

    fclose(file);
    free(tab_liens.links);
}

// Fonction de nettoyage mémoire pour la partition
void liberer_partition(t_partition *partition) {
    for (int i = 0; i < partition->nb_classe; i++) {
        free(partition->classes[i].sommets); // Libère le tableau de pointeurs
    }
    free(partition->classes); // Libère le tableau de classes
    partition->nb_classe = 0;
}