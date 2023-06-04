#include <stdbool.h>
#include <stdlib.h>
#include "assemble.h"
#include "symbolTable.h"
#include "tokenizer.h"

#define HALT 0x8a000000
#define NOOP 0xd503201f

// Applies the shift to the binary input
binary do_SHIFT(operand shift, binary input) {
  if (shift.type != SHIFT) {
    return NULL;
  }
}


// Converts an opcode to its binary representation
binary convert_OPCODE(opcode_name name) {

}

// Converts a register to its binary representation
binary convert_REG(operand op) {
  if (op.type != REG) {
    return NULL;
  }
}

// Converts an immediate value to its binary representation
binary convert_IMM(operand op) {
  // Need to handle if IMM is signed or not


  if (op.type != IMM) {
    return NULL;
  }

  if (op.word[0] == '0' && op.word[1] == 'x') {
    // hex
    return (int)strtol(&op.word[2], NULL, 16);
  } else {
    // decimal
    return atoi(op.word);
  }
}

// Converts an address to its binary representation
binary convert_ADDR(operand op) {
  if (op.type != ADDR) {
    return NULL;
  }
}

// Converts a label to its binary representation
binary convert_LABEL(operand op) {
  if (op.type != LABEL) {
    return NULL;
  }
}

binary convert_COND(operand op) {
  if (op.type != COND) {
    return NULL;
  }
  switch (op.word[0]) {
    case 'e':
      // eq
      return 0x0;
    case 'n':
      // ne
      return 0x1;
    case 'g':
      // ge or gt
      if (op.word[1] == 'e') {
        // ge
        return 0xa;
      } else {
        // gt
        return 0xc;
      }
    case 'l':
      // lt or le
      if (op.word[1] == 'e') {
        // le
        return 0xd;
      } else {
        // lt
        return 0xb;
      }
    case 'a':
      // al
      return 0xe;
    default:
      // error
      return NULL;
  }
}

// Converts an operand to its binary representation
binary convert_OPERAND(operand op) {

}

// Sets bits in given binary to the given value
binary set_bits(binary input, int start, int end, binary value) {
  binary mask = 0;
  for (int i = start; i <= end; i++) {
    mask = mask | (1 << i);
  }
  return (input & ~mask) | (value << start);
}

// Data Processing Instruction Assembler
binary assemble_DP(token_line line) {
  binary result = 0;

  // Immediate
  if (line.operands[2].type == IMM) {
    // Set bits 4 to 0 as value of Rd
    result = set_bits(result, 0, 4, convert_REG(line.operands[0]));
    // Set bits 28 to 26 as 100
    result = set_bits(result, 26, 28, 0x4);
    // Set bits 9 to 5 as value of Rn
    result = set_bits(result, 5, 9, convert_REG(line.operands[1]));  
    // Set bits 30 to 29 as value of opcode
    result = set_bits(result, 29, 30, convert_OPCODE(line.opcode));
    // Set bit 31 to register access mode
    if (line.operands[0].word[0] == 'x') {
      result = set_bits(result, 31, 31, 1);
    } else {
      result = set_bits(result, 31, 31, 0);
    }
    // If IMM value is greater than 2^12 - 1 then sh is 1

  }

  return result;
}

// Branch Instruction Assembler
binary assemble_B(token_line line) {
  return NULL;
}

// Single Data Transfer Instruction Assembler
binary assemble_SDT(token_line line) {
    return NULL;
}

// Special Instruction Assembler
binary assemble_SP(token_line line) {
  if (line.opcode == NOP) {
    return NOOP;
  } else if (line.opcode == DIR) {
    int result;
    // ???????????????????
    return result;
  } else if (line.opcode == HALT) {
    return HALT;
  } else {
    return NULL;
  }
}

binary assemble_line(token_line line) {
  bool has_function = false;
  func_ptr assemble_func;
  for (int i = 0; i < sizeof(instructionMapping) / sizeof(instructionMapping[0]); i++) {
    if (line.opcode == instructionMapping[i].opcode) {
      assemble_func = instructionMapping[i].function;
      has_function = true;
      break;
    }
  }
  return assemble_func(line);
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

// Writes an array of binary to the file given by fp
void write_to_binary_file(FILE *fp, binary *binary_lines, int nlines) {
  // Check if the file pointer is valid
  if (fp == NULL) {
    printf("Invalid file pointer\n");
    return;
  }

  // Check if the binary pointer is valid
  if (binary_lines == NULL) {
    printf("Invalid binary_lines pointer\n");
    return;
  }

  // Write the binary data to the file
  for (int i = 0; i < nlines; i++) {
    fwrite(&binary_lines[i], sizeof(binary), 1, fp);
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
  token_line* token_lines = read_assembly(input, nlines);

  // Convert token_lines to binary_lines
  binary *binary_lines[nlines];
  for (int i = 0; i < nlines; i++) {
    *binary_lines[i] = assemble_line(token_lines[i]);
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
