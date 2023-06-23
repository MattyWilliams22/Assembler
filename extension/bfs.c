#include "bfs.h"

/**
 * Performs pathfinding on the grid using breadth first search to find the shortest path from the source to the destination.
 * 
 * @param parent The 2D array containing the parents of each cell from a search
 * @param srcRow The row that the search starts at
 * @param srcCol The column that the search starts at
 * @param destRow The row that the search ends at
 * @param destCol The column that the search ends at
 * @param path The array of flags to be set by the function
 * @return The length of the path array 
 */
int bfs(int **parent, int srcRow, int srcCol, int destRow, int destCol, int *path) {
  int dx[4] = {0, 0, -1, 1};
  int dy[4] = {-1, 1, 0, 0};
  bool visited[gridHeight][gridWidth];
  setup_pathfinding(visited, parent, srcRow, srcCol);

  int queue[gridHeight * gridWidth];
  int front = 0, rear = 0;
  queue[rear++] = srcRow * gridWidth + srcCol;

  while (front != rear) {
    int currRow = queue[front] / gridWidth;
    int currCol = queue[front] % gridWidth;
    front++;

    if (currRow == destRow && currCol == destCol) {
      break;
    }

    for (int i = 0; i < 4; i++) {
      int newRow = currRow + dx[i];
      int newCol = currCol + dy[i];
      if (isSafe(newRow, newCol) && !visited[newRow][newCol]) {
        visited[newRow][newCol] = true;
        search_and_draw(newRow, newCol);
        queue[rear++] = newRow * gridWidth + newCol;
        parent[newRow][newCol] = currRow * gridWidth + currCol;
      }
    }
  }

  return calculate_path(parent, path, srcRow, srcCol, destRow, destCol);
}
