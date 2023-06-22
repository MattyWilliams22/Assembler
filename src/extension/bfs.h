#ifndef BFS_H
#define BFS_H

#include "global.h"
#include <stdbool.h>
#include <unistd.h>
#include "path_finding_utils.h"

int bfs(int **parent, int srcRow, int srcCol, int destRow, int destCol, int *path);

#endif