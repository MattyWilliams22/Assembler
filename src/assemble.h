#include <stdio.h>
#include <stdint.h>
#include "structures.h"

typedef uint32_t binary;

typedef struct {
  binary *pieces;
  int piece_count;
} binary_line;