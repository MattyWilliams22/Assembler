#include <stdint.h>

// Readable aliases for types
typedef uint32_t instruction;

// Prototypes
void data_processing_immediate(instruction instr);
void data_processing_register(instruction instr);
void single_data_transfer(instruction instr);
void branch_instructions(instruction instr);
void nop(instruction instr);