#include "maze.h"
#include "global.h"
#include "bfs.h"
#include "a_star.h"
#include "dfs.h"
#include "game_utils.h"
#include "dijkstra.h"

double get_tick_speed();
void input();
int get_path(int *path);
void autonomous_loop();