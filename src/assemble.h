#include <stdio.h>
#include <stdint.h>
#include "structures.h"

typedef uint32_t binary;

// Prototypes
binary assemble_DP(token_line line);
binary assemble_SPECIAL(token_line line);

typedef binary (*func_ptr)(token_line);

struct AssembleMapping {
  opcode_name opcode;
  func_ptr function;
}

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
    {B, &assemble_DP},
    {BCOND, &assemble_DP},
    {BR, &assemble_DP},
    {STR, &assemble_DP},
    {LDR, &assemble_DP},
    {NOP, &assemble_SPECIAL},
    {DIR, &assemble_SPECIAL},
    {HALT, &assemble_SPECIAL}
};