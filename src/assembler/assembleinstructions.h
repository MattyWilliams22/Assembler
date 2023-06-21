#ifndef ASSEMBLEINSTRUCTIONS_H
#define ASSEMBLEINSTRUCTIONS_H

#include <stdint.h>
#include <stdio.h>
#include "assemblesetup.h"

#define HALTINSTR 0x8a000000
#define NOOP 0xd503201f

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

// Prototypes
binary set_bits(binary input, int start, int end, binary value);
char *remove_first_char(char *str);
void remove_last_char(char *str);
binary convert_SHIFT(operand op);
binary convert_OPCODE(opcode_name name);
binary convert_REG(operand op);
binary convert_IMM(operand op);
binary get_shift_amount(operand op);
binary convert_COND(operand op);
binary convert_ADDR_MODE(operand op, addressing_mode mode);
binary assemble_DP(token_line line);
binary assemble_B(token_line line);
binary assemble_SDT(token_line line);
binary assemble_SP(token_line line);
addressing_mode get_addressing_mode(token_line line);
binary assemble_line(token_line line);

#endif