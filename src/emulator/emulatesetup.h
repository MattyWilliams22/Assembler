#include <stdint.h>

// Readable aliases for types
typedef uint64_t reg;
typedef uint8_t byte;
typedef uint32_t instruction;

// Type defs
typedef void (*funcptr)(instruction);

//Emulating A64 instructions
#define NUMBER_OF_REGISTERS 31
#define MEMORY_SIZE 2097152
#define HALT 0x8a000000
#define NOOP 0xd503201f

// Modelling Pstate
typedef struct {
  bool n : 1;
  bool z : 1;
  bool c : 1;
  bool v : 1;
} pstate;

// Modelling the state of memory and all registers
typedef struct {
  reg registers[NUMBER_OF_REGISTERS];
  pstate pstate;
  reg pc;
  reg zr;
  uint32_t memory[MEMORY_SIZE];
} state;

void initialise_memory(void);
void process_instructions(void);
funcptr decode(instruction instr);