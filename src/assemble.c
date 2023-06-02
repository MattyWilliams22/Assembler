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
} instr_name;

typedef enum {
  REG,
  IMM,
  SHIFT,
  LITERAL,
  UNRECOGNISED
} operand_type;

typedef struct {
  int count;
  operand_type types[10];
  char * words[10];
} operands;

typedef struct {
  instr_name opcode;
  operands operands;
} token_line;

int nlines;















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

char *process_add(operands ops) {
  ops.types[0] = REG;
  ops.types[1] = REG;
  if (ops.words[2][0] == '#') {
    ops.types[2] = IMM;
    if (ops.words[3] != NULL) {
      // <Rd>, <Rn>, #<imm>, lsl #(0|12)
      ops.types[3] = SHIFT;
      ops.count = 4;
    } else {
      // <Rd>, <Rn>, #<imm>
      ops.count = 3;
    }
  } else {
    ops.types[2] = REG;
    if (ops.words[3] != NULL) {
      // <Rd>, <Rn>, <Rm>, <shift> #<imm>
      ops.types[3] = SHIFT;
      ops.count = 4;
    } else {
      // <Rd>, <Rn>, <Rm>
      ops.count = 3
    }
  }
}

char *process_cmp(operands ops) {

  if (ops.words[1][0] == '#') {
    // <Rn>, #<imm>{, <lsl #(0|12)>}
    if (ops.words[2] != NULL) {
      // <Rn>, #<imm>, <lsl #(0|12)>
    } else {
      // <Rn>, #<imm>
    }
  } else {
    // <Rn>, <Rm>{, <shift> #<imm>}
    if (ops.words[2] != NULL) {
      // <Rn>, <Rm>, <shift> #<imm>
    } else {
      // <Rn>, <Rm>
    }
  }
}

char *process_and(operands ops) {
  // <Rd>, <Rn>, <Rm>, <shift> #<imm>
}

char *process_tst(operands ops) {
  // <Rn>, <Rm>{, <shift> #<imm>}
  if (ops.words[2] != NULL) {
    // <Rn>, <Rm>, <shift> #<imm>
  } else {
    // <Rn>, <Rm>
  }
}

char *process_movx(operands ops) {
  // <Rd>, #<imm>{, lsl #<imm>}
  if (ops.words[2] != NULL) {
    // <Rd>, #<imm>, lsl #<imm>
  } else {
    // <Rd>, #<imm>
  }
}

char *process_mov(operands ops) {
  // <Rd>, <Rn>
}

char *process_mvn(operands ops) {
  // <Rd>, <Rm>{, <shift> #<imm>}
  if (ops.words[2] != NULL) {
    // <Rd>, <Rm>, <shift> #<imm>
  } else {
    // <Rd>, <Rm>
  }
}
  

char *process_madd(operands ops) {
  // <Rd>, <Rn>, <Rm>, <Ra>
}

char *process_mul(operands ops) {
  // <Rd>, <Rn>, <Rm>
}

char *process_b(operands ops) {
  // <literal>      (label or immediate address)
  if (ops.words[0][0] == '0' && ops.words[0][1] == 'x') {
    // <address>
  } else {
    // <label>
  }
}

char *process_br(operands ops) {
  // <Xn>
}

char *process_str(operands ops) {
  int length = strlen(ops.words[2]);
  if (ops.words[2][length] == '!') {
    // <Rt>, [<Xn>, #<simm>]!                  (Pre-index)
  } else if (ops.words[2][length] != ']') {
    // <Rt>, [<Xn>], #<simm>                   (Post-index)
  } else if (ops.words[2][0] != '#') {
    // <Rt>, [<Xn>], #<simm>                   (Post-index)
  } else {
    // <Rt>, [<Xn>, <Rm>{, lsl #<amount>}]
    if (ops.words[3] != NULL) {
    // <Rt>, [<Xn>, <Rm>, lsl #<amount>]
    } else {
    // <Rt>, [<Xn>, <Rm>]
    }
  }
}

char *process_ldr(operands ops) {
  // <Rt>, <literal>
}

char *process_dir(operands ops) {
  // <simm>
}

token_line process_line(char * line) {
  // Process a line from the .s file
  const char s[] = " "; 
  const char d[] = ".";
  char * instr_str = strktok(line, s);
  instr_str = strtok(instr_str, d);
  int count = 0;
  char *words[10];

  for (char *q = strtok(NULL, d); q != NULL; q = strtok(NULL, d)) {
    strcpy(words[count], q);
    count++;
  }
  
  for (char *p = strtok(line, s); p != NULL; p = strtok(NULL, s)) {
    strcpy(words[count], p);
    count++;
  }

  char *sentence = "";
  for (int i = 0; i < count; i++) {
    char *current_word = words[i];  
    int length = strlen(current_word);
    strcat(sentence, current_word);
    if (current_word[length] != ',') {
      strcat(sentence, " ");
    }
  }

  const char c[] = ",";
  operands current_operands;
  current_operands.count = 0;
  for (char *p = strtok(sentence, c); p != NULL; p = strtok(NULL, c)) {
    strcpy(current_operands.words[current_operands.count], p);
    current_operands.count++;
  }

  typedef char (*func_ptr)(operands);
  func_ptr operand_processor;
  instr_name instr;

  if (strcmp(instr_str, "add") == 0) 
  {
    instr = ADD;
    operand_processor = &process_add;
  } 
  else if (strcmp(instr_str, "adds") == 0)
  {
    instr = ADDS;
    operand_processor = &process_add;
  }
  else if (strcmp(instr_str, "sub") == 0)
  {
    instr = SUB;
    operand_processor = &process_add;
  }
  else if (strcmp(instr_str, "subs") == 0)
  {
    instr = SUBS;
    operand_processor = &process_add;
  }
  else if (strcmp(instr_str, "cmp") == 0)
  {
    instr = CMP;
    operand_processor = &process_cmp;
  }
  else if (strcmp(instr_str, "cmn") == 0)
  {
    instr = CMN;
    operand_processor = &process_cmp;
  }
  else if (strcmp(instr_str, "neg") == 0)
  {
    instr = NEG;
    operand_processor = &process_cmp;
  }
  else if (strcmp(instr_str, "negs") == 0)
  {
    instr = NEGS;
    operand_processor = &process_cmp;
  }
  else if (strcmp(instr_str, "and") == 0)
  {
    instr = AND;
    operand_processor = &process_and;

  }
  else if (strcmp(instr_str, "ands") == 0)
  {
    instr = ANDS;
    operand_processor = &process_and;

  }
  else if (strcmp(instr_str, "bic") == 0)
  {
    instr = BIC;
    operand_processor = &process_and;

  }
  else if (strcmp(instr_str, "bics") == 0)
  {
    instr = BICS;
    operand_processor = &process_and;
  }
  else if (strcmp(instr_str, "eor") == 0)
  {
    instr = EOR;
    operand_processor = &process_and;
  }
  else if (strcmp(instr_str, "orr") == 0)
  {
    instr = ORR;
    operand_processor = &process_and;
  }
  else if (strcmp(instr_str, "eon") == 0)
  {
    instr = EON;
    operand_processor = &process_and;
  }
  else if (strcmp(instr_str, "orn") == 0)
  {
    instr = ORN;
    operand_processor = &process_and;
  }
  else if (strcmp(instr_str, "tst") == 0)
  {
    instr = TST;
    operand_processor = &process_tst;
  }
  else if (strcmp(instr_str, "movk") == 0)
  {
    instr = MOVK;
    operand_processor = &process_movx;
  }
  else if (strcmp(instr_str, "movn") == 0)
  {
    instr = MOVN;
    operand_processor = &process_movx;
  }
  else if (strcmp(instr_str, "movz") == 0)
  {
    instr = MOVZ;
    operand_processor = &process_movx;
  }
  else if (strcmp(instr_str, "mov") == 0)
  {
    instr = MOV;
    operand_processor = &process_mov;
  }
  else if (strcmp(instr_str, "mvn") == 0)
  {
    instr = MVN;
    operand_processor = &process_mvn;
  }
  else if (strcmp(instr_str, "madd") == 0)
  {
    instr = MADD;
    operand_processor = &process_madd;
  }
  else if (strcmp(instr_str, "msub") == 0)
  {
    instr = MSUB;
    operand_processor = &process_madd;
  }
  else if (strcmp(instr_str, "mul") == 0)
  {
    instr = MUL;
    operand_processor = &process_mul;
  }
  else if (strcmp(instr_str, "mneg") == 0)
  {
    instr = MNEG;
    operand_processor = &process_mul;
  }
  else if (strcmp(instr_str, "b") == 0)
  {
    instr = B;
    operand_processor = &process_b;
  }
  else if (strcmp(instr_str, "b.") == 0)
  {
    instr = BCOND;
    operand_processor = &process_b;
  }
  else if (strcmp(instr_str, "br") == 0)
  {
    instr = BR;
    operand_processor = &process_br;
  }
  else if (strcmp(instr_str, "str") == 0)
  {
    instr = STR;
    operand_processor = &process_str;
  }
  else if (strcmp(instr_str, "ldr") == 0)
  {
    instr = LDR;
    if (current_operands.count == 2) {
      operand_processor = &process_ldr;
    } else {
      operand_processor = &process_str;
    }
  }
  else if (strcmp(instr_str, "nop") == 0)
  {
    instr = NOP;
  }
  else if (strcmp(instr_str, ".int") == 0)
  { 
    instr = DIR;
    operand_processor = &process_dir;
  }
  else /* default: */
  {
    instr = UNRECOGNISED;
  }

  
  if (operand_processor != NULL) {
    operand_processor(current_operands);
  }
  token_line current_line = {instr, current_operands};
  return current_line;
}

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

void function_assembler(token_line * token_lines, int nlines) {
  // Assemble instructions of each type
}

void write_to_binary_file(FILE *fp, int lines[]) {
  for(int i = 0; i<nlines; i++) {
    fprintf(fp, "%d", lines[i]);
  }
}

int main(int argc, char **argv) {

  FILE* input = fopen(argv[1], "rt");
	if (input == NULL) {
		perror("Could not open input file.");
		exit(EXIT_FAILURE);
	}
  nlines = count_lines(input);
  token_line *token_lines = read_assembly(input, nlines);

  // Assemble instructions using function_assembler 
  // Either one or two-pass process
  int binary_lines[nlines];


  FILE* output = fopen(argv[2], "wb+");
	if (output == NULL) {
		perror("Could not open output file.");
		exit(EXIT_FAILURE);
	}

  write_to_binary_file(output, binary_lines);

  return EXIT_SUCCESS;
}
