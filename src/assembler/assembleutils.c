#include "assembleutils.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Dynamically creates an operand in memory.
 *
 * @param type The type of the operand that is to be created: IMM, REG, LABEL, SHIFT, COND
 * @param word The actual value of the operand, which is to be strdup()ed into memory
 * @return The new malloc()ed operand
 */
operand *make_operand(operand_type type, char *word) {
  operand *new = malloc(sizeof(operand_type) + sizeof(char*));
  assert (new != NULL);
  new->type = type;
  new->word = strdup(word);
  return new;
}

/**
 * Frees the memory allocated operand and its value from memory.
 *
 * @param op The operand that is to be free()ed
 */
void free_operand(operand *op) {
  free(op->word);
  free(op);
}

/**
 * Dynamically creates an assembly token line in memory
 *
 * @param opcode The opcode the assembly instruction
 * @param op_count The number of operands that the instruction has
 * @param ops An array consisting of all of the operands of the instruction
 * @return The new malloc()ed token_line
 */
token_line *make_token_line(opcode_name opcode, int op_count, operand *ops) {
  token_line *new = malloc(sizeof(token_line));
  assert (new != NULL);
  new->opcode = opcode;
  new->operands = ops;
  new->operand_count = op_count;
  return new;
}

/**
 * Frees the memory allocated token line and its operands
 *
 * @param op The token line that is to be free()ed
 */
void free_token_line(token_line *line) {
  for (int i = 0; i < line->operand_count; i++) {
    free_operand(&line->operands[i]);
  }
  free(line->operands);
  free(line);
}