#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "assembleutils.h"
#include <stdio.h>

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

token_line *read_assembly(FILE* fp, int nlines);

typedef struct {
    const char* instruction;
    opcode_name opcode;
    func_ptr_type function;
} InstructionMapping;

#endif