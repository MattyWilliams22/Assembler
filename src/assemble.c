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

typedef char label[];
typedef int imm;

union {
  label label;
  imm imm;
} literal;

typedef enum {
  RI,
  XI,
  WI,
  OPTIONAL,
  RNSP,
  Z12,
  IMM,
  SIMM,
  SHIFT,
  LABEL, ADD,
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



token_line process_line(char * line) {
  // Process a line from the .s file
  char* startOfSecond = strchr(line, ' ');
  size_t lengthOfFirst = startOfSecond - line;
  char* first = (char*)malloc((lengthOfFirst + 1) * sizeof(char));
  strncpy(first, line, lengthOfFirst);

  const char s[] = " "; 
  char * instr_str = strktok(line, s);
  operands current_operands;
  current_operands.count = 0;
  for (char *p = strtok(line, s); p != NULL; p = strtok(NULL, s)) {
    current_operands.words[current_operands.count] = p;


    // need to check actual type of operand
    current_operands.types[current_operands.count] = RI;


    current_operands.count++;
  }

  instr_name instr;

  if (strcmp(first, "add") == 0) 
  {
    instr = ADD;
  } 
  else if (strcmp(first, "adds") == 0)
  {
    instr = ADDS;
  }
  else if (strcmp(first, "sub") == 0)
  {
    instr = SUB;
  }
  else if (strcmp(first, "subs") == 0)
  {
    instr = SUBS;
  }
  else if (strcmp(first, "cmp") == 0)
  {
    instr = CMP;
  }
  else if (strcmp(first, "cmn") == 0)
  {
    instr = CMN;
  }
  else if (strcmp(first, "neg") == 0)
  {
    instr = NEG;
  }
  else if (strcmp(first, "negs") == 0)
  {
    instr = NEGS;
  }
  else if (strcmp(first, "and") == 0)
  {
    instr = AND;
  }
  else if (strcmp(first, "ands") == 0)
  {
    instr = ANDS;
  }
  else if (strcmp(first, "bic") == 0)
  {
    instr = BIC;
  }
  else if (strcmp(first, "bics") == 0)
  {
    instr = BICS;
  }
  else if (strcmp(first, "eor") == 0)
  {
    instr = EOR;
  }
  else if (strcmp(first, "orr") == 0)
  {
    instr = ORR;
  }
  else if (strcmp(first, "eon") == 0)
  {
    instr = EON;
  }
  else if (strcmp(first, "orn") == 0)
  {
    instr = ORN;
  }
  else if (strcmp(first, "tst") == 0)
  {
    instr = TST;
  }
  else if (strcmp(first, "movk") == 0)
  {
    instr = MOVK;
  }
  else if (strcmp(first, "movn") == 0)
  {
    instr = MOVN;
  }
  else if (strcmp(first, "movz") == 0)
  {
    instr = MOVZ;
  }
  else if (strcmp(first, "mov") == 0)
  {
    instr = MOV;
  }
  else if (strcmp(first, "mvn") == 0)
  {
    instr = MVN;
  }
  else if (strcmp(first, "madd") == 0)
  {
    instr = MADD;
  }
  else if (strcmp(first, "msub") == 0)
  {
    instr = MSUB;
  }
  else if (strcmp(first, "mul") == 0)
  {
    instr = MUL;
  }
  else if (strcmp(first, "mneg") == 0)
  {
    instr = MNEG;
  }
  else if (strcmp(first, "b") == 0)
  {
    instr = B;
  }
  else if (strcmp(first, "b.cond") == 0)
  {
    instr = BCOND;
  }
  else if (strcmp(first, "br") == 0)
  {
    instr = BR;
  }
  else if (strcmp(first, "str") == 0)
  {
    instr = STR;
  }
  else if (strcmp(first, "ldr") == 0)
  {
    instr = LDR;
  }
  else if (strcmp(first, "nop") == 0)
  {
    instr = NOP;
  }
  else if (strcmp(first, ".int") == 0)
  { 
    instr = DIR;
  }
  else /* default: */
  {
    instr = UNRECOGNISED;
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
