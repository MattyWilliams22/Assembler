#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "emulatesetup.h"
#include "emulateio.h"

extern state STATE;

/**
 * Reads in the binary contents of a given file into memory.
 *
 * @param filename The name of the file to read in.
 */
void read_from_file(char* filename) {
  FILE *fp;

  fp = fopen(filename, "rb");
  if (fp == NULL) {
    perror("Error opening the binary file!\n");
    exit(EXIT_FAILURE);
  }

  fread(STATE.memory, sizeof(STATE.memory), 1, fp);
  fclose(fp);
}

/**
 * Writes the current state of the processor into a given file.
 *
 * @param filename The name of the file to write to.
 */
void write_to_file(char* filename) {
  FILE *fp;

  fp = fopen(filename, "w");

  if (fp == NULL) {
    perror("Error writing to file!\n");
    exit(EXIT_FAILURE);
  }

  // Print registers
  fprintf(fp, "Registers:\n");
  for (int i = 0; i < NUMBER_OF_REGISTERS; i++) {
    fprintf(fp, "X%02d    = %016lx\n", i, STATE.registers[i]);
  }

  // Print PC
  fprintf(fp, "PC     = %016lx\n", STATE.pc);

  // Print PSTATE flags
  fprintf(fp, "PSTATE : %c%c%c%c\n",
         STATE.pstate.n ? 'N' : '-',
         STATE.pstate.z ? 'Z' : '-',
         STATE.pstate.c ? 'C' : '-',
         STATE.pstate.v ? 'V' : '-');

  // Print memory
  fprintf(fp, "Non-Zero Memory:\n");
  for (int i = 0; i < MEMORY_SIZE / sizeof(uint32_t); i++) {
    if (STATE.memory[i] != 0) {
      fprintf(fp, "0x%08lx : %08x\n", i * sizeof(uint32_t), STATE.memory[i]);
    }
  }

  fclose(fp);
}

/**
 * Outputs the current state of the processor to stdout.
 */
void print_output(void) {
  // Print registers
  printf("Registers:\n");
  for (int i = 0; i < NUMBER_OF_REGISTERS; i++) {
    printf("X%02d    = %016lx\n", i, STATE.registers[i]);
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