#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "assemble.h"
#include "assembleutils.h"
#include "symbolTable.h"
#include "tokenizer.h"

#define HALT 0x8a000000
#define NOOP 0xd503201f

AssembleMapping assembleMappings[] = {
    {ADD, &assemble_DP, 0x0},
    {ADDS, &assemble_DP, 0x1},
    {SUB, &assemble_DP, 0x2},
    {SUBS, &assemble_DP, 0x3},
    {AND, &assemble_DP, 0x0},
    {ANDS, &assemble_DP, 0x3},
    {BIC, &assemble_DP, 0x0},
    {BICS, &assemble_DP, 0x3},
    {EOR, &assemble_DP, 0x2},
    {ORR, &assemble_DP, 0x1},
    {EON, &assemble_DP, 0x2},
    {ORN, &assemble_DP, 0x1},
    {MOVK, &assemble_DP, 0x3},
    {MOVN, &assemble_DP, 0x0},
    {MOVZ, &assemble_DP, 0x2},
};

// Sets bits in given binary to the given value
binary set_bits(binary input, int start, int end, binary value) {
  binary mask = 0;
  for (int i = start; i <= end; i++) {
    mask = mask | (1 << i);
  }
  return (input & ~mask) | (value << start);
}

// Applies the shift to the binary input
binary convert_SHIFT(operand shift) {
  if (shift.type != SHIFT) {
    return NULL;
  }
  if (shift.word[0] == 'l') {
    // lsl or lsr
    if (shift.word[2] == 'l') {
      // lsl
      return 0x0;
    } else {
      // lsr
      return 0x1;
    }
  } else if (shift.word[0] == 'a') {
    // asr
    return 0x2;
  } else {
    // ror
    return 0x3;
  }
}


// Converts an opcode to its binary representation
binary convert_OPCODE(opcode_name name) {
  for (int i = 0; i < sizeof(assembleMappings) / sizeof(assembleMappings[0]); i++) {
    if (name == assembleMappings[i].opcode) {
      return assembleMappings[i].opc;
    }
  }
};


// Converts a register to its binary representation
binary convert_REG(operand op) {
  if (op.type != REG) {
    return NULL;
  }
  return atoi(&op.word[1]);
}

// Converts an immediate value to its binary representation
binary convert_IMM(operand op) {
  // Need to handle if IMM is signed or not

  if (op.type != IMM) {
    return NULL;
  }

  if (op.word[0] == '#') {
    op.word = &op.word[1];
  }

  if (op.word[0] == '0' && op.word[1] == 'x') {
    // hex
    return (int)strtol(&op.word[2], NULL, 16);
  } else {
    // decimal
    return atoi(op.word);
  }
}

// Converts a label to its binary representation
binary convert_LABEL(operand op) {
  if (op.type != LABEL_OPERAND) {
    return NULL;
  }
  // Get from symbol table?
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

binary convert_ADDR_MODE(operand op, addressing_mode mode) {
  binary result = 0;
  if (mode == REG_OFF) {
    result = set_bits(result, 11, 11, 1);
    result = set_bits(result, 6, 10, convert_REG(op));
    result = set_bits(result, 0, 5, 0x1a);
  } else if (mode == UNSIGNED_OFF) {
    result = convert_IMM(op);
  } else {
    result = set_bits(result, 11, 11, 0);
    result = set_bits(result, 2, 10, convert_IMM(op));
    if (mode == PRE_IND) {
      result = set_bits(result, 0, 1, 0x3);
    } else {
      result = set_bits(result, 0, 1, 1);
    }
  }
  return result;
}

// Data Processing Instruction Assembler
binary assemble_DP(token_line line) {
  binary result = 0;

  // Set bit 31 to register access mode
  if (line.operands[0].word[0] == 'x') {
    result = set_bits(result, 31, 31, 1);
  } else {
    result = set_bits(result, 31, 31, 0);
  }

  // Immediate
  if (line.operands[2].type == IMM) {
    // Must add case for wide moves

    
    // Set bits 4 to 0 as value of Rd
    result = set_bits(result, 0, 4, convert_REG(line.operands[0]));
    // Set bits 28 to 26 as 100
    result = set_bits(result, 26, 28, 0x4);
    // Set bits 9 to 5 as value of Rn
    result = set_bits(result, 5, 9, convert_REG(line.operands[1]));  
    // Set bits 30 to 29 as value of opcode
    result = set_bits(result, 29, 30, convert_OPCODE(line.opcode));
    // Setting IMM and sh


    // Set opi
    if (line.opcode == ADD || line.opcode == SUB || line.opcode == ADDS || line.opcode == SUBS) {
      result = set_bits(result, 23, 25, 0x2);
    }
    if (line.opcode == MOVN || line.opcode == MOVZ || line.opcode == MOVK) {
      result = set_bits(result, 23, 25, 0x5);
    }
  } else {
    // Data Processing Register

    // Set bits 4 to 0 as value of Rd
    result = set_bits(result, 0, 4, convert_REG(line.operands[0]));
    // Set bits 9 to 5 as value of Rn
    result = set_bits(result, 5, 9, convert_REG(line.operands[1]));
    // Set tbis 20 to 16 as value of Rm
    result = set_bits(result, 16, 20, convert_REG(line.operands[2]));
    // Set bits 30 to 29 as value of opcode
    result = set_bits(result, 29, 30, convert_OPCODE(line.opcode));
    // Set bits 27 to 25 as 101
    result = set_bits(result, 25, 27, 0x5);
    // Set M and opr
    if (line.opcode == ADD || line.opcode == SUB || line.opcode == ADDS || line.opcode == SUBS) {
      // Set bit 28 as 0
      result = set_bits(result, 28, 28, 0);
      // Set bit 24 as 1
      result = set_bits(result, 24, 24, 1);
      // Set bit 21 as 0
      result = set_bits(result, 21, 21, 0);
      // Set bits 23 to 22 as shift amount

      
    }


  }

  return result;
}

// Branch Instruction Assembler
binary assemble_B(token_line line) {
  binary result = 0;
  if (line.opcode == B) {
    // Unconditional
    result = set_bits(result, 26, 31, 0x5);
    if (line.operands[0].type == IMM) {
      result = set_bits(result, 0, 25, convert_IMM(line.operands[0]));
    } else {
      result = set_bits(result, 0, 25, convert_LABEL(line.operands[0]));
    }
  } else if (line.opcode == BR) {
    // Register
    result = set_bits(result, 10, 31, 0x3587c0);
    result = set_bits(result, 5, 9, convert_REG(line.operands[0]));
    result = set_bits(result, 0, 4, 0);
  } else if (line.opcode == BCOND) {
    // Conditional
    result = set_bits(result, 24, 31, 0x54);
    result = set_bits(result, 0, 4, convert_COND(line.operands[0]));
    if (line.operands[0].type == IMM) {
      result = set_bits(result, 5, 23, convert_IMM(line.operands[1]));
    } else {
      result = set_bits(result, 5, 23, convert_LABEL(line.operands[1]));
    }
  } else {
    return NULL;
  }
}



addressing_mode get_addressing_mode(token_line line) {
  line.operands[1].word = &line.operands[1].word[1];
  int length1 = strlen(line.operands[2].word);
  if (line.operands[2].word[length1 - 1] == '!') {
    // <Rt>, [<Xn>, #<simm>]!                    (Pre-index)
    line.operands[2].word[length1 - 3] = '\0';
    return PRE_IND;
  } else if (line.operands[2].word[length1] != ']') {
    // <Rt>, [<Xn>], #<simm>                     (Post-index)
    line.operands[2].word[length1 - 2] = '\0';
    return POST_IND;
  } else if (line.operands[2].word[0] == '#') {
    // <Rt>, [<Xn|SP>, #<imm>]                   (Unsigned Offset)
    line.operands[2].word[length1 - 2] = '\0';
    return UNSIGNED_OFF;
  } else {
    // <Rt>, [<Xn>, <Rm>{, lsl #<amount>}]
    if (line.operand_count == 4) {
    // <Rt>, [<Xn>, <Rm>, lsl #<amount>]
    int length2 = strlen(line.operands[3].word);
    line.operands[3].word[length2 - 2] = '\0';
    } else {
    // <Rt>, [<Xn>, <Rm>]
    line.operands[2].word[length1 - 2] = '\0';
    }
    return REG_OFF;
  }
}

// Single Data Transfer Instruction Assembler
binary assemble_SDT(token_line line) {
  binary result = 0;

  // Set bit 31 to register access mode
  if (line.operands[0].word[0] == 'x') {
    result = set_bits(result, 30, 30, 1);
  } else {
    result = set_bits(result, 30, 30, 0);
  }

  if (line.opcode == LDR && line.operand_count == 2) {
    // Load literal
    result = set_bits(result, 31, 31, 0);
    result = set_bits(result, 24, 29, 0x18);
    result = set_bits(result, 5, 23, convert_IMM(line.operands[1]));
    result = set_bits(result, 0, 4, convert_REG(line.operands[0]));
  } else {
    // Single Data Transfer

    // 0 L
    if (line.opcode == LDR) {
      result = set_bits(result, 22, 23, 1);
    } else {
      result = set_bits(result, 22, 23, 0);
    }

    // U
    addressing_mode mode = get_addressing_mode(line);
    int len = strlen(line.operands[2].word);
    if (mode == UNSIGNED_OFF) {
      result = set_bits(result, 24, 24, 1);
    } else {
      result = set_bits(result, 24, 24, 0);
    }

    result = set_bits(result, 31, 31, 1);
    result = set_bits(result, 25, 29, 0x1c);
    result = set_bits(result, 0, 4, convert_REG(line.operands[0]));
    result = set_bits(result, 5, 9, convert_REG(line.operands[1]));
    result = set_bits(result, 10, 21, convert_ADDR_MODE(line.operands[2], mode));
  }
  return result;
}

// Special Instruction Assembler
binary assemble_SP(token_line line) {
  if (line.opcode == NOP) {
    return NOOP;
  } else if (line.opcode == DIR) {
    return convert_IMM(line.operands[0]);
  } else if (line.opcode == HALT) {
    return HALT;
  } else {
    return NULL;
  }
}

binary assemble_line(token_line line) {
  bool has_function = false;
  func_ptr assemble_func;
  for (int i = 0; i < sizeof(assembleMappings) / sizeof(assembleMappings[0]); i++) {
    if (line.opcode == assembleMappings[i].opcode) {
      assemble_func = assembleMappings[i].function;
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
  binary binary_lines[nlines];
  for (int i = 0; i < nlines; i++) {
    binary_lines[i] = assemble_line(token_lines[i]);
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
