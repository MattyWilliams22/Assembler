#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "assembleutils.h"
#include "tokenizer.h"
#include "symbolTable.h"

Symbol_Table *label_table;

// get_types_... functions get the types of every operand in the line
// They are grouped by operand pattern as seen in table 2

InstructionMapping instructionMappings[] = {
    {"add", ADD, &get_types_add},
    {"adds", ADDS, &get_types_add},
    {"sub", SUB, &get_types_add},
    {"subs", SUBS, &get_types_add},
    {"cmp", CMP, &get_types_cmp},
    {"cmn", CMN, &get_types_cmp},
    {"neg", NEG, &get_types_cmp},
    {"negs", NEGS, &get_types_cmp},
    {"and", AND, &get_types_and},
    {"ands", ANDS, &get_types_and},
    {"bic", BIC, &get_types_and},
    {"bics", BICS, &get_types_and},
    {"eor", EOR, &get_types_and},
    {"orr", ORR, &get_types_and},
    {"eon", EON, &get_types_and},
    {"orn", ORN, &get_types_and},
    {"tst", TST, &get_types_tst},
    {"movk", MOVK, &get_types_movx},
    {"movn", MOVN, &get_types_movx},
    {"movz", MOVZ, &get_types_movx},
    {"mov", MOV, &get_types_mov},
    {"mvn", MVN, &get_types_mvn},
    {"madd", MADD, &get_types_madd},
    {"msub", MSUB, &get_types_madd},
    {"mul", MUL, &get_types_mul},
    {"mneg", MNEG, &get_types_mul},
    {"b", B, &get_types_b},
    {"b.", BCOND, &get_types_bcond},
    {"br", BR, &get_types_br},
    {"str", STR, &get_types_str},
    {"ldr", LDR, &get_types_ldr},
    {"nop", NOP, &get_types_null},
    {".int", DIR, &get_types_dir},
};

token_line alias(token_line line) {
  int zr_pos = -1;
  if (line.opcode == CMP) {
    line.opcode = SUBS;
    zr_pos = 0;
  } else if (line.opcode == CMN) {
    line.opcode = ADDS;
    zr_pos = 0;
  } else if (line.opcode == NEG) {
    line.opcode = SUB;
    zr_pos = 1;
  } else if (line.opcode == NEGS) {
    line.opcode = SUBS;
    zr_pos = 1;
  } else if (line.opcode == TST) {
    line.opcode = ANDS;
    zr_pos = 0;
  } else if (line.opcode == MVN) {
    line.opcode = ORN;
    zr_pos = 1;
  } else if (line.opcode == MOV) {
    line.opcode = ORR;
    zr_pos = 1;
  } else if (line.opcode == MUL) {
    line.opcode = MADD;
    zr_pos = 3;
  } else if (line.opcode == MNEG) {
    line.opcode = MSUB;
    zr_pos = 3;
  }
  if (zr_pos != -1) {
    for (int i = line.operand_count; i > zr_pos; i--) {
      line.operands[i] = line.operands[i-1];
    }
    line.operands[zr_pos].type = REG;

    // Check if 32-bit or 64-bit access mode
    if (line.operands[0].word[0] == 'w') {
      line.operands[zr_pos].word = "w31";
    } else {
      line.operands[zr_pos].word = "x31";
    }
    line.operand_count++;
  }

  return line;
}

void get_types_add(operand *operands, int op_count) {
  operands[0].type = REG;
  operands[1].type = REG;
  if (operands[2].word[0] == '#') {
    operands[2].type = IMM;
    if (op_count == 4) {
      // <Rd>, <Rn>, #<imm>, lsl #(0|12)
      operands[3].type = SHIFT;
    } else {
      // <Rd>, <Rn>, #<imm>
    }
  } else {
    operands[2].type = REG;
    if (op_count == 4) {
      // <Rd>, <Rn>, <Rm>, <shift> #<imm>
      operands[3].type = SHIFT;
    } else {
      // <Rd>, <Rn>, <Rm>
    }
  }
}

void get_types_cmp(operand *operands, int op_count) {
  operands[0].type = REG;
  if (operands[1].word[0] == '#') {
    // <Rn>, #<imm>{, <lsl #(0|12)>}
    operands[1].type = IMM;
    if (op_count == 3) {
      // <Rn>, #<imm>, <lsl #(0|12)>
      operands[2].type = SHIFT;
    } else {
      // <Rn>, #<imm>
    }
  } else {
    // <Rn>, <Rm>{, <shift> #<imm>}
    operands[1].type = REG;
    if (op_count == 3) {
      // <Rn>, <Rm>, <shift> #<imm>
      operands[2].type = SHIFT;
    } else {
      // <Rn>, <Rm>
    }
  }
}

void get_types_and(operand *operands, int op_count) {
  // <Rd>, <Rn>, <Rm>, <shift> #<imm>
  operands[0].type = REG;
  operands[1].type = REG;
  operands[2].type = REG;
  operands[3].type = SHIFT;
}

void get_types_tst(operand *operands, int op_count) {
  // <Rn>, <Rm>{, <shift> #<imm>}
  operands[0].type = REG;
  operands[1].type = REG;
  if (op_count == 3) {
    // <Rn>, <Rm>, <shift> #<imm>
    operands[2].type = SHIFT;
  } else {
    // <Rn>, <Rm>
  }
}

void get_types_movx(operand *operands, int op_count) {
  // <Rd>, #<imm>{, lsl #<imm>}
  operands[0].type = REG;
  operands[1].type = IMM;
  if (op_count == 3) {
    // <Rd>, #<imm>, lsl #<imm>
    operands[2].type = SHIFT;
  } else {
    // <Rd>, #<imm>
  }
}

void get_types_mov(operand *operands, int op_count) {
  // <Rd>, <Rn>
  operands[0].type = REG;
  operands[1].type = REG;
}

void get_types_mvn(operand *operands, int op_count) {
  // <Rd>, <Rm>{, <shift> #<imm>}
  operands[0].type = REG;
  operands[1].type = REG;
  if (op_count == 3) {
    // <Rd>, <Rm>, <shift> #<imm>
    operands[2].type = SHIFT;
  } else {
    // <Rd>, <Rm>
  }
}
  

void get_types_madd(operand *operands, int op_count) {
  // <Rd>, <Rn>, <Rm>, <Ra>
  operands[0].type = REG;
  operands[1].type = REG;
  operands[2].type = REG;
  operands[3].type = REG;
}

void get_types_mul(operand *operands, int op_count) {
  // <Rd>, <Rn>, <Rm>
  operands[0].type = REG;
  operands[1].type = REG;
  operands[2].type = REG;
}

void get_types_b(operand *operands, int op_count) {
  // <literal>      (IMM or imm)
  if (isdigit(operands[0].word[0])) {
    // <imm>
    operands[0].type = IMM;
  } else {
    // <label>
    operands[0].type = LABEL;
  }
}

void get_types_bcond(operand *operands, int op_count) {
  // <cond>, <literal>
  operands[0].type = COND;
  if (operands[1].word[0] == '#') {
    // <cond>, <imm>
    operands[1].type = IMM;
  } else {
    // <cond>, <label>
    operands[1].type = IMM;
  }
}

void get_types_br(operand *operands, int op_count) {
  // <Xn>
  operands[0].type = REG;
}

void get_types_str(operand *operands, int op_count) {
  // Need to deal with [] somehow!

  operands[0].type = REG;
  operands[1].type = REG;
  
  int length = strlen(operands[2].word);
  if (operands[2].word[length - 1] == '!') {
    // <Rt>, [<Xn>, #<simm>]!                    (Pre-index)
    operands[2].type = IMM;
                    
  } else if (operands[2].word[length - 1] != ']') {
    // <Rt>, [<Xn>], #<simm>                     (Post-index)
    operands[2].type = IMM;
  } else if (operands[2].word[0] == '#') {
    // <Rt>, [<Xn|SP>, #<imm>]                   (Unsigned Offset)
    operands[2].type = IMM;
  } else {
    // <Rt>, [<Xn>, <Rm>{, lsl #<amount>}]
    operands[2].type = REG;
    if (op_count == 4) {
    // <Rt>, [<Xn>, <Rm>, lsl #<amount>]
    operands[3].type = SHIFT;
    } else {
    // <Rt>, [<Xn>, <Rm>]
    }
  }
}

void get_types_ldr(operand *operands, int op_count) {
  // <Rt>, <literal>
  operands[0].type = REG;
  if (operands[0].word[0] == '#') {
    // <Rt>, <imm>
    operands[1].type = IMM;
  } else {
    // <Rt>, <label>
    operands[1].type = IMM;
  }
}

void get_types_dir(operand *operands, int op_count) {
  // <simm>
  operands[0].type = IMM;
}

void get_types_null(operand *operands, int op_count) {
  // Do nothing
}

// Checks if the instruction is the halt instruction (and x0, x0, x0)
bool is_halt(opcode_name opcode, operand *operands, int op_count) {
  if (opcode == AND && op_count == 3) {
    if (strcmp(operands[0].word, "x0") == 0 
    && strcmp(operands[1].word, "x0") == 0
    && strcmp(operands[2].word, "x0") == 0) {
      return true;
    }
  }
  return false;
}

// Converts a line of text into a token_line
token_line *process_line(char *line, int line_no, token_line *lines) {
  int string_count = 0;
  char *strings[10];

  while (*line != '\0') {  
    while (isspace(*line) || *line == ',') {
      line++;
    }

    char *start = line;

    while (*line != '\0' && !isspace(*line) && *line != ',') {
      line++;
    }

    char ch = *line; // char beyond end of characters
    *line = '\0'; // temporarily terminate string
    strings[string_count] = strdup(start);
    string_count++;
    *line = ch; // restore the original unused char
  }

  // Removes a trailing empty string
  if (strings[string_count - 1][0] == '\0') {
    string_count--;
  }

  // b.cond case
  if (string_count > 0) {
    if (strlen(strings[0]) > 1) {
      if (strings[0][1] == '.') {
        strings[2] = strings[1];
        strings[1] = strdup(strings[0] + 2);
        strings[0] = "b.";
        string_count++;
      }
    }
  }

  int operand_count = string_count - 1;
  func_ptr_type get_types;
  bool has_function = false;
  opcode_name opcode = UNRECOGNISED_OPCODE;

  for (int i = 0; i < sizeof(instructionMappings) / sizeof(instructionMappings[0]); i++) {
    if (strcmp(strings[0], instructionMappings[i].instruction) == 0) {
      opcode = instructionMappings[i].opcode;
      get_types = instructionMappings[i].function;
      has_function = true;
      break;
    }
  }
  
  if (opcode == LDR && operand_count != 2) {
    get_types = &get_types_str;
  }

  operand current_operands[operand_count];

  for (int i = 0; i < operand_count; i++) {
    current_operands[i].word = strings[i + 1];
  }

  if (opcode == UNRECOGNISED_OPCODE) {
    int length = strlen(strings[0]) - 1;
    if (strings[0][length] == ':') {
      opcode = LABEL_OPCODE;
      get_types = &get_types_null;
      add_address(label_table, strings[0], line_no, lines);
    }
  }

  if (opcode == AND) {
    if (is_halt(opcode, current_operands, operand_count)) {
      opcode = HALT;
      get_types = &get_types_null;
      operand_count = 0;
    }
  }

  if (has_function) {
    get_types(current_operands, operand_count);
  }

  if (strcmp(current_operands[operand_count - 2].word, "lsl") == 0 ||\
  strcmp(current_operands[operand_count - 2].word, "lsr") == 0 ||\
  strcmp(current_operands[operand_count - 2].word, "asr") == 0 ||\
  strcmp(current_operands[operand_count - 2].word, "ror") == 0) {
    current_operands[operand_count - 2].type = SHIFT;
    current_operands[operand_count - 1].type = IMM;
  }

  operand *operands = malloc(operand_count * sizeof(operand));
  for (int i = 0; i < operand_count; i++) {
    operands[i] = *make_operand(current_operands[i].type, current_operands[i].word);
  }
  token_line *current_line = make_token_line(opcode, operand_count, operands);

  return current_line; 
}

// Reads an assembly code file and processes the lines into a token_array
token_line *read_assembly(FILE* fp, int nlines, int *line_count) {
  char line[100];
  token_line *lines = malloc(nlines * sizeof(token_line));
  label_table = malloc(sizeof(Symbol_Table));
  
  while ((fgets(line, sizeof(line), fp)) != NULL) {
    // Remove trailing newline if there is one
    if (line[strlen(line) - 1] == '\n') {
      line[strlen(line) - 1] = '\0';
    }

    // Removes starting and trailing spaces from string
    int start = 0;
    while (isspace(line[start])) {
      start++;
    }
    int end_pos = strlen(line);
    while (end_pos >= 0) {
      if (isspace(line[end_pos])) {
        end_pos--;
      } else {
        break;
      }
    }
    line[end_pos] = '\0';

    if (strlen(&line[start]) != 0) {
      token_line *current_line = process_line(&line[start], *line_count, lines);
      if (current_line->opcode != UNRECOGNISED_OPCODE && current_line->opcode != LABEL_OPCODE) {
        lines[*line_count] = *current_line;
        lines[*line_count] = alias(lines[*line_count]);
        if (current_line->opcode == B && current_line->operands[0].word[0] != '#') {
          add_dependency(label_table, current_line->operands[0].word, current_line->operands[0], *line_count, lines);
        } else if (current_line->opcode == BCOND && current_line->operands[1].word[1] != '#') {
          add_dependency(label_table, current_line->operands[1].word, current_line->operands[1], *line_count, lines);
        } else if (current_line->opcode == LDR && current_line->operands[1].word[1] != '#') {
          add_dependency(label_table, current_line->operands[1].word, current_line->operands[1], *line_count, lines);
        }
        (*line_count)++;
      }
    }
  }

  free_table(label_table);
  fclose(fp);
  return lines;
}