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
#define WIDTH 30
#define HEIGHT 30
#define UP 'w'
#define LEFT 'a'
#define DOWN 's'
#define RIGHT 'd'

enum Mode {
  MANUAL_MODE,
  AUTONOMOUS_BFS_MODE
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
  x = WIDTH / 2;
  y = HEIGHT / 2;
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
  while (game_grid[y][x] != EMPTY) {
    x = rand() % get_grid_width(mazeWidth, mazeSize);
    y = rand() % get_grid_height(mazeHeight, mazeSize);
  }
  game_grid[y][x] = HEAD;
  get_fruit(get_grid_height(mazeHeight, mazeSize), get_grid_width(mazeWidth, mazeSize));
  score = 0;
  nTail = 0;
  flag = -1;
  gridHeight = get_grid_height(mazeHeight, mazeSize);
  gridWidth = get_grid_width(mazeWidth, mazeSize);
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

bool isSafe(int bfs_grid[HEIGHT][WIDTH], int row, int col) {
  return (row >= 0 && row < HEIGHT && col >= 0 && col < WIDTH && bfs_grid[row][col] == 0);
}

void bfs(int bfs_grid[HEIGHT][WIDTH], int srcRow, int srcCol, int destRow, int destCol, int* nextMoveRow, int* nextMoveCol) {
  int dx[] = {0, 0, -1, 1};
  int dy[] = {-1, 1, 0, 0};
  
  bool visited[HEIGHT][WIDTH];
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      visited[i][j] = false;
    }
  }

  int queue[HEIGHT * WIDTH];
  int front = 0, rear = 0;

  visited[srcRow][srcCol] = true;
  queue[rear++] = srcRow * WIDTH + srcCol;

  int parent[HEIGHT][WIDTH];
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      parent[i][j] = -1;
    }
  }

  while (front != rear) {
    int currRow = queue[front] / WIDTH;
    int currCol = queue[front] % WIDTH;
    front++;

    if (currRow == destRow && currCol == destCol) {
      break;
    }

    for (int i = 0; i < 4; i++) {
      int newRow = currRow + dx[i];
      int newCol = currCol + dy[i];
      if (isSafe(bfs_grid, newRow, newCol) && !visited[newRow][newCol]) {
        visited[newRow][newCol] = true;
        queue[rear++] = newRow * WIDTH + newCol;
        parent[newRow][newCol] = currRow * WIDTH + currCol;
      }
    }
  }

  *nextMoveRow = destRow;
  *nextMoveCol = destCol;

  // Print the BFS search
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      if (visited[i][j]) {
        printf("#");
      } else {
        printf(" ");
      }
    }
    printf("\n");
  }
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

void autonomous_bfs() {
  int bfs_grid[HEIGHT][WIDTH];
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      bfs_grid[i][j] = 0;
    }
  }

  for (int i = 0; i < nTail; i++) {
    bfs_grid[tailY[i]][tailX[i]] = 1;
  }

  bfs_grid[y][x] = -1;
  bfs_grid[fruitY][fruitX] = -1;

  int nextMoveRow, nextMoveCol;
  bfs(bfs_grid, y, x, fruitY, fruitX, &nextMoveRow, &nextMoveCol);

  if (nextMoveRow < y) {
    flag = 1;
  } else if (nextMoveRow > y) {
    flag = 2;
  } else if (nextMoveCol < x) {
    flag = 3;
  } else if (nextMoveCol > x) {
    flag = 4;
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
  
  if (mode == AUTONOMOUS_BFS_MODE) {
    autonomous_bfs();
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
    if (gridType == MAZE) {
      get_fruit(get_grid_height(mazeHeight, mazeSize), get_grid_width(mazeWidth, mazeSize));
    } else {
      get_fruit(HEIGHT, WIDTH);
    }
    game_grid[tailY[nTail]][tailX[nTail]] = TAIL;
    nTail++;
  } else {
    game_grid[tailY[nTail]][tailX[nTail]] = EMPTY;
  }
  game_grid[y][x] = HEAD;
}

void check_gameover() {
  if (gameover) {
    // system("clear");
    printf("Game Over!\n");
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

int main() {
  srand(time(0));
  printf("Select the mode of operation:\n");
  printf("0. Manual Mode\n");
  printf("1. Autonomous BFS Mode\n");
  printf("Enter your choice: ");
  scanf("%d", &mode);
  printf("\nSelect the type of grid:\n");
  printf("0. Standard Snake\n");
  printf("1. Random Maze\n");
  printf("Enter your choice: ");
  scanf("%d", &gridType);

  while (getchar() != '\n') continue;  // Clear input buffer

  if (gridType == MAZE) {
    maze_setup();
    while (1) {
      draw_maze_grid(game_grid, mazeHeight, mazeWidth, mazeSize, nTail, score);
      input();
      logic();
      check_gameover();
      usleep(get_tick_speed());
    }
    free_maze_grid(game_grid, mazeHeight, mazeSize);
  } else {
    highscore = 0;
    setup();
    while (1) {
      draw_default_grid(game_grid, HEIGHT, WIDTH, nTail, score);
      input();
      logic();
      check_gameover();
      usleep(get_tick_speed());
    }
    free_default_grid(game_grid, HEIGHT);
  }
  return 0;
}
