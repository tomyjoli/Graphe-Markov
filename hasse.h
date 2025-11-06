#ifndef __HASSE_H__
#define __HASSE_H__
#include "utils.h"

void removeTransitiveLinks(t_link_array *p_link_array);

/**
 * @brief Creates a link array from the given partition and graph.
 *
 * @param part The partition of the graph.
 * @param graph The adjacency list representation of the graph.
 * @return The created link array.
 */

t_link_array createLinkArray(t_list_adjacence adjList);

#endif