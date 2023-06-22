#include "dijkstra.h"

int dijkstra(int **parent, int srcRow, int srcCol, int destRow, int destCol, int *path) {
  int dx[] = {0, 0, -1, 1};
  int dy[] = {-1, 1, 0, 0};
  
  bool visited[gridHeight][gridWidth];
  for (int i = 0; i < gridHeight; i++) {
    for (int j = 0; j < gridWidth; j++) {
      visited[i][j] = false;
    }
  }

  int queue[gridHeight * gridWidth];
  int front = 0, rear = 0;

  visited[srcRow][srcCol] = true;

  if (game_grid[srcRow][srcCol] == EMPTY) {
    game_grid[srcRow][srcCol] = SEARCHED;
    draw();
  }

  queue[rear++] = srcRow * gridWidth + srcCol;

  for (int i = 0; i < gridHeight; i++) {
    for (int j = 0; j < gridWidth; j++) {
      parent[i][j] = -1;
    }
  }

  int gCost[gridHeight][gridWidth];
  for (int i = 0; i < gridHeight; i++) {
    for (int j = 0; j < gridWidth; j++) {
      gCost[i][j] = INT_MAX;
    }
  }
  gCost[srcRow][srcCol] = 0;

  while (front != rear) {
    int minGCost = INT_MAX;
    int currNode;
    for (int i = front; i < rear; i++) {
      int row = queue[i] / gridWidth;
      int col = queue[i] % gridWidth;
      if (gCost[row][col] < minGCost) {
        minGCost = gCost[row][col];
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

        if (game_grid[newRow][newCol] == EMPTY) {
          game_grid[newRow][newCol] = SEARCHED;
          usleep(SEARCHDELAY);
          draw();
        }

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
