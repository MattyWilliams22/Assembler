#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tokenizer.h"

// get_types_... functions get the types of every operand in the line
// They are grouped by operand pattern as seen in table 2


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
  operands[3].type = IMM;
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
  // <literal>      (label or immediate address)
  if (operands[0].word[0] == '0' && operands[0].word[1] == 'x') {
    // <address>
    operands[0].type = ADDR;
  } else {
    // <label>
    operands[0].type = LABEL;
  }
}

void get_types_bcond(operand *operands, int op_count) {
  // <cond>, <literal>
  operands[0].type = COND;
  if (operands[1].word[0] == '0' && operands[1].word[1] == 'x') {
    // <cond>, <address>
    operands[1].type = ADDR;
  } else {
    // <cond>, <label>
    operands[1].type = LABEL;
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
  if (operands[2].word[length] == '!') {
    // <Rt>, [<Xn>, #<simm>]!                    (Pre-index)
    operands[2].type = IMM;
                    
  } else if (operands[2].word[length] != ']') {
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
  if (operands[0].word[0] == '0' && operands[0].word[1] == 'x') {
    // <Rt>, <address>
    operands[1].type = ADDR;
  } else {
    // <Rt>, <label>
    operands[1].type = LABEL;
  }
}

void get_types_dir(operand *operands, int op_count) {
  // <simm>
  operands[0].type = IMM;
}

void get_types_null(operand *operands, int op_count);

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

// Converts a string into a token_line
token_line process_line(char * line) {
  // Process a line from the .s file
  const char s[] = " "; 
  const char d[] = ".";
  char * instr_str = strtok(line, s);
  instr_str = strtok(instr_str, d);
  int string_count = 0;
  char *strings[10];

  for (char *q = strtok(NULL, d); q != NULL; q = strtok(NULL, d)) {
    strcpy(strings[string_count], q);
    string_count++;
  }
  
  for (char *p = strtok(line, s); p != NULL; p = strtok(NULL, s)) {
    strcpy(strings[string_count], p);
    string_count++;
  }

  char *sentence = "";
  for (int i = 0; i < string_count; i++) {
    char *current_word = strings[i];  
    int length = strlen(current_word);
    strcat(sentence, current_word);
    if (current_word[length] != ',') {
      strcat(sentence, " ");
    }
  }

  int word_count = 0;
  char *words;
  const char c[] = ",";


  // Need to assemble words differently for STR and LDR due to []'s

  for (char *p = strtok(sentence, c); p != NULL; p = strtok(NULL, c)) {
    strcpy(&words[word_count], p);
    word_count++;
  }

  func_ptr_type get_types;
  bool has_function = false;
  opcode_name opcode = UNRECOGNISED_OPCODE;

  for (int i = 0; i < sizeof(instructionMappings) / sizeof(instructionMappings[0]); i++) {
    if (strcmp(instr_str, instructionMappings[i].instruction) == 0) {
      opcode = instructionMappings[i].opcode;
      get_types = instructionMappings[i].function;
      has_function = true;
      break;
    }
  }

  if(opcode == AND) {
    if(is_halt(opcode, words, word_count)) {
      opcode = HALT;
      get_types = &get_types_null;
    }
  }

  if (opcode == LDR && word_count != 2) {
    get_types = &get_types_str;
  }

  operand *current_operands;
  token_line current_line;

  for (int i = 0; i < word_count; i++) {
    current_operands[i].word = &words[i];
  }

  if (has_function) {
    get_types(current_operands, word_count);
  }

  current_line.opcode = opcode;
  current_line.operands = current_operands;
  current_line.operand_count = word_count;

  return current_line;
}

// Reads an assembly code file and processes each line into a token_line
token_line* read_assembly(FILE* fp, int nlines) {
  char* line = NULL;
  size_t len = 0;
  int read;
  int count = 0;
  token_line* token_lines = malloc(nlines * sizeof(token_line));

  if (token_lines == NULL) {
    // Handle memory allocation failure
    perror("Memory allocation failed");
    fclose(fp);
    return NULL;
  }

  while ((read = getline(&line, &len, fp)) != -1) {
    token_lines[count] = process_line(line);
    count++;
  }

  fclose(fp);
  free(line);
  return token_lines;
}