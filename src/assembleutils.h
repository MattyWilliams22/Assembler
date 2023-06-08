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

operand make_operand(operand_type type, char* word) {
  operand new = malloc(sizeof(operand_type) + sizeof(char*));
  assert (new != NULL);
  new->type = type;
  int length = strlen(word);
  new->word = malloc(sizeof(char) * length); 
}

void free_operand(operand op) {
  free(op.word[0]);
  free(op.word);
  free(op);
}

typedef struct {
  opcode_name opcode;
  operand *operands;
  int operand_count;
} token_line;

token_line make_token_line(opcode_name opcode, operand *operands, int op_count) {
  token_line new = malloc(sizeof(opcode_name) + sizeof(operand*));
  assert (new != NULL);
  new->opcode = opcode;
  new->operands = malloc((sizeof(operand_type) + sizeof(char*)) * op_count);
}

void free_token_line(token_line line) {
  free(line.operands[0]);
  free(line.operands);
  free(line);
}

typedef struct {
  token_line *token_lines;
  int line_count;
} token_array;

token_array make_token_array(token_line *token_lines, int line_count) {
  token_array new = malloc(sizeof(token_line*) + sizeof(int));
  new->token_lines = malloc((sizeof(operand_type) + sizeof(char*)) * 6 * line_count);
  new->line_count = line_count;
}

void free_token_array(token_array array) {
  free(array.token_lines[0]);
  free(array.token_lines);
  free(array);
}

typedef uint32_t binary;

#endif