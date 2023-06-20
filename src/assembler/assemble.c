#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "assemble.h"
#include "assembleutils.h"
#include "tokenizer.h"

// A mapping of Assembly instruction opcodes to the function that translates it into binary
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
    {MADD, &assemble_DP, 0x0},
    {MSUB, &assemble_DP, 0x0},
    {B, &assemble_B, 0x0},
    {BCOND, &assemble_B, 0x0},
    {BR, &assemble_B, 0x0},
    {STR, &assemble_SDT, 0x0},
    {LDR, &assemble_SDT, 0x0},
    {LDRLIT, &assemble_SDT, 0x0},
    {NOP, &assemble_SP, 0x0},
    {HALT, &assemble_SP, 0x0},
    {DIR, &assemble_SP, 0x0}
};

/**
 * Set bits within the range provided within the input to to the given value.
 *
 * @param input The number that the bits are to be inserted into.
 * @param start The beginning where to insert the bits within the number.
 * @param end The end of where to insert the bits within the number.
 * @param value The value to be inserted into the given input.
 * @return The input with the value inserted into the appropriate bit positions.
 */
binary set_bits(binary input, int start, int end, binary value) {
  binary mask = 0;
  for (int i = start; i <= end; i++) {
    mask = mask | (binary) 1 << i;
  }
  return (input & ~mask) | ((value << start) & mask);
}

/**
 * Removes the first character from a given string, creating and returning a new string.
 *
 * @param str The string from which the first character is to be removed
 * @return The new string, which is exactly as the input string but the first characters has been removed
 */
char *remove_first_char(char *str) {
  char *result = strdup(&str[1]);
  return result;
}

/**
 * Removes the last character from a given string
 *
 * @param str The string from which the last character is to be removed
 */
void remove_last_char(char *str) {
  int end = strlen(str) - 1;
  str[end] = '\0';
}

/**
 * Converts a given opcode to its binary representation
 *
 * @param name The opcode to be converted into binary
 * @return The binary representation of the given opcode
 */
binary convert_OPCODE(opcode_name name) {
  for (int i = 0; i < sizeof(assembleMappings) / sizeof(assembleMappings[0]); i++) {
    if (name == assembleMappings[i].opcode) {
      return assembleMappings[i].opc;
    }
  }
  return 0xff;
}

/**
 * Converts a given immediate value to its binary representation
 *
 * @param op The operand containing the immediate value to convert into binary
 * @return The binary representation of the given immediate value
 */
binary convert_IMM(operand op) {
  if (op.word[0] == '#') {
    op.word = &op.word[1];
  }
  if (op.word[0] == '0' && op.word[1] == 'x') {
    // Immediate value is in hexadecimal
    return (binary) strtol(&op.word[2], NULL, 16);
  } else {
    // Immediate value is in decimal
    return atoi(op.word);
  }
}

/**
 * Converts a given register to its binary representation
 *
 * @param op The operand containing the register to convert into binary
 * @return The binary representation of the given register
 */
binary convert_REG(operand op) {
  // Converts zero register to register 32
  if (op.word[1] == 'z') {
    op.word[1] = '3';
    op.word[2] = '1';
  }

  // Ignore the first character which is an 'x' or 'w'
  return atoi(&op.word[1]);
}

/**
 * Converts a given shift instruction into its binary representation
 *
 * @param op The operand containing the shift instruction to convert into binary
 * @return The binary representation of the given shift
 */
binary convert_SHIFT(operand op) {
  binary result;
  if (op.word[0] == 'l') {
    // lsl or lsr
    if (op.word[2] == 'l') {
      // lsl
      result = 0x0;
    } else {
      // lsr
      result = 0x1;
    }
  } else if (op.word[0] == 'a') {
    // asr
    result = 0x2;
  } else {
    // ror
    result = 0x3;
  }
  return result;
}

/**
 * Converts a given shift instruction into its binary representation
 *
 * @param op The operand containing the shift instruction to convert into binary
 * @return The binary representation of the given shift
 */
binary get_shift_amount(operand op) {
  int length = strlen(op.word);
  for (int i = 0; i < length; i++) {
    if (op.word[i] == '#') {
      op.word = &op.word[i];
    }
  }
  return convert_IMM(op);
}

/**
 * Converts a given branch condition type into its binary representation
 *
 * @param op The operand containing the branch condition type to convert into binary
 * @return The binary representation of the given branch condition
 */
binary convert_COND(operand op) {
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
      return 0xf;
  }
}

/**
 * Converts a given into its binary representation based on its addressing mode
 *
 * @param op The operand to convert into binary
 * @param mode The type of addressing mode of the operand
 * @return The binary representation of the given operand based on its addressing mode
 */
binary convert_ADDR_MODE(operand op, addressing_mode mode) {
  binary result = 0;
  if (mode == REG_OFF) {
    result = set_bits(result, 11, 11, 1);
    result = set_bits(result, 6, 10, convert_REG(op));
    result = set_bits(result, 0, 5, 0x1a);
  } else if (mode == UNSIGNED_OFF) {
    result = convert_IMM(op) / 8;
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

/**
 * Given a token line of an assembly instruction, returns the type of addressing mode
 * use within the operand
 *
 * @param line The assembly token line from which the addressing mode is to be figured out
 * @return The type of addressing mode used within the token line
 */
addressing_mode get_addressing_mode(token_line line) {
  if (line.operand_count == 2) {
    // rt, [xn]                Unsigned offset (2 operands)
    line.operands[1].word = remove_first_char(line.operands[1].word);
    remove_last_char(line.operands[1].word);
    line.operand_count++;
    line.operands[2].type = IMM;
    line.operands[2].word = "#0";
    // rt, xn, #0
    return UNSIGNED_OFF;
  } else {
    line.operands[1].word = remove_first_char(line.operands[1].word);
    int length1 = strlen(line.operands[1].word);
    int length2 = strlen(line.operands[2].word);
    if (line.operands[2].word[length2 - 1] == '!') {
      // rt, xn, #simm]!                    
      remove_last_char(line.operands[2].word);
      remove_last_char(line.operands[2].word);
      // rt, xn, #simm 
      return PRE_IND;
    } else if (line.operands[1].word[length1 - 1] == ']') {
      // rt, xn], #simm                     
      remove_last_char(line.operands[1].word);
      // rt, xn, #simm
      return POST_IND;
    } else if (line.operands[2].word[0] == '#') {
      // rt, [xn, #imm]                                     
      remove_last_char(line.operands[2].word);
      // rt, xn, #imm
      return UNSIGNED_OFF;
    } else {
      if (line.operand_count > 3) {
        // rt, xn, xm, lsl #imm]                            
        remove_last_char(line.operands[3].word);
        // rt, xn, xm, lsl #imm
      } else {
        // rt, xn, xm]                                      
        remove_last_char(line.operands[2].word);
        // rt, xn, xm
      }
      return REG_OFF;
    }
  }
}

/**
 * Given a token line of a Data Processing Assembly instruction, it converts the given instruction
 * into its binary representation.
 *
 * @param line The token line of the assembly instruction to be converted into binary 
 * @return The binary representation of the Data Processing assembly instruction
 */
binary assemble_DP(token_line line) {
  binary result = 0;
  // Set bit 31 to register access mode
  if (line.operands[0].word[0] == 'x') {
    result = set_bits(result, 31, 31, 1);
  } else {
    result = set_bits(result, 31, 31, 0);
  }

  // Set bits 30 to 29 as value of opcode
  result = set_bits(result, 29, 30, convert_OPCODE(line.opcode));
  // Set bits 4 to 0 as value of Rd
  result = set_bits(result, 0, 4, convert_REG(line.operands[0]));

  // Immediate
  if (line.operands[1].type == IMM || (line.operand_count > 2 && line.operands[2].type == IMM)) {
    // Set bits 28 to 26 as 100
    result = set_bits(result, 26, 28, 0x4);

    if (line.operands[1].type == IMM) {
      // Set bits 20 to 5 as imm16
      result = set_bits(result, 5, 20, convert_IMM(line.operands[1]));
      // Set bits 22 to 21 as hw (shift left by hw * 16 bits), checking if there is a shift to be applied to the operand
      if (line.operands[line.operand_count - 2].type == SHIFT) {
        result = set_bits(result, 21, 22, get_shift_amount(line.operands[line.operand_count - 1]) / 16);
      }
      // Set bits 25 to 23 as opi
      result = set_bits(result, 23, 25, 0x5);
    } else {
      // Set bits 9 to 5 as value of Rn
      result = set_bits(result, 5, 9, convert_REG(line.operands[1]));  
      // Set bits 21 to 10 as imm12
      result = set_bits(result, 10, 21, convert_IMM(line.operands[2]));
      // Set bit 22 as sh, checking if there is a shift that is to be applied to the operand
      if (line.operands[line.operand_count - 2].type == SHIFT && get_shift_amount(line.operands[line.operand_count - 1]) == 0xc) {
        result = set_bits(result, 22, 22, 1);
      }
      // Set bits 25 to 23 as opi
      result = set_bits(result, 23, 25, 0x2);
    }
  } else {
    // Data Processing Register

    // Set bits 9 to 5 as value of Rn
    result = set_bits(result, 5, 9, convert_REG(line.operands[1]));
    // Set tbis 20 to 16 as value of Rm
    result = set_bits(result, 16, 20, convert_REG(line.operands[2]));
    // Set bits 27 to 25 as 101
    result = set_bits(result, 25, 27, 0x5);

    if (line.opcode == ADD || line.opcode == SUB || line.opcode == ADDS || line.opcode == SUBS) {
      // Arithmetic

      // Set bit 28 as 0
      result = set_bits(result, 28, 28, 0);
      // Set bit 24 as 1
      result = set_bits(result, 24, 24, 1);

      // Check if there is a shift to be applied to the operand
      if (line.operands[line.operand_count - 2].type == SHIFT) {
        // Set bits 23 to 22 as shift type
        result = set_bits(result, 22, 23, convert_SHIFT(line.operands[line.operand_count - 2])); 
        // Set bits 15 to 10 as shift amount
        result = set_bits(result, 10, 15, get_shift_amount(line.operands[line.operand_count - 1]));      
      }
      
      // Set bit 21 as 0
      result = set_bits(result, 21, 21, 0);
      
    } else if (line.opcode == MADD || line.opcode == MSUB) {
      // Multiply

      // Set bit 28 as 1
      result = set_bits(result, 28, 28, 1);
      // Set bits 24 to 21 as 1000
      result = set_bits(result, 21, 24, 0x8);
      // Set bit 15 to M
      if (line.opcode == MSUB) {
        result = set_bits(result, 15, 15, 1);
      }
      // Set bits 14 to 10 as value of ra
      result = set_bits(result, 10, 14, convert_REG(line.operands[3]));
    } else {
      // Logical

      // Set bit 28 as 0
      result = set_bits(result, 28, 28, 0);
      // Set bit 24 as 1
      result = set_bits(result, 24, 24, 0);

      // Check if there is a shift to be applied to the operand
      if (line.operands[line.operand_count - 2].type == SHIFT) {
        // Set bits 23 to 22 as shift type
        result = set_bits(result, 22, 23, convert_SHIFT(line.operands[line.operand_count - 2])); 
        // Set bits 15 to 10 as shift amount
        result = set_bits(result, 10, 15, get_shift_amount(line.operands[line.operand_count - 1]));      
      }

      // Set bit 21 as N
      if (line.opcode == BIC || line.opcode == ORN || line.opcode == EON || line.opcode == BICS) {
        result = set_bits(result, 21, 21, 1);
      }
    }
  }
  return result;
}

/**
 * Given a token line of a Branch Assembly instruction, it converts the given instruction
 * into its binary representation.
 *
 * @param line The token line of the assembly instruction to be converted into binary 
 * @return The binary representation of the Branch assembly instruction
 */
binary assemble_B(token_line line) {
  binary result = 0;
  if (line.opcode == B) {
    // Unconditional
    // Set bits 31 to 26 to 000101
    result = set_bits(result, 26, 31, 0x5);
    // Set bits 25 to 0 to the value of the literal
    if (line.operands[0].type == LABEL) {
      result = set_bits(result, 0, 25, convert_IMM(line.operands[0]));
    } else { 
      result = set_bits(result, 0, 25, convert_IMM(line.operands[0]) / 4);
    }
  } else if (line.opcode == BR) {
    // Register
    // Set bits 31 to 10 to 1101011000011111000000
    result = set_bits(result, 10, 31, 0x3587c0);
    // Set bits 9 to 5 to value of xn
    result = set_bits(result, 5, 9, convert_REG(line.operands[0]));
    // Set bits 4 to 0 to 00000
    result = set_bits(result, 0, 4, 0);
  } else if (line.opcode == BCOND) {
    // Conditional
    // Set bits 31 to 24 to 01010100
    result = set_bits(result, 24, 31, 0x54);
    // Set bits 4 to 0 to cond
    result = set_bits(result, 0, 4, convert_COND(line.operands[0]));
    // Set bits 23 to 5 to the value of the literal
    int value = convert_IMM(line.operands[1]);
    if (value < 0) {
      result = set_bits(result, 5, 23, value & 0x7ffff);
    } else {
      result = set_bits(result, 5, 23, value);
    }
  }
  return result;
}

/**
 * Given a token line of a Single Data Transfer Assembly instruction, it converts the given instruction
 * into its binary representation.
 *
 * @param line The token line of the assembly instruction to be converted into binary 
 * @return The binary representation of the Single Data Transfer assembly instruction
 */
binary assemble_SDT(token_line line) {
  binary result = 0;
  // Set bit 31 to register access mode
  if (line.operands[0].word[0] == 'x') {
    result = set_bits(result, 30, 30, 1);
  } else {
    result = set_bits(result, 30, 30, 0);
  }

  // Set bits 4 to 0 to rt
  result = set_bits(result, 0, 4, convert_REG(line.operands[0]));

  if (line.opcode == LDR && line.operand_count == 2 && line.operands[1].word[0] != '[') {
    // Load literal
    // Set bit 31 to 0
    result = set_bits(result, 31, 31, 0);
    // Set bits 29 to 24 to 011000
    result = set_bits(result, 24, 29, 0x18);
    // Set bits 23 to 5 to simm19
    result = set_bits(result, 5, 23, convert_IMM(line.operands[1]));
  } else {
    // Single Data Transfer
    // Set bits 23 to 22 to 0L
    if (line.opcode == LDR) {
      result = set_bits(result, 22, 23, 1);
    } else {
      result = set_bits(result, 22, 23, 0);
    }
    // Set bit 24 to U
    addressing_mode mode = get_addressing_mode(line);
    if (mode == UNSIGNED_OFF) {
      result = set_bits(result, 24, 24, 1);
    } else {
      result = set_bits(result, 24, 24, 0);
    }
    // Set bit 31 to 1
    result = set_bits(result, 31, 31, 1);
    // Set bits 29 to 25 to 11100
    result = set_bits(result, 25, 29, 0x1c);
    // Set bits 9 to 5 to xn
    result = set_bits(result, 5, 9, convert_REG(line.operands[1]));
    // Set bits 21 to 10 to offset
    result = set_bits(result, 10, 21, convert_ADDR_MODE(line.operands[2], mode));
  }
  return result;
}

/**
 * Given a token line of a NOP, DIR or HALT instruction, it converts the given instruction
 * into its binary representation.
 *
 * @param line The token line of the assembly instruction to be converted into binary 
 * @return The binary representation of the special assembly instruction
 */
binary assemble_SP(token_line line) {
  if (line.opcode == NOP) {
    return NOOP;
  } else if (line.opcode == DIR) {
    return convert_IMM(line.operands[0]);
  } else {
    // line.opcode == HALT
    return HALTINSTR;
  }
}

/**
 * Given a token line of an Assembly instruction, it converts the given instruction
 * into its binary representation by calling the function corresponding to the type of 
 * instruction that it is
 *
 * @param line The token line of the assembly instruction to be converted into binary 
 * @return The binary representation of assembly instruction
 */
binary assemble_line(token_line line) {
  func_ptr assemble_func;
  for (int i = 0; i < sizeof(assembleMappings) / sizeof(assembleMappings[0]); i++) {
    if (line.opcode == assembleMappings[i].opcode) {
      assemble_func = assembleMappings[i].function;
      break;
    }
  }
  binary result = assemble_func(line);
  return result;
}

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

int main(int argc, char **argv) {
  // Open the input file
  FILE* fp = fopen(argv[1], "rt");
	if (fp == NULL) {
		perror("Could not open input file.");
		exit(EXIT_FAILURE);
	}

  // Get number of lines in input file
  int nlines = count_lines(fp);

  // Variable to store the number of lines which contain assembly instructions
  int ntokenlines = 0;

  // Convert lines of file to an array of token_lines
  token_line *token_lines = read_assembly(fp, nlines, &ntokenlines);

  // Convert token_lines to binary_lines
  binary binary_lines[ntokenlines];
  for (int i = 0; i < ntokenlines; i++) {
      binary_lines[i] = assemble_line(token_lines[i]);
  }

  // Writes binary_lines to output file
  write_to_binary_file(argv[2], binary_lines, ntokenlines, token_lines);

  return EXIT_SUCCESS;
}
