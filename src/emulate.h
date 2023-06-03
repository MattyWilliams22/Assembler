#include <stdint.h>

// Readable aliases for types
typedef uint32_t instruction;
typedef uint64_t reg;
typedef uint8_t byte;

// Type defs
typedef void (*funcptr)(instruction);

// Prototypes
void read_from_file(char* filename);
void write_to_file(char* filename);
void print_output(void);
reg choose_access_mode(byte sf, reg operand);
reg sign_or_zero_extend(uint32_t operand, byte size_in_bits, bool type_of_extension);
funcptr decode(instruction instr);
void data_processing_immediate(instruction instr);
void data_processing_register(instruction instr);
void single_data_transfer(instruction instr);
void branch_instructions(instruction instr);
void initialise_memory(void);
void process_instructions(void);
void nop(instruction instr);