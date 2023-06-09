#include "assembleutils.h"
#include <assert.h>
#include <stdlib.h>

operand *make_operand(operand_type type) {
  operand *new = malloc(sizeof(operand_type) + sizeof(char*));
  assert (new != NULL);
  new->type = type;
  new->word = malloc(sizeof(char) * 15);
  return new;
}

void free_operand(operand *op) {
  free(op->word);
  free(op);
}

token_line *make_token_line(opcode_name opcode, int op_count) {
  token_line *new = malloc(sizeof(opcode_name) + sizeof(operand*));
  assert (new != NULL);
  new->opcode = opcode;
  new->operands = malloc((sizeof(operand_type) + sizeof(char*)) * op_count);
  return new;
}

void free_token_line(token_line *line) {
  for (int i = 0; i < line->operand_count; i++) {
    free_operand(&line->operands[i]);
  }
  free(line->operands);
  free(line);
}

token_array *make_token_array(int line_count) {
  token_array *new = malloc(sizeof(token_line*) + sizeof(int));
  assert (new != NULL);
  new->token_lines = malloc((sizeof(operand_type) + sizeof(char*)) * 6 * line_count);
  new->line_count = 0;
  return new;
}

void free_token_array(token_array *array) {
  for (int i = 0; i < array->line_count; i++) {
    free_token_line(&array->token_lines[i]);
  }
  free(array->token_lines);
  free(array);
}