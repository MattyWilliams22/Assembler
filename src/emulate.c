#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

//Emulating A64 instructions

#define NUMBER_OF_REGISTERS 31
#define MEMORY_SIZE 2097152
#define HALT 0x8a000000

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
void print_output(void) {
  // Print registers
  printf("Registers:\n");
  for (int i = 0; i < NUMBER_OF_REGISTERS; i++) {
    printf("X%-2d    = %016lx\n", i, STATE.registers[i]);
  }

  // Print PC
  printf("PC     = %016lx\n", STATE.pc);

  // Print PSTATE flags
  printf("PSTATE : %c%c%c%c\n",
         STATE.pstate.n ? 'N' : '-',
         STATE.pstate.z ? 'Z' : '-',
         STATE.pstate.c ? 'C' : '-',
         STATE.pstate.v ? 'V' : '-');

  // Print memory
  printf("Non-Zero Memory:\n");
  for (int i = 0; i < MEMORY_SIZE / sizeof(uint32_t); i++) {
    if (STATE.memory[i] != 0) {
      printf("0x%08lx : %08x\n", i * sizeof(uint32_t), STATE.memory[i]);
    }
  }
}

enum instr_type {
  DATA_PROCESSING_IMMEDIATE,
  BRANCHES,
  SINGLE_DATA_TRANSFER,
  DATA_PROCESSING_REGISTER,
  UNRECOGNISED
};

// Decode instruction using bits 28 to 25 in instruction.
int decode(instruction instr) {
  // Extract bits 28 to 25
  byte bits_28_to_25 = (instr >> 25) & 0xf;

  // Decode instruction
  switch (bits_28_to_25) {
    case 0x8:
    case 0x9:
      return DATA_PROCESSING_IMMEDIATE;
      break;
    case 0xa:
    case 0xb:
      return BRANCHES;
      break;
    case 0x5:
    case 0xd:
      return DATA_PROCESSING_REGISTER;
      break;
    case 0xc:
      return SINGLE_DATA_TRANSFER;
      break;
    default:
      return UNRECOGNISED;
      break;
  }
}

// Data Processing Immediate
void data_processing_immediate(instruction instr) {
  // Extract bit 31
  byte sf = (instr >> 31) & 0x1;
  // Extract bits 30-29
  byte opc = (instr >> 29) & 0x3;
  // Extract bits 25-23
  byte opi = (instr >> 23) & 0x7;
  // Extract bits 4-0
  byte rd = instr & 0x1f;

  // Arithmetic operations - check opi is 010
  if (opi == 0x2) {
    // Extract bit 22
    byte sh = (instr >> 22) & 0x1;
    // Extract bits 21-10
    byte imm12 = (instr >> 10) & 0xfff;
    // Extract bits 9-5
    byte rn = (instr >> 5) & 0x1f;

    reg result;

    // Calculate the result based on the opcode
    switch (opc) {
      case 0x0:  // ADD
        result = STATE.registers[rn] + (sh ? (imm12 << 12) : imm12);
        break;

      case 0x1:  // ADDS
        result = STATE.registers[rn] + (sh ? (imm12 << 12) : imm12);
        STATE.pstate.n = (result >> (sf ? 63 : 31)) & 0x1;
        STATE.pstate.z = (result == 0);
        STATE.pstate.c = (result < STATE.registers[rn]);
        STATE.pstate.v = (((STATE.registers[rn] ^ result) >> (sf ? 63 : 31)) & 0x1) && (((STATE.registers[rn] ^ (sh ? (imm12 << 12) : imm12)) >> (sf ? 63 : 31)) & 0x1);
        break;

      case 0x2:  // SUB
        result = STATE.registers[rn] - (sh ? (imm12 << 12) : imm12);
        break;

      case 0x3:  // SUBS
        result = STATE.registers[rn] - (sh ? (imm12 << 12) : imm12);
        STATE.pstate.n = (result >> (sf ? 63 : 31)) & 0x1;
        STATE.pstate.z = (result == 0);
        STATE.pstate.c = (STATE.registers[rn] >= (sh ? (imm12 << 12) : imm12));
        STATE.pstate.v = (((STATE.registers[rn] ^ result) >> (sf ? 63 : 31)) & 0x1) && (((STATE.registers[rn] ^ (sh ? (imm12 << 12) : imm12)) >> (sf ? 63 : 31)) & 0x1);
        break;
    }

    // Store the result in the destination register
    STATE.registers[rd] = result;
  }

  // Wide Move - check opi is 101
  else if (opi == 0x5) {
    // Extract bit 22-21
    byte hw = (instr >> 21) & 0x3;
    // Extract bits 20-5
    byte imm16 = (instr >> 5) & 0xffff;

    // Calculate the result based on the opcode
    switch (opc) {
      case 0x0:
        // MOVZ
        STATE.registers[rd] = (hw ? (imm16 << 16) : imm16);
        break;
      case 0x2:
        // MOVN
        STATE.registers[rd] = ~(hw ? (imm16 << 16) : imm16);
        break;
      case 0x3:
        // MOVK

    }
  }

}

void single_data_transfer(instruction instr) {
  // Extract bit 30
  byte sf = (instr >> 24) & 0x1;
  // Extract bit 24
  byte U = (instr >> 24) & 0x1;
  // Extract bit 11
  byte I = (instr >> 11) & 0x1;
  // Extract bits 9 to 5
  byte xn = (instr >> 5) & 0x1f;
  // Extract bits 4 to 0
  byte rt = instr  & 0x1f;

  // Unsigned Immediate Offset
  if (U == 1) {
    // Extract bits 21 to 10
    byte imm12 = (instr >> 10) & 0xfff;
  }
}

void initialise_memory(void) {
  // Set program counter to 0
  STATE.pc = 0;

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
}

void process_instructions(void) {
  instruction instr;
  int decoded;

  while (STATE.memory[STATE.pc] != HALT) {
    instr = STATE.memory[STATE.pc];
    decoded = decode(instr);

    switch(decoded) {
      // Data Processing Instruction (Immediate)
      case 0:
        data_processing_immediate(instr);
        break;
      // Branch
      case 1:

        break;
      // Single Data Transfer
      case 2:
        single_data_transfer(instr);
        break;
      // Data Processing Instruction (Register)
      case 3:

        break;

    }

    STATE.pc++;
  }

}

int main(int argc, char **argv) {

  if (argc != 2) {
    printf("Usage: ./emulator <filename>\n");
    return EXIT_FAILURE;
  }

  // Initialise the state of the memory
  initialise_memory();

  // Load binary file into memory
  FILE *fp;

  fp = fopen(argv[1], "rb");
  if (fp == NULL) {
    perror("Error opening the binary file!\n");
    exit(EXIT_FAILURE);
  }

  fread(STATE.memory, sizeof(STATE.memory), 1, fp);
  fclose(fp);

  // Process all of the read-in instructions
  process_instructions();

  print_output();

  return EXIT_SUCCESS;
}
