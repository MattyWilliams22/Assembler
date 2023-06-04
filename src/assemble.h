#include <stdio.h>
#include <stdint.h>
#include "utils.h"

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
};

struct AssembleMapping instructionMappings[] = {
    {ADD, &assemble_DP},
    {ADDS, &assemble_DP},
    {SUB, &assemble_DP},
    {SUBS, &assemble_DP},
    {CMP, &assemble_DP},
    {CMN, &assemble_DP},
    {NEG, &assemble_DP},
    {NEGS, &assemble_DP},
    {AND, &assemble_DP},
    {ANDS, &assemble_DP},
    {BIC, &assemble_DP},
    {BICS, &assemble_DP},
    {EOR, &assemble_DP},
    {ORR, &assemble_DP},
    {EON, &assemble_DP},
    {ORN, &assemble_DP},
    {TST, &assemble_DP},
    {MOVK, &assemble_DP},
    {MOVN, &assemble_DP},
    {MOVZ, &assemble_DP},
    {MOV, &assemble_DP},
    {MVN, &assemble_DP},
    {MADD, &assemble_DP},
    {MSUB, &assemble_DP},
    {MUL, &assemble_DP},
    {MNEG, &assemble_DP},
    {B, &assemble_B},
    {BCOND, &assemble_B},
    {BR, &assemble_B},
    {STR, &assemble_SDT},
    {LDR, &assemble_SDT},
    {NOP, &assemble_SP},
    {DIR, &assemble_SP},
    {HALT, &assemble_SP}
};