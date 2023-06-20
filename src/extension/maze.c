#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "maze.h"

#define MAZEDELAY 100000

int get_grid_height(int height, int size) {
    return height * (size + 1) + 1;
}

int get_grid_width(int width, int size) {
    return width * (size + 1) + 1;
}

void draw_grid(Component **grid, int height, int width, int size) {
    system("clear");
    int gridHeight = get_grid_height(height, size);
    int gridWidth = get_grid_width(width, size);
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
                default:
                    // U for undefined
                    printf("U");
            }
        }
        printf("\n");
    }
    usleep(MAZEDELAY);
}

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

Cell get_random_cell(Cell *cells, int num_cells) {
    int index = rand() % num_cells;
    return cells[index];
}

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
    draw_grid(grid, height, width, size);
}

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

Component **allocate_grid(int height, int width, int size) {
    int gridHeight = get_grid_height(height, size);
    int gridWidth = get_grid_width(width, size); 
    Component **grid;
    grid = malloc(gridHeight * sizeof(*grid));
    for (int i = 0; i < gridHeight; i++) {
        grid[i] = malloc(gridWidth * sizeof(*grid[i]));
    }
    return grid;
}

void free_grid(Component **grid, int height, int size) {
    for (int i = 0; i < get_grid_height(height, size); i++) {
        free(grid[i]);
    }
    free(grid);
}

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
    draw_grid(grid, height, width, size);
    printf("Here is the grid before the algorithm!\n");
}

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
}
