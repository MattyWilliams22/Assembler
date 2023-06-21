#ifndef SETUP_H
#define SETUP_H

#include <stdint.h>
#include <stdio.h>

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
  LABEL,
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

typedef uint32_t binary;

int count_lines(FILE *fp);
void write_to_binary_file(char *filename, binary *binary_lines, int nlines, token_line *token_lines);
operand *make_operand(operand_type type, char *word);
void free_operand(operand *op);
token_line *make_token_line(opcode_name opcode, int op_count, operand* operands);
void free_token_line(token_line *line);

#endif