#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "assemblesetup.h"
#include <stdio.h>
#include <stdbool.h>

typedef void (*func_ptr_type)(operand*, int);

typedef struct {
    const char* instruction;
    opcode_name opcode;
    func_ptr_type function;
} InstructionMapping;

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

bool is_halt(opcode_name opcode, operand *operands, int op_count);
token_line *process_line(char *line, int line_no, token_line *lines);
token_line *read_assembly(FILE* fp, int nlines, int *line_count);
token_line alias(token_line line);

#endif