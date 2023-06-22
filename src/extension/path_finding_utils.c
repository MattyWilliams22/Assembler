#include "path_finding_utils.h"

bool isSafe(int row, int col) {
  if (row >= 0 && row < gridHeight && col >= 0 && col < gridWidth) {
    return (game_grid[row][col] != WALL && game_grid[row][col] != TAIL && game_grid[row][col] != HEAD);
  }
  return false;
}

int calculate_path(int **parent, int *path, int srcRow, int srcCol, int destRow, int destCol) {
  int path_length = 0;
  int curr_row = destRow;
  int curr_col = destCol;
  int prev_col;
  int prev_row;
  do {
    prev_col = curr_col;
    prev_row = curr_row;
    if (game_grid[prev_row][prev_col] == EMPTY || game_grid[prev_row][prev_col] == SEARCHED) {
      game_grid[prev_row][prev_col] = PATH_ELEM;
    }
    int parent_index = parent[curr_row][curr_col];
    if (parent_index == -1) {
      printf("ERROR\n");
    }
    curr_row = parent_index / gridWidth;
    curr_col = parent_index % gridWidth;
    if (curr_row < prev_row) {
      path[path_length] = 2;
    } else if (curr_row > prev_row) {
      path[path_length] = 1;
    } else if (curr_col < prev_col) {
      path[path_length] = 4;
    } else if (curr_col > prev_col) {
      path[path_length] = 3;
    }
    path_length++;
  } while (curr_col != srcCol || curr_row != srcRow);
  if (gridType == STANDARD) {
    draw_default_grid(game_grid, HEIGHT, WIDTH, nTail, score);
  } else {
    draw_maze_grid(game_grid, mazeHeight, mazeWidth, mazeSize, nTail, score);
  }
  printf("Path found!\n");
  printf("Press enter to continue... \n");
  getchar();
  clear_searched(game_grid, gridHeight, gridWidth);
  return path_length;
}

void draw() {
  if (gridType == STANDARD) {
    draw_default_grid(game_grid, HEIGHT, WIDTH, nTail, score);
  } else {
    draw_maze_grid(game_grid, mazeHeight, mazeWidth, mazeSize, nTail, score);
  }
}