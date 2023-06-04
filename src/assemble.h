#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include <stdint.h>
#include "assembleutils.h"

typedef uint32_t binary;

// Prototypes
binary assemble_DP(token_line line);
binary assemble_B(token_line line);
binary assemble_SDT(token_line line);
binary assemble_SP(token_line line);

typedef binary (*func_ptr)(token_line);

struct AssembleMapping {
  opcode_name opcode;
  func_ptr function;
  binary opc;
};

struct AssembleMapping assembleMappings[] = {
    {ADD, &assemble_DP, 0x0},
    {ADDS, &assemble_DP, 0x1},
    {SUB, &assemble_DP, 0x2},
    {SUBS, &assemble_DP, 0x3},
    {CMP, &assemble_DP, NULL},
    {CMN, &assemble_DP, NULL},
    {NEG, &assemble_DP, NULL},
    {NEGS, &assemble_DP, NULL},
    {AND, &assemble_DP, 0x0},
    {ANDS, &assemble_DP, 0x3},
    {BIC, &assemble_DP, 0x0},
    {BICS, &assemble_DP, 0x3},
    {EOR, &assemble_DP, 0x2},
    {ORR, &assemble_DP, 0x1},
    {EON, &assemble_DP, 0x2},
    {ORN, &assemble_DP, 0x1},
    {TST, &assemble_DP, NULL},
    {MOVK, &assemble_DP, 0x3},
    {MOVN, &assemble_DP, 0x0},
    {MOVZ, &assemble_DP, 0x2},
    {MOV, &assemble_DP, NULL},
    {MVN, &assemble_DP, NULL},
    {MADD, &assemble_DP, NULL},
    {MSUB, &assemble_DP, NULL},
    {MUL, &assemble_DP, NULL},
    {MNEG, &assemble_DP, NULL},
    {B, &assemble_B, NULL},
    {BCOND, &assemble_B, NULL},
    {BR, &assemble_B, NULL},
    {STR, &assemble_SDT, NULL},
    {LDR, &assemble_SDT, NULL},
    {NOP, &assemble_SP, NULL},
    {DIR, &assemble_SP, NULL},
    {HALT, &assemble_SP, NULL}
};

#endif