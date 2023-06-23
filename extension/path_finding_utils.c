#include "path_finding_utils.h"

/**
 * Determines whether a cell on the grid is safe to move to. 
 * Checks that the row and col values are within the expected range 
 * and that the cell is not a wall or the tail of the snake. 
 *
 * @param row The row within the grid that is being checked
 * @param col The column within the grid that is being checked
 * @return True iff the cell is safe to move to and false otherwise.
 */
bool isSafe(int row, int col) {
  if (row >= 0 && row < gridHeight && col >= 0 && col < gridWidth) {
    return (game_grid[row][col] != WALL && game_grid[row][col] != TAIL && game_grid[row][col] != HEAD);
  }
  return false;
}

/**
 * Converts a 2D array of parents into an array of flags by backtracking from the destination cell
 * to the source cell and adding a flag for every move made. 
 *
 * @param parent The 2D array containing the parents of each cell from a search
 * @param path The array of flags to be set by the function
 * @param srcRow The row that the path starts at
 * @param srcCol The column that the path starts at
 * @param destRow The row that the path ends at
 * @param destCol The column that the path ends at
 * @return The length of the path array. 
 */
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

/**
 * Calls a drawing function based upon the type of grid used for the game.
 */
void draw() {
  if (gridType == STANDARD) {
    draw_default_grid(game_grid, HEIGHT, WIDTH, nTail, score);
  } else {
    draw_maze_grid(game_grid, mazeHeight, mazeWidth, mazeSize, nTail, score);
  }
}

void setup_pathfinding(bool visited[gridHeight][gridWidth], int **parent, int srcRow, int srcCol) {
  for (int i = 0; i < gridHeight; i++) {
    for (int j = 0; j < gridWidth; j++) {
      visited[i][j] = false;
      parent[i][j] = -1;
    }
  }
  visited[srcRow][srcCol] = true;

  search_and_draw(srcRow, srcCol);
}

void search_and_draw(int row, int col) {
  if (game_grid[row][col] == EMPTY) {
    game_grid[row][col] = SEARCHED;
    usleep(SEARCHDELAY);
    draw();
  }
}