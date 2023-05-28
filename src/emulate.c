#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

//Emulating A64 instructions

#define NUMBER_OF_REGISTERS 32
#define MEMORY_SIZE 2097152

// Readable aliases for types
typedef uint64_t reg;
typedef uint8_t byte;
typedef uint32_t instruction;

// Modelling Pstate
typedef struct {
  bool n : 1;
  bool z : 1;
  bool c : 1;
  bool v : 1;
} pstate;

// Special Registers
typedef struct {
  reg registers[NUMBER_OF_REGISTERS];
  pstate pstate;
  reg pc;
  uint32_t memory[MEMORY_SIZE];
} state;

state STATE;

// Printing the output of the emulator

void print_output() {
  // Print registers
  printf("Registers:\n");
  for (int i = 0; i < NUMBER_OF_REGISTERS; i++) {
    printf("X%-2d    = %016x\n", i, STATE.registers[i]);
  }

  // Print PC
  printf("PC     = %016x\n", STATE.pc);

  // Print PSTATE flags
  printf("PSTATE : %c-%c-%c-%c\n",
         STATE.pstate.n ? 'N' : '-',
         STATE.pstate.z ? 'Z' : '-',
         STATE.pstate.c ? 'C' : '-',
         STATE.pstate.v ? 'V' : '-');

  // Print memory
  printf("Non-zero memory:\n");
  for (int i = 0; i < MEMORY_SIZE; i++) {
    if (STATE.memory[i] != 0) {
      printf("0x%08x: 0x%08x\n", i * 4, bswap_32(STATE.memory[i]));
    }
  }

}

int main(int argc, char **argv) {

  if (argc != 2) {
    printf("Usage: ./emulator <filename>\n");
    return EXIT_FAILURE;
  }

  // Initialise the pstate
  STATE.pstate = (pstate) {.n = 0, .z = 0, .c = 0, .v = 0};
  
  // Set all registers to 0
  for (int i = 0; i < NUMBER_OF_REGISTERS; i++) {
    STATE.registers[i] = 0;
  }

  // Set all memory to 0
  for (int i = 0; i < MEMORY_SIZE; i++) {
    STATE.memory[i] = 0;
  }

  // Load binary file into memory
  FILE *fp = fopen(argv[1], "rb");
  if (fp == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  // Read the file into memory
  fread(STATE.memory, sizeof(byte), MEMORY_SIZE, fp);
  fclose(fp);

  return EXIT_SUCCESS;
}
