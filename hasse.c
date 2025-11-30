#include <malloc.h>
#include "hasse.h"
#include <stdlib.h>

t_link_array createLinkArray(t_list_adjacence adjList) {
        t_link_array linkArray;
    linkArray.phy_size = adjList.nbSommets * adjList.nbSommets; // Taille max possible
    linkArray.log_size = 0;
    linkArray.links = (t_link*)malloc(linkArray.phy_size * sizeof(t_link));
    if (linkArray.links == NULL) {
        perror("Erreur d'allocation pour le tableau de liens");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < adjList.nbSommets; i++) {
        t_cell *current = adjList.adjLists[i].head;
        while (current != NULL) {
            linkArray.links[linkArray.log_size].from = i + 1;
            linkArray.links[linkArray.log_size].to = current->dest;
            linkArray.log_size++;
            current = current->next;
        }
    }

    return linkArray;
}

void removeTransitiveLinks(t_link_array *p_link_array)
{
    int i = 0;
    while (i < p_link_array->log_size)
    {
        t_link link1 = p_link_array->links[i];
        int j = 0;
        int to_remove = 0;
        while (j < p_link_array->log_size && !to_remove)
        {
            if (j != i)
            {
                t_link link2 = p_link_array->links[j];
                if (link1.from == link2.from)
                {
                    // look for a link from link2.to to link1.to
                    int k = 0;
                    while (k < p_link_array->log_size && !to_remove)
                    {
                        if (k != j && k != i)
                        {
                            t_link link3 = p_link_array->links[k];
                            if ((link3.from == link2.to) && (link3.to == link1.to))
                            {
                                to_remove = 1;
                            }
                        }
                        k++;
                    }
                }
            }
            j++;
        }
        if (to_remove)
        {
            // remove link1 by replacing it with the last link
            p_link_array->links[i] = p_link_array->links[p_link_array->log_size - 1];
            p_link_array->log_size--;
        }
        else
        {
            i++;
        }
    }
}
