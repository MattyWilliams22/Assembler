#ifndef GAME_UTILS_H
#define GAME_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include "maze.h"
#include "global.h"

int keyboard_event();
void get_fruit(int height, int width);
void get_start_point(int height, int width);
void check_collision();
void check_teleport();
void logic();
void setup();
void maze_setup();
void check_gameover();

#endif