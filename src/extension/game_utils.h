#ifndef GAME_UTILS_H
#define GAME_UTILS_H

#include "global.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include "maze.h"

int keyboard_event();
void get_fruit(int height, int width);
void get_start_point(int height, int width);
void check_collision_default();
void check_collision_maze();
void check_teleport();
void logic();
void setup();
void maze_setup();
void check_gameover();

#endif