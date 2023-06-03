#include <stdint.h>
#include <stdio.h>

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
  UNRECOGNISED_OPCODE
} opcode_name;

typedef enum {
  REG,
  // May need to differentiate between IMM and SIMM
  IMM,
  SHIFT,
  ADDR,
  LABEL,
  UNRECOGNISED_OPERAND
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

typedef operand* (*func_ptr)(operand*, int);

// Get types functions
operand *get_types_add(operand *operands, int op_count);
operand *get_types_cmp(operand *operands, int op_count);
operand *get_types_and(operand *operands, int op_count);
operand *get_types_tst(operand *operands, int op_count);
operand *get_types_movx(operand *operands, int op_count);
operand *get_types_mov(operand *operands, int op_count);
operand *get_types_mvn(operand *operands, int op_count);
operand *get_types_madd(operand *operands, int op_count);
operand *get_types_mul(operand *operands, int op_count);
operand *get_types_b(operand *operands, int op_count);
operand *get_types_br(operand *operands, int op_count);
operand *get_types_str(operand *operands, int op_count);
operand *get_types_ldr(operand *operands, int op_count);
operand *get_types_dir(operand *operands, int op_count);

struct InstructionMapping {
    const char* instruction;
    opcode_name opcode;
    func_ptr function;
};

struct InstructionMapping instructionMappings[] = {
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
    {"b.", BCOND, &get_types_b},
    {"br", BR, &get_types_br},
    {"str", STR, &get_types_str},
    {"ldr", LDR, &get_types_ldr},
    {"nop", NOP, NULL},
    {".int", DIR, &get_types_dir},
};