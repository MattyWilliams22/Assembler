#include <stdint.h>

typedef uint32_t binary;

typedef struct {
  binary *pieces;
  int piece_count;
} binary_line;