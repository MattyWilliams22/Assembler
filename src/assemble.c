#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include "assemble.h"
#include "symbolTable.h"
#include "tokenizer.h"

// Applies the shift to the binary input
binary do_SHIFT(operand shift, binary input) {
  
}


// Converts an opcode to its binary representation
binary convert_OPCODE(opcode_name name) {

}

// Converts a register to its binary representation
binary convert_REG(operand op) {
  
}

// Converts an immediate value to its binary representation
binary convert_IMM(operand op) {
  
}

// Converts an address to its binary representation
binary convert_ADDR(operand op) {
  
}

// Converts a label to its binary representation
binary convert_LABEL(operand op) {
  
}

// Converts an operand to its binary representation
binary convert_OPERAND(operand op) {

}

// Counts the number of lines of text in the file given by fp
int count_lines(FILE *fp) {
  int count = 0;
  char c;
  for (c = getc(fp); c != EOF; c = getc(fp)) {
    if (c == '\n') { 
      count = count + 1; 
    }
  }
  return count;
}

// Writes an array of binary_lines to the file given by fp
void write_to_binary_file(FILE *fp, binary_line *binary_lines, int nlines) {
  // Check if the file pointer is valid
  if (fp == NULL) {
    printf("Invalid file pointer\n");
    return;
  }

  // Check if the binary_lines pointer is valid
  if (binary_lines == NULL) {
    printf("Invalid binary_lines pointer\n");
    return;
  }

  // Write the binary data to the file
  for (int i = 0; i < nlines; i++) {
    fwrite(binary_lines[i].pieces, sizeof(binary), binary_lines[i].piece_count, fp);
  }
}

int main(int argc, char **argv) {

  FILE* input = fopen(argv[1], "rt");
	if (input == NULL) {
		perror("Could not open input file.");
		exit(EXIT_FAILURE);
	}

  // Get number of lines in input file
  int nlines = count_lines(input);

  // Convert lines of file to an array of token_lines
  token_line *token_lines = read_assembly(input, nlines);

  // Convert token_lines to binary_lines
  binary *binary_lines[nlines];
  for (int i = 0; i < nlines; i++) {
    token_line current_t_line = token_lines[i];
    binary_line current_b_line;
    current_b_line.pieces[0] = convert_OPCODE(current_t_line.opcode);
    current_b_line.piece_count = 1;
    for (int j = 0; j < current_t_line.operand_count; j++) {
      current_b_line.pieces[current_b_line.piece_count] = convert_OPERAND(current_t_line.operands[j]);
    }
  }


  FILE* output = fopen(argv[2], "wb+");
	if (output == NULL) {
		perror("Could not open output file.");
		exit(EXIT_FAILURE);
	}

  // Writes binary_lines to output file
  write_to_binary_file(output, binary_lines, nlines);

  return EXIT_SUCCESS;
}
