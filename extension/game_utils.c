#include "game_utils.h"

/**
 * Checks for a key press by the user. 
 * Does not wait for the user to press enter.
 */
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

/**
 * Sets the position of the fruit to a random empty position on the game_grid.
 * 
 * @param height The height of the grid
 * @param width The width of the grid
 */
void get_fruit(int height, int width) {
  while (game_grid[fruitY][fruitX] != EMPTY) {
    fruitX = rand() % width;
    fruitY = rand() % height;
  }
  game_grid[fruitY][fruitX] = FRUIT;
}

/**
 * Sets the position of the snakes head to a random empty position on the game_grid.
 * 
 * @param height The height of the grid
 * @param width The width of the grid
 */
void get_start_point(int height, int width) {
  while (game_grid[y][x] != EMPTY) {
    x = rand() % width;
    y = rand() % height;
  }
  game_grid[y][x] = HEAD;
}

/**
 * Sets gameover to 1 if the head of the snake had collided into a wall or its own tail.
 */
void check_collision() {
  if (game_grid[y][x] == WALL || game_grid[y][x] == TAIL) {
    gameover = 1;
  }
}

/**
 * Checks if the head of the snake has reached the edge of the grid,
 * in which case it teleports to the other side of the grid.
 */
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
 
/**
 * Contains the logic required to move the snake by one position.
 * The direction of movement is determined by the current state of the flag.
 */
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

  check_collision();

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

/**
 * Sets up the game variables ready to start a game of snake using the default grid.
 */
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

/**
 * Sets up the game variables ready to start a game of snake using a randomly generated maze as the grid.
 */
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

/**
 * If the gameover variable is set to 1, ask the user if they want to restart or exit.
 */
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