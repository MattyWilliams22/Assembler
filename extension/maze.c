#include "maze.h"

#define MAZEDELAY 100000

/**
 * Prints the 2D grid array to the console along with the score.
 *
 * @param grid The 2D array of components to be printed
 * @param gridHeight The height of the grid array
 * @param gridWidth The width of the grid array
 * @param nTail The number of tail pieces the snake has
 * @param score The current score of the game
 */
void draw_default_grid(Component **grid, int gridHeight, int gridWidth, int nTail, int score) {
    system("clear");
  if (nTail > 0) {
    printf("Score: %d\n", score);
  } else if (nTail == 0) {
    printf("Welcome to snake!\n");
  }
  for (int i = 0; i < gridHeight; i++) {
    for (int j = 0; j < gridWidth; j++) {
      switch (grid[i][j]) {
        case WALL:
          printf("#");
          break;
        case HEAD:
          printf("O");
          break;
        case TAIL:
          printf("o");
          break;
        case FRUIT:
          printf("X");
          break;
        case EMPTY:
          printf(" ");
          break;
        case PATH_ELEM:
            printf("*");
            break;
        case SEARCHED:
            printf("?");
            break;
        default:
          // U for undefined
          printf("U");
      }
    }
    printf("\n");
  }
} 

/**
 * Checks every component in the grid and changes them back to EMPTY if they are currently SEARCHED.
 * Used to clean up the grid after a search has been performed.
 *
 * @param grid The 2D array of components to be printed
 * @param height The height of the grid array
 * @param width The width of the grid array
 */
void clear_searched(Component **grid, int height, int width) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (grid[i][j] == SEARCHED) {
                grid[i][j] = EMPTY;
            }
        }
    }
}

/**
 * Prints a grid that has been created by the random maze generator.
 *
 * @param grid The 2D array of components to be printed
 * @param height The height of the maze
 * @param width The width of the maze
 * @param size The density of the maze
 * @param nTail The number of tail pieces the snake has
 * @param score The current score of the game
 */
void draw_maze_grid(Component **grid, int height, int width, int size, int nTail, int score) {
    draw_default_grid(grid, get_grid_height(height, size), get_grid_width(width, size), nTail, score);
}

/**
 * Gets the grid height based upon the height of the maze and the density of the maze.
 *
 * @param height The maze height
 * @param size The density of the maze
 * @return The grid height
 */
int get_grid_height(int height, int size) {
    return height * (size + 1) + 1;
}

/**
 * Gets the grid width based upon the width of the maze and the density of the maze.
 *
 * @param width The maze height
 * @param size The density of the maze
 * @return The grid width
 */
int get_grid_width(int width, int size) {
    return width * (size + 1) + 1;
}

/**
 * Sets the array valid_neighbours to the neighbours of the cell s 
 * that are within the bounds of the grid and have not been visited.
 *
 * @param s The cell that is having its neighbours checked
 * @param height The maze height
 * @param width The maze width
 * @param valid_neighbours The array of cells to be set by the function
 * @param visited The 2D array of booleans determining if a cell has been visited yet by the search
 * @return The number of valid neighbours found
 */
int get_valid_neighbours(Cell s, int height, int width, Cell *valid_neighbours, bool **visited) {
    int offset[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    int currRow;
    int currCol;
    int num_valid_neighbours = 0;
    for (int i = 0; i < 4; i++) {
        currRow = s.py + offset[i][1];
        currCol = s.px + offset[i][0];
        if (currRow >= 0 && currRow < height && currCol >= 0 && currCol < width) {
            if (!visited[currRow][currCol]) {
                Cell neighbour = {currCol, currRow};
                valid_neighbours[num_valid_neighbours] = neighbour;
                num_valid_neighbours++;
            }
        }
    }
    return num_valid_neighbours;
}

/**
 * Picks a random cell from the given array of cells
 *
 * @param cells The array of cells to choose from
 * @param num_cells The number of cells in the array
 * @return The cell that has been randomly chosen
 */
Cell get_random_cell(Cell *cells, int num_cells) {
    int index = rand() % num_cells;
    return cells[index];
}

/**
 * Removes the wall between the cell first and the cell second. 
 * It does this by setting tthe relevant components of the grid to empty.
 *
 * @param first One of the cells to be connected
 * @param second The other cell to be connected
 * @param grid The 2D array of components to be altered by the function
 * @param size The density of the maze
 * @param height The height of the maze
 * @param width The width of the maze
 */
void connect_cells(Cell first, Cell second, Component **grid, int size, int height, int width) {
    if (first.px == second.px) {
        if (first.py < second.py) {
            int j = second.py * (size + 1);
            int start_i = first.px * (size + 1) + 1;
            for (int i = start_i; i < start_i + size; i++) {
                grid[i][j] = EMPTY;
            }
        } else if (first.py > second.py) {
            int j = first.py * (size + 1);
            int start_i = first.px * (size + 1) + 1;
            for (int i = start_i; i < start_i + size; i++) {
                grid[i][j] = EMPTY;
            }
        } 
    } else if (first.py == second.py) {
        if (first.px < second.px) {
            int i = second.px * (size + 1);
            int start_j = first.py * (size + 1) + 1;
            for (int j = start_j; j < start_j + size; j++) {
                grid[i][j] = EMPTY;
            }
        } else if (first.px > second.px) {
            int i = first.px * (size + 1);
            int start_j = first.py * (size + 1) + 1;
            for (int j = start_j; j < start_j + size; j++) {
                grid[i][j] = EMPTY;
            }
        } 
    }
    draw_maze_grid(grid, height, width, size, -1, -1);
    usleep(MAZEDELAY);
}

/**
 * Performs depth first search to form a random maze in the grid.
 *
 * @param root The cell to start the dfs at
 * @param grid The 2D array of components that contains the maze 
 * @param visited The 2D array of booleans determining if a cell has been visited yet by the search
 * @param height The height of the maze
 * @param width The width of the maze
 * @param size The density of the maze
 */
void random_dfs(Cell root, Component **grid, bool **visited, int height, int width, int size) {
    visited[root.py][root.px] = true;
    Cell nextVertex;
    Cell *valid_neighbours;
    valid_neighbours = malloc(4 * sizeof(*valid_neighbours));
    int num_valid_neighbours = get_valid_neighbours(root, height, width, valid_neighbours, visited);
    while (num_valid_neighbours != 0) {
        nextVertex = get_random_cell(valid_neighbours, num_valid_neighbours); 
        connect_cells(root, nextVertex, grid, size, height, width);
        random_dfs(nextVertex, grid, visited, height, width, size);
        num_valid_neighbours = get_valid_neighbours(root, height, width, valid_neighbours, visited);
    }
    free(valid_neighbours);
}

/**
 * Allocates memory for a 2D array of components.
 *
 * @param height The height of the maze
 * @param width The width of the maze
 * @param size The density of the maze
 * @return The pointer to the grid that has been allocated memory
 */
Component **allocate_maze_grid(int height, int width, int size) {
    return allocate_default_grid(get_grid_height(height, size), get_grid_width(width, size));
}

/**
 * Allocates memory for a 2D array of components.
 *
 * @param gridHeight The height of the grid
 * @param gridWidth The width of the maze
 * @return The pointer to the grid that has been allocated memory
 */
Component **allocate_default_grid(int gridHeight, int gridWidth) {
    Component **grid;
    grid = malloc(gridHeight * sizeof(*grid));
    for (int i = 0; i < gridHeight; i++) {
        grid[i] = malloc(gridWidth * sizeof(*grid[i]));
    }
    return grid;
}

/**
 * Frees a grid from memory.
 */
void free_maze_grid(Component **grid, int height, int size) {
    free_default_grid(grid, get_grid_height(height, size));
}

/**
 * Frees a grid from memory.
 */
void free_default_grid(Component **grid, int gridHeight) {
    for (int i = 0; i < gridHeight; i++) {
        free(grid[i]);
    }
    free(grid);
}

/**
 * Sets the components of the grid so that every cell of the maze is enclosed by walls.
 *
 * @param size The density of the maze
 * @param height The height of the maze
 * @param width The width of the maze
 * @param grid The 2D array of components to be setup by the function
 */
void setup_grid(int size, int height, int width, Component **grid) {
    int gridHeight = get_grid_height(height, size);
    int gridWidth = get_grid_width(width, size);
    for (int i = 0; i < gridHeight; i++) {
        for (int j = 0; j < gridWidth; j++) {
            if (i % (size + 1) == 0) {
                grid[i][j] = WALL;
            } else if (j % (size + 1) == 0) {
                grid[i][j] = WALL;
            } else {
                grid[i][j] = EMPTY;
            }
        }
    }
    draw_maze_grid(grid, height, width, size, -1, -1);
}

/**
 * Makes a randomly generated maze.
 *
 * @param size The density of the maze
 * @param height The height of the maze
 * @param width The width of the maze
 * @param grid The 2D array of components that will become the maze
 */
void make_maze(int size, int height, int width, Component **grid) {
    setup_grid(size, height, width, grid);
    Cell start = {0, 0};
    bool **visited;
    visited = malloc(height * sizeof(*visited));
    for (int i = 0; i < height; i++) {
        visited[i] = malloc(width * sizeof(*visited[i]));
    }
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            visited[i][j] = false;
        }
    }
    random_dfs(start, grid, visited, height, width, size);
    for (int i = 0; i < height; i++) {
        free(visited[i]);
    }
    free(visited);
    printf("Here is the grid after the algorithm!\n");
    printf("Press enter to continue...\n");
    getchar();
}
