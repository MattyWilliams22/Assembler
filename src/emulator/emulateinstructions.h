#include <stdint.h>

// Readable aliases for types
typedef uint8_t byte;
typedef uint32_t instruction;
typedef uint64_t reg;

// Prototypes
reg arithmetic_instructions(reg op1, reg op2, byte sf, byte opc);
void data_processing_immediate(instruction instr);
void data_processing_register(instruction instr);
void single_data_transfer(instruction instr);
void branch_instructions(instruction instr);
void nop(instruction instr);