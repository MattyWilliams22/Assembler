#ifndef A_STAR_H
#define A_STAR_H

#include "path_finding_utils.h"

int calculateHCost(int row, int col, int destRow, int destCol);
int aStar(int **parent, int srcRow, int srcCol, int destRow, int destCol, int *path);

#endif