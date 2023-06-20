#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdbool.h>

#define TICKDELAY 100000.0
#define WIDTH 15
#define HEIGHT 15
#define UP 'w'
#define LEFT 'a'
#define DOWN 's'
#define RIGHT 'd'

enum Mode {
  MANUAL_MODE,
  AUTONOMOUS_BFS_MODE  
};

int score;
int highscore;
int gameover;
int x, y, fruitX, fruitY, flag;
int mode;

int tailX[100], tailY[100];
int nTail;

double get_tick_speed() {
  double rate = 0.9;
  double multiplier = 1.0;
  for (int i = 0; i < nTail; i++) {
    multiplier *= rate;
  }
  return TICKDELAY * multiplier + 50000;
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
  fruitX = rand() % WIDTH;
  fruitY = rand() % HEIGHT;
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

bool isSafe(int grid[HEIGHT][WIDTH], int row, int col) {
  return (row >= 0 && row < HEIGHT && col >= 0 && col < WIDTH && grid[row][col] == 0);
}

void bfs(int grid[HEIGHT][WIDTH], int srcRow, int srcCol, int destRow, int destCol, int* nextMoveRow, int* nextMoveCol) {
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
      if (isSafe(grid, newRow, newCol) && !visited[newRow][newCol]) {
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

void autonomous_bfs() {
  int grid[HEIGHT][WIDTH];
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      grid[i][j] = 0;
    }
  }

  for (int i = 0; i < nTail; i++) {
    grid[tailY[i]][tailX[i]] = 1;
  }

  grid[y][x] = -1;
  grid[fruitY][fruitX] = -1;

  int nextMoveRow, nextMoveCol;
  bfs(grid, y, x, fruitY, fruitX, &nextMoveRow, &nextMoveCol);

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

void logic() {
  int prevX = tailX[0];
  int prevY = tailY[0];
  int prev2X, prev2Y;
  tailX[0] = x;
  tailY[0] = y;
  for (int i = 1; i < nTail; i++) {
    prev2X = tailX[i];
    prev2Y = tailY[i];
    tailX[i] = prevX;
    tailY[i] = prevY;
    prevX = prev2X;
    prevY = prev2Y;
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

  if (x >= WIDTH) {
    x = 0;
  } else if (x < 0) {
    x = WIDTH - 1;
  }

  if (y >= HEIGHT) {
    y = 0;
  } else if (y < 0) {
    y = HEIGHT - 1;
  }

  for (int i = 0; i < nTail; i++) {
    if (tailX[i] == x && tailY[i] == y) {
      gameover = 1;
      break;
    }
  }

  if (x == fruitX && y == fruitY) {
    score += 10;
    fruitX = rand() % WIDTH;
    fruitY = rand() % HEIGHT;
    nTail++;
  }
}

void check_gameover() {
  if (gameover) {
    system("clear");
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
          setup();
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

  while (getchar() != '\n') continue;  // Clear input buffer
  highscore = 0;
  setup();
  while (1) {
    draw();
    input();
    logic();
    check_gameover();
    usleep(get_tick_speed());
  }
  return 0;
}
