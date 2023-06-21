#include "assemblesetup.h"
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/**
 * Count the number of lines in a given file given the filename.
 *
 * @param filename The name of the file to count the lines in
 * @return The number of lines in the file
 */
int count_lines(FILE *fp) {
  int count = 0;
  char line[100];

  // Read in file line by line until the end of the file
  while ((fgets(line, sizeof(line), fp)) != NULL) {
    // Remove trailing newline if there is one
    if (line[strlen(line) - 1] == '\n') {
      line[strlen(line) - 1] = '\0';
    }

    // Removes spaces
    int start = 0;
    while (isspace(line[start])) {
      start++;
    }

    if (strlen(&line[start]) != 0) {
      count += 1;
    }
  }

  // Put the file pointer back to the beginning of the file
  rewind(fp);
  
  return count;
}

/**
 * Writes an array of binary to the file with the given file name.
 *
 * @param filename The name of the file to write to
 * @param binary_lines The array of binary lines to write to file
 * @param nlines The number of lines which are to be written to the file
 * @param token_lines The array of parsed assembly instruction lines
 */
void write_to_binary_file(char *filename, binary *binary_lines, int nlines, token_line *token_lines) {
  FILE* fp = fopen(filename, "wb+");
	if (fp == NULL) {
		perror("Could not open output file.");
		exit(EXIT_FAILURE);
	}

  // Check if the binary pointer is valid
  if (binary_lines == NULL) {
    perror("Invalid binary_lines pointer\n");
    return;
  }

  // Write the binary data to the file
  for (int i = 0; i < nlines; i++) {
    if (binary_lines[i] != 0 || token_lines[i].opcode == DIR) {
      fwrite(&binary_lines[i], sizeof(binary), 1, fp);
    }
  }

  fclose(fp);
}

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
  new->operand_count = op_count;

  // Create a dynamically allocated array of operands
  new->operands =  malloc(op_count * sizeof(operand));
  for (int i = 0; i < op_count; i++) {
    new->operands[i] = *make_operand(ops[i].type, ops[i].word);
  }
  
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