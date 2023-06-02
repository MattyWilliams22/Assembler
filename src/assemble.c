#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

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
  UNRECOGNISED
} opcode_name;

typedef enum {
  REG,
  // May need to differentiate between IMM and SIMM
  IMM,
  SHIFT,
  ADDR,
  LABEL,
  UNRECOGNISED
} operand_type;

typedef struct {
  operand_type type;
  char *word;
} operand;

typedef struct {
  opcode_name opcode;
  operand *operands;
  int operand_count;
} token_line;

typedef int binary;

typedef struct {
  binary *pieces;
  int piece_count;
} binary_line;















// Aliases for types

typedef char* Key;
typedef uint32_t Value;

// Linked List Node
typedef struct {
  Key key;
  Value value;
  struct Node *next;
} Node;

// Linked List
typedef struct {
  Node *head;
} Symbol_Table;

// Adding to Symbol Table
void add_to_table(Symbol_Table *table, Key key, Value value) {
  Node *new_node = malloc(sizeof(Node));
  new_node->key = key;
  new_node->value = value;
  new_node->next = NULL;

  if (table->head == NULL) {
    table->head = new_node;
  } else {
    Node *current = table->head;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = new_node;
  }
}

// Removing from Symbol Table
void remove_from_table(Symbol_Table *table, Key key) {
  Node *current = table->head;
  Node *previous = NULL;

  while (current != NULL) {
    if (strcmp(current->key, key) == 0) {
      if (previous == NULL) {
        table->head = current->next;
      } else {
        previous->next = current->next;
      }
      free(current);
      return;
    }
    previous = current;
    current = current->next;
  }
}

// Finding in Symbol Table
Value find_in_table(Symbol_Table *table, Key key) {
  Node *current = table->head;
  while (current != NULL) {
    if (strcmp(current->key, key) == 0) {
      return current->value;
    }
    current = current->next;
  }
  return -1;
}

// Freeing Symbol Table
void free_table(Symbol_Table *table) {
  Node *current = table->head;
  Node *next = NULL;
  while (current != NULL) {
    next = current->next;
    free(current);
    current = next;
  }
  table->head = NULL;
}

// Check existence of key in Symbol Table
bool exists_in_table(Symbol_Table *table, Key key) {
  Node *current = table->head;
  while (current != NULL) {
    if (strcmp(current->key, key) == 0) {
      return true;
    }
    current = current->next;
  }
  return false;
}



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

// get_types_... functions get the types of every operand in the line
// They are grouped by operand pattern as seen in table 2


operand *get_types_add(operand *operands, int op_count) {
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

char *get_types_cmp(operand *operands, int op_count) {
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
    operands[1].type == REG;
    if (op_count == 3) {
      // <Rn>, <Rm>, <shift> #<imm>
      operands[2].type = SHIFT;
    } else {
      // <Rn>, <Rm>
    }
  }
}

char *get_types_and(operand *operands, int op_count) {
  // <Rd>, <Rn>, <Rm>, <shift> #<imm>
  operands[0].type = REG;
  operands[1].type = REG;
  operands[2].type = REG;
  operands[3].type = IMM;
}

char *get_types_tst(operand *operands, int op_count) {
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

char *get_types_movx(operand *operands, int op_count) {
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

char *get_types_mov(operand *operands, int op_count) {
  // <Rd>, <Rn>
  operands[0].type = REG;
  operands[1].type = REG;
}

char *get_types_mvn(operand *operands, int op_count) {
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
  

char *get_types_madd(operand *operands, int op_count) {
  // <Rd>, <Rn>, <Rm>, <Ra>
  operands[0].type = REG;
  operands[1].type = REG;
  operands[2].type = REG;
  operands[3].type = REG;
}

char *get_types_mul(operand *operands, int op_count) {
  // <Rd>, <Rn>, <Rm>
  operands[0].type = REG;
  operands[1].type = REG;
  operands[2].type = REG;
}

char *get_types_b(operand *operands, int op_count) {
  // <literal>      (label or immediate address)
  if (operands[0].word[0] == '0' && operands[0].word[1] == 'x') {
    // <address>
    operands[0].type = ADDR;
  } else {
    // <label>
    operands[0].type = LABEL;
  }
}

char *get_types_br(operand *operands, int op_count) {
  // <Xn>
  operands[0].type = REG;
}

char *get_types_str(operand *operands, int op_count) {
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

char *get_types_ldr(operand *operands, int op_count) {
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

char *get_types_dir(operand *operands, int op_count) {
  // <simm>
  operands[0].type = IMM;
}


// Converts a string into a token_line
token_line process_line(char * line) {
  // Process a line from the .s file
  const char s[] = " "; 
  const char d[] = ".";
  char * instr_str = strktok(line, s);
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

  for (char *p = strtok(sentence, c); p != NULL; p = strtok(NULL, c)) {
    strcpy(words[word_count], p);
    word_count++;
  }

  typedef operand * (*func_ptr)(operand *, int);
  func_ptr get_types;
  opcode_name opcode;

  if (strcmp(instr_str, "add") == 0) {
    opcode = ADD;
    get_types = &get_types_add;
  } else if (strcmp(instr_str, "adds") == 0) {
    opcode = ADDS;
    get_types = &get_types_add;
  } else if (strcmp(instr_str, "sub") == 0) {
    opcode = SUB;
    get_types = &get_types_add;
  } else if (strcmp(instr_str, "subs") == 0) {
    opcode = SUBS;
    get_types = &get_types_add;
  } else if (strcmp(instr_str, "cmp") == 0) {
    opcode = CMP;
    get_types = &get_types_cmp;
  } else if (strcmp(instr_str, "cmn") == 0) {
    opcode = CMN;
    get_types = &get_types_cmp;
  } else if (strcmp(instr_str, "neg") == 0) {
    opcode = NEG;
    get_types = &get_types_cmp;
  } else if (strcmp(instr_str, "negs") == 0) {
    opcode = NEGS;
    get_types = &get_types_cmp;
  } else if (strcmp(instr_str, "and") == 0) {
    opcode = AND;
    get_types = &get_types_and;
  } else if (strcmp(instr_str, "ands") == 0) {
    opcode = ANDS;
    get_types = &get_types_and;
  } else if (strcmp(instr_str, "bic") == 0) {
    opcode = BIC;
    get_types = &get_types_and;
  } else if (strcmp(instr_str, "bics") == 0) {
    opcode = BICS;
    get_types = &get_types_and;
  } else if (strcmp(instr_str, "eor") == 0) {
    opcode = EOR;
    get_types = &get_types_and;
  } else if (strcmp(instr_str, "orr") == 0) {
    opcode = ORR;
    get_types = &get_types_and;
  } else if (strcmp(instr_str, "eon") == 0) {
    opcode = EON;
    get_types = &get_types_and;
  } else if (strcmp(instr_str, "orn") == 0) {
    opcode = ORN;
    get_types = &get_types_and;
  } else if (strcmp(instr_str, "tst") == 0) {
    opcode = TST;
    get_types = &get_types_tst;
  } else if (strcmp(instr_str, "movk") == 0) {
    opcode = MOVK;
    get_types = &get_types_movx;
  } else if (strcmp(instr_str, "movn") == 0) {
    opcode = MOVN;
    get_types = &get_types_movx;
  } else if (strcmp(instr_str, "movz") == 0) {
    opcode = MOVZ;
    get_types = &get_types_movx;
  } else if (strcmp(instr_str, "mov") == 0) {
    opcode = MOV;
    get_types = &get_types_mov;
  } else if (strcmp(instr_str, "mvn") == 0) {
    opcode = MVN;
    get_types = &get_types_mvn;
  } else if (strcmp(instr_str, "madd") == 0) {
    opcode = MADD;
    get_types = &get_types_madd;
  } else if (strcmp(instr_str, "msub") == 0) {
    opcode = MSUB;
    get_types = &get_types_madd;
  } else if (strcmp(instr_str, "mul") == 0) {
    opcode = MUL;
    get_types = &get_types_mul;
  } else if (strcmp(instr_str, "mneg") == 0) {
    opcode = MNEG;
    get_types = &get_types_mul;
  } else if (strcmp(instr_str, "b") == 0) {
    opcode = B;
    get_types = &get_types_b;
  } else if (strcmp(instr_str, "b.") == 0) {
    opcode = BCOND;
    get_types = &get_types_b;
  } else if (strcmp(instr_str, "br") == 0) {
    opcode = BR;
    get_types = &get_types_br;
  } else if (strcmp(instr_str, "str") == 0) {
    opcode = STR;
    get_types = &get_types_str;
  } else if (strcmp(instr_str, "ldr") == 0) {
    opcode = LDR;
    if (word_count == 2) {
      get_types = &get_types_ldr;
    } else {
      get_types = &get_types_str;
    }
  } else if (strcmp(instr_str, "nop") == 0) {
    opcode = NOP;
  } else if (strcmp(instr_str, ".int") == 0) { 
    opcode = DIR;
    get_types = &get_types_dir;
  } else /* default: */ {
    opcode = UNRECOGNISED;
  }


  operand *current_operands;
  token_line current_line;

  operand current_operands[word_count];
  for (int i = 0; i < word_count; i++) {
    current_operands[i].word = words[i];
  }

  if (get_types != NULL) {
    current_operands = get_types(current_operands, word_count);
    current_line.opcode = opcode;
    current_line.operands = current_operands;
    current_line.operand_count = word_count;
  }

  return current_line;
}

// Reads an assembly code file and processes each line into a token_line
token_line *read_assembly(FILE *fp, int nlines) {
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  int count = 0;
  token_line token_lines[nlines];

  while ((read = getline(&line, &len, fp)) != -1) {
    token_lines[count] = process_line(line);
    count++;
  }

  fclose(fp);
  free(line);
  return token_lines;
}

// Writes an array of binary_lines to the file given by fp
void write_to_binary_file(FILE *fp, binary_line *binary_lines, int nlines) {
  for(int i = 0; i < nlines; i++) {
    binary_line current_line = binary_lines[i];
    char *output_line;
    for (int j = 0; j < current_line.piece_count; j++) {
      strcat(output_line, current_line.pieces[j]);
    }
    fprintf(fp, "%d", output_line);
  }

  fclose(fp);
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
