#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "emulatesetup.h"
#include "instructions.h"

state STATE;

/**
 * Initialises all registers, special registers, and memory addresses to the starting value 0,
 * apart from the zero flag which is initialised to a 1
 */
void initialise_memory(void) {
  // Set program counter to 0
  STATE.pc = 0;

  // Set zero register to 0
  STATE.zr = 0;

  // Initialise the pstate
  STATE.pstate = (pstate) {.n = 0, .z = 1, .c = 0, .v = 0};
  
  // Set all registers to 0
  for (int i = 0; i < NUMBER_OF_REGISTERS; i++) {
    STATE.registers[i] = 0;
  }

  // Set all memory to 0
  for (int i = 0; i < MEMORY_SIZE; i++) {
    STATE.memory[i] = 0;
  }
}

/**
 * Decodes a given instruction into one of the below categories by using bits 28 to 25 of the instruction.
 *
 * @param instr The instruction to decode.
 * @return A function pointer to the function which will be able to process the given instruction.
 */
funcptr decode(instruction instr) {
  // Extract bits 28 to 25
  byte bits_28_to_25 = (instr >> 25) & 0xf;

  if (instr == NOOP) {
    return &nop;
  }

  // Decode instruction
  switch (bits_28_to_25) {
    case 0x8:
    case 0x9:
      return &data_processing_immediate;
      break;
    case 0xa:
    case 0xb:
      return &branch_instructions;
      break;
    case 0x5:
    case 0xd:
      return &data_processing_register;
      break;
    case 0xc:
      return &single_data_transfer;
      break;
    default:
      return NULL;
  }
}

/**
 * Loops through each memory address one by one, processing and executing each instruction.
 */
void process_instructions(void) {
  int i = 0;
  instruction instr;
  funcptr decoded_func;

  // Loop through memory until you reach HALT instruction
  while (instr != HALT) {
    // If PC is a multiple of 4, just read in the next address as the next instruction
    if (STATE.pc % 4 == 0) {
      i = STATE.pc / 4;
      instr = STATE.memory[i];
    // If PC is not a multiple of 4, the next instruction has to be read over the next two addresses
    } else {
      // Most significiant (4 - mod) bytes at address stored will be the lowest (4 - mod) bytes of instruction
      int mod = STATE.pc % 4; // mod/4 of the way into address
      instruction result1 = (STATE.memory[STATE.pc / 4] >> (mod * 8));

      // The least significant (mod) bytes at (address + 4) are going to be the (4 - mod) most significant bytes
      // of the instruction
      instruction result2 = (STATE.memory[(STATE.pc + 4) / 4] << ((4 - mod) * 8));

      instr = result1 | result2;
    }

    decoded_func = decode(instr);

    if (decoded_func != NULL) {
      if (instr != HALT) {
        decoded_func(instr);
        if (decoded_func != &branch_instructions && decoded_func != &nop) {
          STATE.pc += 4;
        } 
      }
    } else {
      perror("Invalid Instruction Detected!\n");
      exit(EXIT_FAILURE);
    }
  }
}