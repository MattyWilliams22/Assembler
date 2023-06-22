#ifndef DFS_H
#define DFS_H

#include "global.h"
#include <stdbool.h>
#include <stdlib.h>
#include "path_finding_utils.h"
#include <unistd.h>

int dfs(int **parent, int srcRow, int srcCol, int destRow, int destCol, int *path);

#endif