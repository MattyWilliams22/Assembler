#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include <stdint.h>
#include "assembleutils.h"

// Prototypes
binary assemble_DP(token_line line);
binary assemble_B(token_line line);
binary assemble_SDT(token_line line);
binary assemble_SP(token_line line);

typedef binary (*func_ptr)(token_line);

typedef struct {
  opcode_name opcode;
  func_ptr function;
  binary opc;
} AssembleMapping;

typedef enum {
  REG_OFF,
  PRE_IND,
  POST_IND,
  UNSIGNED_OFF
} addressing_mode;

#endif