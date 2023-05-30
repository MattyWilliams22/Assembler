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
  reg zr;
  uint32_t memory[MEMORY_SIZE];
} state;

state STATE;

// Writing the state of the emulator to file
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

// Printing the output of the emulator
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
    int imm12 = (instr >> 10) & 0xfff;
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
    if (rd != 31) {
      STATE.registers[rd] = result;
    }
  }

  // Wide Move - check opi is 101
  else if (opi == 0x5) {
    // Extract bit 22-21
    byte hw = (instr >> 21) & 0x3;
    // Extract bits 20-5
    int imm16 = (instr >> 5) & 0xffff;
    // Operand
    int operand = imm16 << (hw * 16);

    // Calculate the result based on the opcode
    switch (opc) {
      case 0x0:
        // MOVN
        STATE.registers[rd] = ~operand;
        break;
      case 0x2:
        // MOVZ
        STATE.registers[rd] = operand;
        break;
      case 0x3:
        // MOVK
        STATE.registers[rd] = (STATE.registers[rd] & ~(0xffff << (hw * 16))) | (imm16 << (hw * 16));
        break;
    }
  }
}

// Data Processing Register
void data_processing_register(instruction instr) {
  // Extract bit 31
  byte sf = (instr >> 31) & 0x1;
  // Extract bits 30-29
  byte opc = (instr >> 29) & 0x3;
  // Extract bit 28
  byte M = (instr >> 28) & 0x1;
  // Extract bits 24-21
  byte opr = (instr >> 21) & 0xf;
  // Extract bits 20-16
  byte rm = (instr >> 16) & 0x1f;
  // Extract bits 15-10
  byte operand = (instr >> 10) & 0x3f;
  // Extract bits 9-5
  byte rn = (instr >> 5) & 0x1f;
  // Extract bits 4-0
  byte rd = instr & 0x1f;

  reg result = rn;

  // Logic and Arithmetic Instructions

  if (M == 0) {
    // Extract bits 23-22
    byte shift_type = (instr >> 22) & 0x3;
    reg op2 = STATE.registers[rm];
    
    switch (shift_type) {
      case 0x1:
        // LSL
        op2 = (sf ? (op2 << operand) : ((op2 << operand) & 0xffffffff));
        break;
      case 0x2:
        // LSR
        op2 = op2 >> operand;
        break;
      case 0x3:
        // ASR
        bool sign_bit = (op2 >> (sf ? 63 : 31)) & 0x1;
        op2 = (op2 >> operand) | (sign_bit << ((sf ? 64 : 32) - operand));
        break;
      case 0x4:
        // ROR
        op2 = (op2 >> operand) | (op2 << ((sf ? 64 : 32) - operand));
        break;
    }

    // Logical Instructions

    if (((instr >> 24) & 0x1) == 0) {
      // Extract bit 21
      bool N = (instr >> 21) & 0x1;
      
      
      switch (opc) {
        case 0x0:
          // AND + BIC
          if (N == 0) {
            result = STATE.registers[rn] & op2;
          } else {
            result = STATE.registers[rn] & ~op2;
          }
          break;
        case 0x1:
          // ORR + ORN
          if (N == 0) {
            result = STATE.registers[rn] | op2;
          } else {
            result = STATE.registers[rn] | ~op2;
          }
          break;
        case 0x2:
          // EOR + EON
          if (N == 0) {
            result = STATE.registers[rn] ^ op2;
          } else {
            result = STATE.registers[rn] ^ ~op2;
          }
          break;
        case 0x3:
          // ANDS + BICS
          if (N == 0) {
            result = STATE.registers[rn] & op2;
          } else {
            result = STATE.registers[rn] & ~op2;
          }

          //Update Condition Flags
          STATE.pstate.n = (result >> (sf ? 63 : 31)) & 0x1;
          STATE.pstate.z = (result == 0);
          STATE.pstate.c = 0;
          STATE.pstate.v = 0;
          break;
      }
    }

    // Arithmetic Instructions

    if ((((instr >> 24) & 0x1) == 1) && (((instr >> 21) & 0x1) == 0)) {
      switch (opc) {
        case 0x0:  // ADD
          result = STATE.registers[rn] + op2;
          break;

        case 0x1:  // ADDS
          result = STATE.registers[rn] + op2;
          STATE.pstate.n = (result >> (sf ? 63 : 31)) & 0x1;
          STATE.pstate.z = (result == 0);
          STATE.pstate.c = (result < STATE.registers[rn]) || (result < op2);
          STATE.pstate.v = (((STATE.registers[rn] ^ result) >> (sf ? 63 : 31)) & 0x1) && (((STATE.registers[rn] ^ op2) >> (sf ? 63 : 31)) & 0x1);
          break;

        case 0x2:  // SUB
          result = STATE.registers[rn] - op2;
          break;

        case 0x3:  // SUBS
          result = STATE.registers[rn] - op2;
          STATE.pstate.n = (result >> (sf ? 63 : 31)) & 0x1;
          STATE.pstate.z = (result == 0);
          STATE.pstate.c = (STATE.registers[rn] >= op2);
          STATE.pstate.v = (((STATE.registers[rn] ^ result) >> (sf ? 63 : 31)) & 0x1) && (((STATE.registers[rn] ^ op2) >> (sf ? 63 : 31)) & 0x1);
          break;
      }
    }
  }

  // Multiply Instructions
  if (M == 1 && opr == 0x8) {
    // Extract bit 15
    bool x = (instr >> 15) & 0x1;
    // Extract bits 14-10
    byte ra = (instr >> 10) & 0x1f;

    if (x == 0) {
      // MADD
      if (ra == 0x1f) {
        result = STATE.registers[rn] * STATE.registers[rm];
      } else {
        result = STATE.registers[ra] + STATE.registers[rn] * STATE.registers[rm];
      }
    } else {
      // MSUB
      if (ra == 0x1f) {
        result = 0 - STATE.registers[rn] - STATE.registers[rm];
      } else {
        result = STATE.registers[ra] - STATE.registers[rn] - STATE.registers[rm];
      }
    }
  }
  
  if (rd != 31) {
    STATE.registers[rd] = result;
  }
}

// Single Data Transfer
void single_data_transfer(instruction instr) {
  // Extract bit 31
  byte load_lit = (instr >> 31) & 0x1;
  // Extract bit 30
  byte sf = (instr >> 30) & 0x1;
  // Extract bit 24
  byte U = (instr >> 24) & 0x1;
  // Extract bit 22
  byte L = (instr >> 22) & 0x1;
  // Extract bits 15 to 10
  byte reg_offset = (instr >> 10) & 0x3f;
  // Extract bit 11
  byte I = (instr >> 11) & 0x1;
  // Extract bits 9 to 5
  byte xn = (instr >> 5) & 0x1f;
  // Extract bits 4 to 0
  byte rt = instr  & 0x1f;

  bool sign_bit;
  int mask;
  int extended;

  reg address;

  // Load Literal
  if (load_lit == 0) {
    // Extract bits 23 to 5
    int simm19 = (instr >> 5) & 0x7ffff;

    // Sign extend to 64 bits
    sign_bit = (simm19 >> 18) & 0x1;
    mask = sign_bit << 18;
    extended = (simm19 ^ mask) - mask;

    address = STATE.pc + (extended * 4);
    //STATE.registers[rt] = STATE.memory[(STATE.pc + (extended * 4)) / 4];
  } else {
    // Unsigned Immediate Offset
    if (U == 1) {
      // Extract bits 21 to 10
      int imm12 = (instr >> 10) & 0xfff;

      // Zero extend to 64 bits
      sign_bit = (imm12 >> 11) & 0x1;
      mask = sign_bit << 11;
      extended = (imm12 ^ mask) - mask;

      address = STATE.registers[xn] + extended;
    // Register Offset
    } else if (reg_offset == 0x1a) {
      // Extract bits 20 to 16
      byte xm = (instr >> 16) & 0x1f;

      address = STATE.registers[xn] + STATE.registers[xm];
    // Pre-Indexed
    } else if (I == 1) {
      // Extract bits 20 to 12
      int simm9 = (instr >> 12) & 0x1ff;

      address = STATE.registers[xn] + simm9 * 4;
      STATE.registers[xn] = address;
    // Post-Indexed
    } else if (I == 0) {
      // Extract bits 20 to 12
      int simm9 = (instr >> 12) & 0x1ff;

      address = STATE.registers[xn];
      STATE.registers[xn] += simm9;
    }
  }

  // Load Instruction
  if (L == 1 || load_lit == 0) {
    if (sf == 1) {
      // If address isn't a multiple of 4 then will have to access 3 different memory addresses to get
      // the whole 8 bytes to load in
      if (address % 4 != 0) {
        // Most significiant (4 - mod) bytes at address stored in the lowest (4 - mod) bytes of rt
        int mod = address % 4; // mod/4 of the way into address
        reg result1 = (STATE.memory[address / 4] >> (mod * 8));

        // Zero extend to 64 bits the 8 bytes stored at (address + 3)
        mask = 0 << 31;
        long int ext = (STATE.memory[(address + 3) / 4] ^ mask) - mask;

        // Zero extend to 64 bits the least significant ((mod * 8) - 1) bytes stored at (address + 7)
        mask = 0 << ((mod * 8) - 1);
        long int ext1 = (STATE.memory[(address + 7) / 4] ^ mask) - mask;
        
        // All 8 bytes stored at (address + 3) are to be stored in rt after the lowest (4 - mod) bytes
        reg result2 = (ext << ((4 - mod) * 8));

        // The least significant (mod) bytes at (address + 7) are to be stored as the (8 - mod) most significant bytes
        // in rt
        reg result3 = (ext1 << ((8 - mod) * 8));

        STATE.registers[rt] = result1 | result2 | result3;
      // If address is a multiple of 4 then the 8 bytes to load in are in the current address and the next one,
      // so only two memory addresses to be accessed
      } else {
        // Zero extend all 8 bytes at next address to 64 bits
        mask = 0 << 31;
        long int ext = (STATE.memory[(address + 4) / 4] ^ mask) - mask;

        STATE.registers[rt] = STATE.memory[address / 4] | (ext << 32);
      }
    } else {
      if (address % 4 == 0) {
        STATE.registers[rt] = STATE.memory[address];
      } else {
        // Most significiant (4 - mod) bytes at address stored in the lowest (4 - mod) bytes of rt
        int mod = address % 4; // mod/4 of the way into address
        reg result1 = (STATE.memory[address / 4] >> (mod * 8));

        // The least significant (mod) bytes at (address + 4) are to be stored as the (8 - mod) most significant bytes
        // in rt
        reg result2 = (STATE.memory[(address + 4) / 4] << ((8 - mod) * 8));

        STATE.registers[rt] = result1 | result2;
      }
    }
  // Store Instruction
  } else {
    STATE.memory[address / 4] = (sf ? STATE.registers[rt] : (STATE.registers[rt] & 0xffffffff)); 
  }  
}

void branch_instructions(instruction instr) {
  // Extract bits 31 to 26
  byte sf = (instr >> 26) & 0x3f;
  switch (sf) {
    // Unconditional
    case 0x5:
      // Extract bits 25 to 0
      int simm26 = instr & 0x3ffffff;

      // Sign extend to 64 bits
      bool s_b = (simm26 >> 25) & 0x1;
      int m = s_b << 25;
      int ext = (simm26 ^ m) - m;

      STATE.pc = STATE.pc + ext * 4;
      break;
    case 0x35:
      // Extract bits 9 to 5
      byte xn = (instr >> 5) & 0x1f;
      STATE.pc = xn;
      break;
    case 0x15:
      // Extract bits 23 to 5
      int simm19 = (instr >> 5) & 0x7ffff;

      // Sign extend to 64 bits
      bool sign_bit = (simm19 >> 18) & 0x1;
      int mask = sign_bit << 18;
      int extended = (simm19 ^ mask) - mask;

      // Extract bits 3 to 0
      byte cond = instr & 0xf;
      switch (cond) {
        case 0x0:
          if (STATE.pstate.z == 1) {
            STATE.pc = STATE.pc + extended * 4;
          } else {
            STATE.pc += 4;
          }
          break;
        case 0x1:
          if (STATE.pstate.z == 0) {
            STATE.pc = STATE.pc + extended * 4;
          } else {
            STATE.pc += 4;
          }
          break;
        case 0xa:
          if (STATE.pstate.n == 1) {
            STATE.pc = STATE.pc + extended * 4;
          } else {
            STATE.pc += 4;
          }
          break;
        case 0xb:
          if (STATE.pstate.n != 1) {
            STATE.pc = STATE.pc + extended * 4;
          } else {
            STATE.pc += 4;
          }
          break;
        case 0xc:
          if (STATE.pstate.z == 0 && STATE.pstate.n == STATE.pstate.v) {
            STATE.pc = STATE.pc + extended * 4;
          } else {
            STATE.pc += 4;
          }
          break;
        case 0xd:
          if (!(STATE.pstate.z == 0 && STATE.pstate.n == STATE.pstate.v)) {
            STATE.pc = STATE.pc + extended * 4;
          } else {
            STATE.pc += 4;
          }
          break;
        case 0xe:
          STATE.pc = STATE.pc + extended * 4;
          break;
      }
  }
}

void initialise_memory(void) {
  // Set program counter to 0
  STATE.pc = 0;

  // Set zero register to 0
  STATE.zr = 0;

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
  int i = 0;
  instruction instr;
  int decoded;

  while (STATE.memory[i] != HALT) {
    i = STATE.pc / 4;
    instr = STATE.memory[i];
    decoded = decode(instr);

    switch(decoded) {
      // Data Processing Instruction (Immediate)
      case 0:
        data_processing_immediate(instr);
        STATE.pc += 4;
        break;
      // Branch
      case 1:
        branch_instructions(instr);
        break;
      // Single Data Transfer
      case 2:
        single_data_transfer(instr);
        STATE.pc += 4;
        break;
      // Data Processing Instruction (Register)
      case 3:
        if (instr != HALT) {
          data_processing_register(instr);
          STATE.pc += 4;
        }
        break;
      default:
    }
  }
  STATE.pstate.z = 1;
}

int main(int argc, char **argv) {

  if (argc != 3) {
    printf("Usage: ./emulate <input-filename> <output-filename>\n");
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

  // Write the state of the memory to file
  write_to_file(argv[2]);
  print_output();

  return EXIT_SUCCESS;
}
