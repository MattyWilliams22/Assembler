#ifndef GLOBAL_H
#define GLOBAL_H

extern int score;
extern int highscore;
extern int gameover;
extern int x, y, fruitX, fruitY, flag;
extern int mode;
extern int gridType;
extern int mazeHeight;
extern int mazeWidth;
extern int mazeSize;
extern int gridHeight;
extern int gridWidth;

extern int tailX[100], tailY[100];
extern int nTail;

enum GridType {
  STANDARD,
  MAZE
};

enum Mode {
  MANUAL_MODE,
  AUTONOMOUS_BFS_MODE,
  AUTONOMOUS_ASTAR_MODE,
  AUTONOMOUS_DFS_MODE,
  AUTONOMOUS_DIJKSTRA_MODE
};

typedef enum {
  WALL,
  HEAD,
  TAIL,
  FRUIT,
  EMPTY,
  PATH_ELEM,
  SEARCHED  
} Component;

extern Component **game_grid;

#define TICKDELAY 100000.0
#define SEARCHDELAY 10000
#define WIDTH 30
#define HEIGHT 30
#define UP 'w'
#define LEFT 'a'
#define DOWN 's'
#define RIGHT 'd'

#endif