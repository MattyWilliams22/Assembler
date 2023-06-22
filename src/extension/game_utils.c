#include "game_utils.h"

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
  printf("Enter maze height/width: ");
  scanf("%d", &mazeHeight);
  mazeWidth = mazeHeight;
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