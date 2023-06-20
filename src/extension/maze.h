typedef enum {
  WALL,
  HEAD,
  TAIL,
  FRUIT,
  EMPTY  
} Component;

typedef struct {
    int px;
    int py;
} Cell;

void setup_grid(int size, int height, int width, Component **grid);
void make_maze(int size, int height, int width, Component **grid);
void draw_grid(Component **grid, int height, int width, int size);
Component **allocate_grid(int height, int width, int size);
void free_grid(Component **grid, int height, int size);