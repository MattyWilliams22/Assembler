#include "a_star.h"

/**
 * Performs pathfinding on the grid using the A* search algorithm to find the shortest path from the source to the destination.
 * 
 * @param parent The 2D array containing the parents of each cell from a search
 * @param srcRow The row that the search starts at
 * @param srcCol The column that the search starts at
 * @param destRow The row that the search ends at
 * @param destCol The column that the search ends at
 * @param path The array of flags to be set by the function
 * @return The length of the path array 
 */
int calculateHCost(int row, int col, int destRow, int destCol) {
  return abs(destRow - row) + abs(destCol - col);
}

int aStar(int **parent, int srcRow, int srcCol, int destRow, int destCol, int *path) {
  int dx[4] = {0, 0, -1, 1};
  int dy[4] = {-1, 1, 0, 0};
  bool visited[gridHeight][gridWidth];
  setup_pathfinding(visited, parent, srcRow, srcCol);

  int queue[gridHeight * gridWidth];
  int front = 0, rear = 0;
  queue[rear++] = srcRow * gridWidth + srcCol;

  int gCost[gridHeight][gridWidth];
  for (int i = 0; i < gridHeight; i++) {
    for (int j = 0; j < gridWidth; j++) {
      gCost[i][j] = INT_MAX;
    }
  }
  gCost[srcRow][srcCol] = 0;

  while (front != rear) {
    int minFCost = INT_MAX;
    int currNode;
    for (int i = front; i < rear; i++) {
      int row = queue[i] / gridWidth;
      int col = queue[i] % gridWidth;
      int fCost = gCost[row][col] + calculateHCost(row, col, destRow, destCol);
      if (fCost < minFCost) {
        minFCost = fCost;
        currNode = i;
      }
    }

    int currRow = queue[currNode] / gridWidth;
    int currCol = queue[currNode] % gridWidth;

    if (currRow == destRow && currCol == destCol) {
      break;
    }

    queue[currNode] = queue[front];
    front++;

    for (int i = 0; i < 4; i++) {
      int newRow = currRow + dx[i];
      int newCol = currCol + dy[i];
      if (isSafe(newRow, newCol) && !visited[newRow][newCol]) {
        visited[newRow][newCol] = true;
        search_and_draw(newRow, newCol);
        queue[rear++] = newRow * gridWidth + newCol;
        parent[newRow][newCol] = currRow * gridWidth + currCol;

        int newGCost = gCost[currRow][currCol] + 1;
        if (newGCost < gCost[newRow][newCol]) {
          gCost[newRow][newCol] = newGCost;
        }
      }
    }
  }

  return calculate_path(parent, path, srcRow, srcCol, destRow, destCol);
}
