#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "utils.h"

typedef void (*func_ptr_type)(operand*, int);

// Get types functions
void get_types_add(operand *operands, int op_count);
void get_types_cmp(operand *operands, int op_count);
void get_types_and(operand *operands, int op_count);
void get_types_tst(operand *operands, int op_count);
void get_types_movx(operand *operands, int op_count);
void get_types_mov(operand *operands, int op_count);
void get_types_mvn(operand *operands, int op_count);
void get_types_madd(operand *operands, int op_count);
void get_types_mul(operand *operands, int op_count);
void get_types_b(operand *operands, int op_count);
void get_types_bcond(operand *operands, int op_count);
void get_types_br(operand *operands, int op_count);
void get_types_str(operand *operands, int op_count);
void get_types_ldr(operand *operands, int op_count);
void get_types_nop(operand *operands, int op_count);
void get_types_dir(operand *operands, int op_count);
void get_types_null(operand *operands, int op_count);

token_line* read_assembly(FILE* fp, int nlines);

struct InstructionMapping {
    const char* instruction;
    opcode_name opcode;
    func_ptr_type function;
    
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
    {"b.", BCOND, &get_types_bcond},
    {"br", BR, &get_types_br},
    {"str", STR, &get_types_str},
    {"ldr", LDR, &get_types_ldr},
    {"nop", NOP, &get_types_null},
    {".int", DIR, &get_types_dir},
};

#endif