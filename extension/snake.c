#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#define WIDTH 20
#define HEIGHT 20
#define UP 'w'
#define LEFT 'a'
#define DOWN 's'
#define RIGHT 'd'

int score;
int gameover;
int x, y;
int fruitX, fruitY;
int flag;

int tailX[100], tailY[100];
int nTail;

int keyboard_event() {
  struct termios oldterminal, newterminal;
  int ch;
  int original_flags;

  tcgetattr(STDIN_FILENO, &oldterminal);
  newterminal = oldterminal;
  newterminal.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newterminal);
  original_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, original_flags | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldterminal);
  fcntl(STDIN_FILENO, F_SETFL, original_flags);

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
}

void draw() {
  system("clear");
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
      }
      else if (i == fruitY && j == fruitX) {
        printf("X");
      }
      else {
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
  printf("Score: %d", score);
}

void input() {
  if (keyboard_event()) {
    char key = getchar();
    switch (key) {
    case UP:
      flag = 1;
      break;
    case DOWN:
      flag = 2;
      break;
    case LEFT:
      flag = 3;
      break;
    case RIGHT:
      flag = 4;
      break;
    case 'x':
      gameover = 1;
      break;
    }
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
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
    gameover = 1;
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

int main() {
  srand(time(0));

  setup();
  while (!gameover) {
    draw();
    input();
    logic();
    usleep(100000); // Delay controls speed of game
  }
  printf("\nGame Over!\n");

  return 0;
}
