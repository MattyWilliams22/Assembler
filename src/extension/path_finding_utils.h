#ifndef PATH_FINDING_UTILS_H
#define PATH_FINDING_UTILS_H

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include "global.h"
#include "maze.h"

bool isSafe(int row, int col);
int calculate_path(int **parent, int *path, int srcRow, int srcCol, int destRow, int destCol);
void draw();

#endif