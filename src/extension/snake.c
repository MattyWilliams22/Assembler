#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdbool.h>
#include "snake.h"
#include "maze.h"

#define TICKDELAY 100000.0
#define SEARCHDELAY 10000
#define WIDTH 30
#define HEIGHT 30
#define UP 'w'
#define LEFT 'a'
#define DOWN 's'
#define RIGHT 'd'

enum Mode {
  MANUAL_MODE,
  AUTONOMOUS_BFS_MODE,
  AUTONOMOUS_ASTAR_MODE,
  AUTONOMOUS_DFS_MODE,
  AUTONOMOUS_DIJKSTRA_MODE
};

enum GridType {
  STANDARD,
  MAZE
};

int score;
int highscore;
int gameover;
int x, y, fruitX, fruitY, flag;
int mode;
int gridType;
int mazeHeight;
int mazeWidth;
int mazeSize;
int gridHeight;
int gridWidth;

int tailX[100], tailY[100];
int nTail;

Component **game_grid;

double get_tick_speed() {
  double rate = 0.9;
  double multiplier = 1.0;
  for (int i = 0; i < nTail; i++) {
    multiplier *= rate;
  }
  return TICKDELAY * multiplier + 50000;
}

void get_fruit(int height, int width) {
  while (game_grid[fruitY][fruitX] != EMPTY) {
    fruitX = rand() % width;
    fruitY = rand() % height;
  }
  game_grid[fruitY][fruitX] = FRUIT;
}

void get_start_point(int height, int width) {
  while (game_grid[y][x] != EMPTY) {
    x = rand() % width;
    y = rand() % height;
  }
  game_grid[y][x] = HEAD;
}

int keyboard_event() {
  struct termios oldterminal, newterminal;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldterminal);
  newterminal = oldterminal;
  newterminal.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newterminal);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldterminal);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if (ch != EOF) {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

void setup() {
  system("clear");
  gameover = 0;
  score = 0;
  nTail = 0;
  flag = -1;
  int i, j;
  game_grid = allocate_default_grid(HEIGHT, WIDTH);
  for (i = 0; i < WIDTH; i++) {
    game_grid[0][i] = WALL; 
  }
  for (i = 0; i < HEIGHT; i++) {
    for (j = 0; j < WIDTH; j++) {
      if (j == 0 || j == WIDTH - 1 || i == 0 || i == HEIGHT - 1) {
        game_grid[i][j] = WALL; 
      } else {
        game_grid[i][j] = EMPTY; 
      }
    }
  }
  get_start_point(HEIGHT, WIDTH);
  get_fruit(HEIGHT, WIDTH);
  gridHeight = HEIGHT;
  gridWidth = WIDTH;
}

void maze_setup() {
  printf("Enter maze height: ");
  scanf("%d", &mazeHeight);
  printf("Enter maze width: ");
  scanf("%d", &mazeWidth);
  printf("Enter maze size: ");
  scanf("%d", &mazeSize);
  Component **maze_grid = allocate_maze_grid(mazeHeight, mazeWidth, mazeSize);
  make_maze(mazeSize, mazeHeight, mazeWidth, maze_grid);
  game_grid = maze_grid;
  system("clear");
  gameover = 0;
  x = 0;
  y = 0;
  gridHeight = get_grid_height(mazeHeight, mazeSize);
  gridWidth = get_grid_width(mazeWidth, mazeSize);
  get_start_point(gridHeight, gridWidth);
  get_fruit(gridHeight, gridWidth);
  score = 0;
  nTail = 0;
  flag = -1;
}

void draw() {
  system("clear");
  if (nTail > 0) {
    printf("Score: %d\n", score);
  } else {
    printf("Welcome to snake!\n");
  }
  
  int i, j;
  for (i = 0; i < WIDTH + 2; i++)
    printf("#");
  printf("\n");

  for (i = 0; i < HEIGHT; i++) {
    for (j = 0; j < WIDTH; j++) {
      if (j == 0) {
        printf("#");
      }
      if (i == y && j == x) {
        printf("O");
      } else if (i == fruitY && j == fruitX) {
        printf("X");
      } else {
        int print = 0;
        for (int k = 0; k < nTail; k++) {
          if (i == tailY[k] && j == tailX[k]) {
            printf("o");
            print = 1;
          }
        }
        if (!print) {
          printf(" ");
        }
      }
      if (j == WIDTH - 1) {
        printf("#");
      }
    }
    printf("\n");
  }

  for (i = 0; i < WIDTH + 2; i++) {
    printf("#");
  }
  printf("\n");

  if (nTail == 0) {
    printf("Use WASD or Arrow Keys to move\n");
  }
}

void input() {
  if (keyboard_event()) {
    char key = getchar();
    int arrow = 0;
    if (key == 27) {
      getchar();
      arrow = getchar();
    }
    if ((key == UP || arrow == 65) && flag != 2) {
      flag = 1;
    } else if ((key == DOWN || arrow == 66) && flag != 1) {
      flag = 2;
    } else if ((key == LEFT || arrow == 68) && flag != 4) {
      flag = 3;
    } else if ((key == RIGHT || arrow == 67) && flag != 3) {
      flag = 4;
    } else if (key == 'x') {
      gameover = 1;
    }
  }
}

bool isSafe(int row, int col) {
  if (row >= 0 && row < gridHeight && col >= 0 && col < gridWidth) {
    return (game_grid[row][col] != WALL && game_grid[row][col] != TAIL && game_grid[row][col] != HEAD);
  }
  return false;
}

int bfs(int srcRow, int srcCol, int destRow, int destCol, int *path) {
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
    if (gridType == STANDARD) {
      draw_default_grid(game_grid, HEIGHT, WIDTH, nTail, score);
    } else {
      draw_maze_grid(game_grid, mazeHeight, mazeWidth, mazeSize, nTail, score);
    }
  }

  queue[rear++] = srcRow * gridWidth + srcCol;

  int parent[gridHeight][gridWidth];
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
          if (gridType == STANDARD) {
            draw_default_grid(game_grid, HEIGHT, WIDTH, nTail, score);
          } else {
            draw_maze_grid(game_grid, mazeHeight, mazeWidth, mazeSize, nTail, score);
          }
        }

        queue[rear++] = newRow * WIDTH + newCol;
        parent[newRow][newCol] = currRow * WIDTH + currCol;
      }
    }
  }

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
  printf("Here is the shortest path to the fruit!\n");
  printf("Press any key to continue... \n");
  getchar();
  clear_searched(game_grid, gridHeight, gridWidth);
  return path_length;
}

int calculateHCost(int row, int col, int destRow, int destCol) {
  return abs(destRow - row) + abs(destCol - col);
}

int aStar(int srcRow, int srcCol, int destRow, int destCol, int *path) {
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
    if (gridType == STANDARD) {
      draw_default_grid(game_grid, HEIGHT, WIDTH, nTail, score);
    } else {
      draw_maze_grid(game_grid, mazeHeight, mazeWidth, mazeSize, nTail, score);
    }
  }

  queue[rear++] = srcRow * gridWidth + srcCol;

  int parent[gridHeight][gridWidth];
  for (int i = 0; i < gridHeight; i++) {
    for (int j = 0; j < gridWidth; j++) {
      parent[i][j] = -1;
    }
  }

  int gCost[gridHeight][gridWidth];
  for (int i = 0; i < gridHeight; i++) {
    for (int j = 0; j < gridWidth; j++) {
      gCost[i][j] = INT_MAX;
    }
  }
  gCost[srcRow][srcCol] = 0;

  while (front != rear) {
    int minFCost = INT_MAX;
    int currNode;
    for (int i = front; i < rear; i++) {
      int row = queue[i] / gridWidth;
      int col = queue[i] % gridWidth;
      int fCost = gCost[row][col] + calculateHCost(row, col, destRow, destCol);
      if (fCost < minFCost) {
        minFCost = fCost;
        currNode = i;
      }
    }

    int currRow = queue[currNode] / gridWidth;
    int currCol = queue[currNode] % gridWidth;

    if (currRow == destRow && currCol == destCol) {
      break;
    }

    queue[currNode] = queue[front];
    front++;

    for (int i = 0; i < 4; i++) {
      int newRow = currRow + dx[i];
      int newCol = currCol + dy[i];
      if (isSafe(newRow, newCol) && !visited[newRow][newCol]) {
        visited[newRow][newCol] = true;

        if (game_grid[newRow][newCol] == EMPTY) {
          game_grid[newRow][newCol] = SEARCHED;
          usleep(SEARCHDELAY);
          if (gridType == STANDARD) {
            draw_default_grid(game_grid, HEIGHT, WIDTH, nTail, score);
          } else {
            draw_maze_grid(game_grid, mazeHeight, mazeWidth, mazeSize, nTail, score);
          }
        }

        queue[rear++] = newRow * WIDTH + newCol;
        parent[newRow][newCol] = currRow * WIDTH + currCol;

        int newGCost = gCost[currRow][currCol] + 1;
        if (newGCost < gCost[newRow][newCol]) {
          gCost[newRow][newCol] = newGCost;
        }
      }
    }
  }

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
  printf("Here is the shortest path to the fruit!\n");
  printf("Press any key to continue... \n");
  getchar();
  clear_searched(game_grid, gridHeight, gridWidth);
  return path_length;
}

int dijkstra(int srcRow, int srcCol, int destRow, int destCol, int *path) {
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
    if (gridType == STANDARD) {
      draw_default_grid(game_grid, HEIGHT, WIDTH, nTail, score);
    } else {
      draw_maze_grid(game_grid, mazeHeight, mazeWidth, mazeSize, nTail, score);
    }
  }

  queue[rear++] = srcRow * gridWidth + srcCol;

  int parent[gridHeight][gridWidth];
  for (int i = 0; i < gridHeight; i++) {
    for (int j = 0; j < gridWidth; j++) {
      parent[i][j] = -1;
    }
  }

  int gCost[gridHeight][gridWidth];
  for (int i = 0; i < gridHeight; i++) {
    for (int j = 0; j < gridWidth; j++) {
      gCost[i][j] = INT_MAX;
    }
  }
  gCost[srcRow][srcCol] = 0;

  while (front != rear) {
    int minGCost = INT_MAX;
    int currNode;
    for (int i = front; i < rear; i++) {
      int row = queue[i] / gridWidth;
      int col = queue[i] % gridWidth;
      if (gCost[row][col] < minGCost) {
        minGCost = gCost[row][col];
        currNode = i;
      }
    }

    int currRow = queue[currNode] / gridWidth;
    int currCol = queue[currNode] % gridWidth;

    if (currRow == destRow && currCol == destCol) {
      break;
    }

    queue[currNode] = queue[front];
    front++;

    for (int i = 0; i < 4; i++) {
      int newRow = currRow + dx[i];
      int newCol = currCol + dy[i];
      if (isSafe(newRow, newCol) && !visited[newRow][newCol]) {
        visited[newRow][newCol] = true;

        if (game_grid[newRow][newCol] == EMPTY) {
          game_grid[newRow][newCol] = SEARCHED;
          usleep(SEARCHDELAY);
          if (gridType == STANDARD) {
            draw_default_grid(game_grid, HEIGHT, WIDTH, nTail, score);
          } else {
            draw_maze_grid(game_grid, mazeHeight, mazeWidth, mazeSize, nTail, score);
          }
        }

        queue[rear++] = newRow * WIDTH + newCol;
        parent[newRow][newCol] = currRow * WIDTH + currCol;

        int newGCost = gCost[currRow][currCol] + 1;
        if (newGCost < gCost[newRow][newCol]) {
          gCost[newRow][newCol] = newGCost;
        }
      }
    }
  }

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
  printf("Here is the shortest path to the fruit!\n");
  printf("Press any key to continue... \n");
  getchar();
  clear_searched(game_grid, gridHeight, gridWidth);
  return path_length;
}

int dfs(int srcRow, int srcCol, int destRow, int destCol, int *path) {
  int dx[] = {0, 0, -1, 1};
  int dy[] = {-1, 1, 0, 0};
  
  bool visited[gridHeight][gridWidth];
  for (int i = 0; i < gridHeight; i++) {
    for (int j = 0; j < gridWidth; j++) {
      visited[i][j] = false;
    }
  }

  int stack[gridHeight * gridWidth];
  int top = -1;

  visited[srcRow][srcCol] = true;

  if (game_grid[srcRow][srcCol] == EMPTY) {
    game_grid[srcRow][srcCol] = SEARCHED;
    if (gridType == STANDARD) {
      draw_default_grid(game_grid, HEIGHT, WIDTH, nTail, score);
    } else {
      draw_maze_grid(game_grid, mazeHeight, mazeWidth, mazeSize, nTail, score);
    }
  }

  stack[++top] = srcRow * gridWidth + srcCol;

  int parent[gridHeight][gridWidth];
  for (int i = 0; i < gridHeight; i++) {
    for (int j = 0; j < gridWidth; j++) {
      parent[i][j] = -1;
    }
  }

  while (top != -1) {
    int currRow = stack[top] / gridWidth;
    int currCol = stack[top] % gridWidth;
    top--;

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
          if (gridType == STANDARD) {
            draw_default_grid(game_grid, HEIGHT, WIDTH, nTail, score);
          } else {
            draw_maze_grid(game_grid, mazeHeight, mazeWidth, mazeSize, nTail, score);
          }
        }

        stack[++top] = newRow * WIDTH + newCol;
        parent[newRow][newCol] = currRow * WIDTH + currCol;
      }
    }
  }

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
  printf("Path found (it might not be the shortest)!\n");
  printf("Press any key to continue... \n");
  getchar();
  clear_searched(game_grid, gridHeight, gridWidth);
  return path_length;
}


void check_collision_default() {
    if (game_grid[y][x] == TAIL || game_grid[y][x] == WALL) {
      gameover = 1;
    }
}

void check_collision_maze() {
  if (game_grid[y][x] == WALL || game_grid[y][x] == TAIL) {
    gameover = 1;
  }
}

void check_teleport() {
  if (game_grid[y][x] == WALL) {
    if (x == 0) {
      x = WIDTH - 2;
    } else if (x == WIDTH - 1) {
      x = 1;
    } else if (y == 0) {
      y = HEIGHT - 2;
    } else if (y == HEIGHT - 1) {
      y = 1;
    }
  }
}
 
void logic() {
  for (int i = nTail; i > 0; i--) {
    tailX[i] = tailX[i - 1];
    tailY[i] = tailY[i - 1];
  }
  if (nTail > 0) {
    tailX[0] = x;
    tailY[0] = y;
    game_grid[y][x] = TAIL;
  } else {
    game_grid[y][x] = EMPTY;
  }
  
  switch (flag) {
    case 1:
      y--;
      break;
    case 2:
      y++;
      break;
    case 3:
      x--;
      break;
    case 4:
      x++;
      break;
    default:
      break;
  }

  if (gridType == STANDARD && mode == MANUAL_MODE) {
    check_teleport();
  }

  if (gridType == MAZE) {
    check_collision_maze();
  } else {
    check_collision_default();
  }

  if (game_grid[y][x] == FRUIT) {
    score += 10;
    get_fruit(gridHeight, gridWidth);
    game_grid[tailY[nTail]][tailX[nTail]] = TAIL;
    nTail++;
  } else {
    game_grid[tailY[nTail]][tailX[nTail]] = EMPTY;
  }
  game_grid[y][x] = HEAD;
}

void check_gameover() {
  if (gameover) {
    printf("\nGame Over!\n");
    if (score > highscore) {
      highscore = score;
      printf("New Highscore: %d\n", highscore);
    } else {
      printf("Score: %d\n", score);
      printf("Highscore: %d\n", highscore);
    }
    printf("Press 'r' to restart or 'x' to exit\n");
    while (1) {
      if (keyboard_event()) {
        char key = getchar();
        if (key == 'r') {
          if (gridType == MAZE) {
            maze_setup();
          } else {
            setup();
          }
          break;
        } else if (key == 'x') {
          exit(0);
        }
      }
    }
  }
}

// Set path to an array of flags, 
// which determine the way the snake must move to reach the fruit. 
// Return the number of moves the snake must make to reach the fruit.
int get_path(int *path) {
  int path_length = 0;
  // Calculate path
  switch (mode) {
    case AUTONOMOUS_BFS_MODE:
      path_length = bfs(y, x, fruitY, fruitX, path);
      break;
    case AUTONOMOUS_ASTAR_MODE:
      path_length = aStar(y, x, fruitY, fruitX, path);
      break;
    case AUTONOMOUS_DIJKSTRA_MODE:
      path_length = dijkstra(y, x, fruitY, fruitX, path);
      break;
    case AUTONOMOUS_DFS_MODE:
      path_length = dfs(y, x, fruitY, fruitX, path);
      break;
    default:
      break;
  }
  return path_length;
}

void ai_loop() {
  int *path;
  path = malloc(gridHeight * gridWidth * sizeof(*path));
  int path_length;
  while (!gameover) {
    path_length = get_path(path);
    while (path_length > 0 && !gameover) {
      if (gridType == STANDARD) {
        draw_default_grid(game_grid, HEIGHT, WIDTH, nTail, score);
      } else {
        draw_maze_grid(game_grid, mazeHeight, mazeWidth, mazeSize, nTail, score);
      }
      if (keyboard_event()) {
        char key = getchar();
        if (key == 'x') {
          exit(0);
        }
      }
      flag = path[path_length - 1];
      logic();
      usleep(get_tick_speed());
      path_length--;
    }
    check_gameover();
  }
  free(path);
}

int main() {
  system("clear");
  printf("Select the type of grid:\n");
  printf("0. Standard Snake\n");
  printf("1. Random Maze\n");
  printf("Enter your choice: ");
  scanf("%d", &gridType);

  printf("\nSelect the mode of operation:\n");
  printf("0. Manual Mode\n");
  printf("1. Autonomous BFS Mode\n");
  printf("2. Autonomous A* Mode\n");
  printf("3. Autonomous DFS Mode\n");
  printf("4. Autonomous Dijkstra Mode\n");
  printf("Enter your choice: ");
  scanf("%d", &mode);
  
  while (getchar() != '\n') continue;  // Clear input buffer

  highscore = 0;
  if (gridType == STANDARD) {
    setup();
    if (mode == MANUAL_MODE) {
      while (1) {
        draw_default_grid(game_grid, HEIGHT, WIDTH, nTail, score);
        input();
        logic();
        check_gameover();
        usleep(get_tick_speed());
      }
    } else {
      ai_loop();
    }
    free_default_grid(game_grid, HEIGHT);
  } else if (gridType == MAZE) {
    maze_setup();
    if (mode == MANUAL_MODE) {
      while (1) {
        draw_maze_grid(game_grid, mazeHeight, mazeWidth, mazeSize, nTail, score);
        input();
        logic();
        check_gameover();
        usleep(TICKDELAY);
      }
    } else {
      ai_loop();
    }
    free_maze_grid(game_grid, mazeHeight, mazeSize);
  }
  return 0;
}
