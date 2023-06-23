#ifndef MAZE_H
#define MAZE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "global.h"

typedef struct {
    int px;
    int py;
} Cell;

void setup_grid(int size, int height, int width, Component **grid);
void make_maze(int size, int height, int width, Component **grid);
Component **allocate_maze_grid(int height, int width, int size);
Component **allocate_default_grid(int gridHeight, int gridWidth);
void free_maze_grid(Component **grid, int height, int size);
void free_default_grid(Component **grid, int gridHeight);
int get_grid_height(int height, int size);
int get_grid_width(int width, int size);
void draw_maze_grid(Component **grid, int height, int width, int size, int nTail, int score);
void draw_default_grid(Component **grid, int gridHeight, int gridWidth, int nTail, int score);
int get_valid_neighbours(Cell s, int height, int width, Cell *valid_neighbours, bool **visited);
Cell get_random_cell(Cell *cells, int num_cells);
void clear_searched(Component **grid, int height, int width);

#endif