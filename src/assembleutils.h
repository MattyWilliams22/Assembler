#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdint.h>

typedef enum {
  ADD,ADDS,SUB,SUBS,
  CMP,CMN,
  NEG,NEGS,
  AND,ANDS,BIC,BICS,EOR,ORR,EON,ORN,
  TST,
  MOVK,MOVN,MOVZ,
  MOV,
  MVN,
  MADD,MSUB,
  MUL,MNEG,
  B,BCOND,
  BR,
  STR,LDR,
  LDRLIT,
  NOP,
  DIR,
  HALT,
  LABEL_OPCODE,
  UNRECOGNISED_OPCODE
} opcode_name;

typedef enum {
  REG,
  // May need to differentiate between IMM and SIMM
  IMM,
  SHIFT,
  COND,
  UNRECOGNISED_OPERAND
} operand_type;

typedef struct {
  operand_type type;
  char *word;
} operand;

typedef struct {
  opcode_name opcode;
  operand *operands;
  int operand_count;
} token_line;

typedef struct {
  token_line *token_lines;
  int line_count;
} token_array;

typedef uint32_t binary;

#endif