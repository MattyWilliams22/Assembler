#include "bfs.h"

int bfs(int **parent, int srcRow, int srcCol, int destRow, int destCol, int *path) {
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

        if (game_grid[newRow][newCol] == EMPTY) {
          game_grid[newRow][newCol] = SEARCHED;
          usleep(SEARCHDELAY);
          draw();
        }

        queue[rear++] = newRow * gridWidth + newCol;
        parent[newRow][newCol] = currRow * gridWidth + currCol;
      }
    }
  }

  return calculate_path(parent, path, srcRow, srcCol, destRow, destCol);
}
