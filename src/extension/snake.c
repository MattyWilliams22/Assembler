#include "snake.h"
#include "maze.h"
#include "global.h"
#include "bfs.h"
#include "a_star.h"
#include "dfs.h"
#include "game_utils.h"
#include "dijkstra.h"

double get_tick_speed() {
  double rate = 0.9;
  double multiplier = 1.0;
  for (int i = 0; i < nTail; i++) {
    multiplier *= rate;
  }
  return TICKDELAY * multiplier + 50000;
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

// Set path to an array of flags, 
// which determine the way the snake must move to reach the fruit. 
// Return the number of moves the snake must make to reach the fruit.
int get_path(int *path) {
  int **parent;
  parent = malloc(gridHeight * sizeof(*parent));
  for (int i = 0; i < gridHeight; i++) {
    parent[i] = malloc(gridWidth * sizeof(*parent[i]));
  }
  int path_length = 0;
  // Calculate path
  switch (mode) {
    case AUTONOMOUS_BFS_MODE:
      path_length = bfs(parent, y, x, fruitY, fruitX, path);
      break;
    case AUTONOMOUS_ASTAR_MODE:
      path_length = aStar(parent, y, x, fruitY, fruitX, path);
      break;
    case AUTONOMOUS_DIJKSTRA_MODE:
      path_length = dijkstra(parent, y, x, fruitY, fruitX, path);
      break;
    case AUTONOMOUS_DFS_MODE:
      path_length = dfs(parent, y, x, fruitY, fruitX, path);
      break;
    default:
      break;
  }
  for (int i = 0; i < gridHeight; i++) {
    free(parent[i]);
  }
  free(parent);
  return path_length;
}

void ai_loop() {
  int *path;
  path = malloc(gridHeight * gridWidth * sizeof(*path));
  int path_length;
  while (!gameover) {
    path_length = get_path(path);
    while (path_length > 0 && !gameover) {
      draw();
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
  } else {
    maze_setup();
  }

  if (mode == MANUAL_MODE) {
    while (1) {
      draw();
      input();
      logic();
      check_gameover();
      usleep(get_tick_speed());
    }
  } else {
    ai_loop();
  }

  if (gridType == STANDARD) {
    free_default_grid(game_grid, HEIGHT);
  } else {
    free_maze_grid(game_grid, mazeHeight, mazeSize);
  }
  return 0;
}
