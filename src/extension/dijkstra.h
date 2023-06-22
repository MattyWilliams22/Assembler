#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "global.h"
#include <stdbool.h>
#include <unistd.h>
#include "path_finding_utils.h"
#include <limits.h>

int dijkstra(int **parent, int srcRow, int srcCol, int destRow, int destCol, int *path);

#endif