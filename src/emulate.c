#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "prototypes.h"

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

// Special Registers
typedef struct {
  reg registers[NUMBER_OF_REGISTERS];
  pstate pstate;
  reg pc;
  reg zr;
  uint32_t memory[MEMORY_SIZE];
} state;

state STATE;

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

void read_from_file(char* filename) {
  // Load binary file into memory
  FILE *fp;

  fp = fopen(filename, "rb");
  if (fp == NULL) {
    perror("Error opening the binary file!\n");
    exit(EXIT_FAILURE);
  }

  fread(STATE.memory, sizeof(STATE.memory), 1, fp);
  fclose(fp);
}

// Decode instruction using bits 28 to 25 in instruction.
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

void process_instructions(void) {
  int i = 0;
  instruction instr;
  typedef void (*func_ptr)(instruction);
  func_ptr decoded_func;

  while (instr != HALT) {
    if (STATE.pc % 4 == 0) {
      i = STATE.pc / 4;
      instr = STATE.memory[i];
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

  reg result;

  // Arithmetic operations - check opi is 010
  if (opi == 0x2) {
    // Extract bit 22
    byte sh = (instr >> 22) & 0x1;
    // Extract bits 21-10
    uint64_t imm12 = (instr >> 10) & 0xfff;
    // Extract bits 9-5
    byte rn = (instr >> 5) & 0x1f;

    uint64_t op = imm12 << 12;
    reg reg_op;

    if (sf == 0) { // 32-bit accesss mode
      reg_op = STATE.registers[rn] & 0xffffffff;
    } else { // 64-bit access mode
      reg_op = STATE.registers[rn];
    }

    // Calculate the result based on the opcode
    switch (opc) {
      case 0x0:  // ADD
        result = reg_op + (sh ? op : imm12);
        break;

      case 0x1:  // ADDS
        result = reg_op + (sh ? op : imm12);
        STATE.pstate.n = (result >> (sf ? 63 : 31)) & 0x1;
        STATE.pstate.z = (result == 0);
        STATE.pstate.c = (result < reg_op);
        STATE.pstate.v = (((reg_op ^ result) >> (sf ? 63 : 31)) & 0x1) && (((reg_op ^ (sh ? op : imm12)) >> (sf ? 63 : 31)) & 0x1);
        break;

      case 0x2:  // SUB
        result = reg_op - (sh ? op : imm12);
        break;

      case 0x3:  // SUBS
        result = reg_op - (sh ? op : imm12);
        STATE.pstate.n = (result >> (sf ? 63 : 31)) & 0x1;
        STATE.pstate.z = (result == 0);
        STATE.pstate.c = (reg_op >= (sh ? op : imm12));
        STATE.pstate.v = (((reg_op ^ result) >> (sf ? 63 : 31)) & 0x1) && (((reg_op ^ (sh ? op : imm12)) >> (sf ? 63 : 31)) & 0x1);
        break;
    }
  }

  // Wide Move - check opi is 101
  else if (opi == 0x5) {
    // Extract bit 22-21
    byte hw = (instr >> 21) & 0x3;
    // Extract bits 20-5
    uint64_t imm16 = (instr >> 5) & 0xffff;
    // Operand
    uint64_t operand = imm16 << (hw * 16);

    // Calculate the result based on the opcode
    switch (opc) {
      case 0x0:
        // MOVN
        result = ~operand;
        break;
      case 0x2:
        // MOVZ
        result = operand;
        break;
      case 0x3:
        // MOVK
        uint64_t mask = 0xffff;
        result = (STATE.registers[rd] & ~(mask << (hw * 16))) | (imm16 << (hw * 16));
        break;
    }
  }

  // Store the result in the destination register
  if (rd != 31) {
    if (sf == 0) { // 32-bit mode
      STATE.registers[rd] = result & 0xffffffff;
    } else { // 64-bit mode
      STATE.registers[rd] = result;
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
    reg op2;

    // Check if encodes zero register
    if (rm == 31) {
      op2 = 0;
    } else {
      if (sf == 0) { // 32-bit accesss mode
        op2 = STATE.registers[rm] & 0xffffffff;
      } else { // 64-bit access mode
        op2 = STATE.registers[rm];
      }
    }
    
    switch (shift_type) {
      case 0x0:
        // LSL
        op2 = op2 << operand;
        break;
      case 0x1:
        // LSR
        op2 = op2 >> operand;
        break;
      case 0x2:
        // ASR
        long int sign_bit = (op2 >> (sf ? 63 : 31)) & 0x1;
        
        op2 = (op2 >> operand);

        // Sign extend
        long int mask = sign_bit << ((sf ? 63 : 31) - operand);
        op2 = (op2 ^ mask) - mask;

        break;
      case 0x3:
        // ROR
        reg rotated = op2 << ((sf ? 64 : 32) - operand);

        op2 = (op2 >> operand) | rotated;
        break;
    }

    // Logical Instructions

    if (((instr >> 24) & 0x1) == 0) {
      // Extract bit 21
      bool N = (instr >> 21) & 0x1;
      long int op1;

      if (sf == 0) { // 32-bit access mode
        op1 = (rn == 31 ? 0 : STATE.registers[rn]) & 0xffffffff;
      } else { // 64-bit access mode
        op1 = (rn == 31 ? 0 : STATE.registers[rn]);
      }
      
      switch (opc) {
        case 0x0:
          // AND + BIC
          if (N == 0) {
            result = op1 & op2;
          } else {
            result = op1 & ~op2;
          }
          break;
        case 0x1:
          // ORR + ORN
          if (N == 0) {
            result = op1 | op2;
          } else {
            result = op1 | ~op2;
          }
          break;
        case 0x2:
          // EOR + EON
          if (N == 0) {
            result = op1 ^ op2;
          } else {
            result = op1 ^ ~op2;
          }
          break;
        case 0x3:
          // ANDS + BICS
          if (N == 0) {
            result = op1 & op2;
          } else {
            result = op1 & ~op2;
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
      long int op;

      if (sf == 0) { // 32-bit access mode
        op = (rn == 31 ? 0 : STATE.registers[rn]) & 0xffffffff;
      } else { // 64-bit access mode
        op = (rn == 31 ? 0 : STATE.registers[rn]);
      }

      switch (opc) {
        case 0x0:  // ADD
          result = op + op2;
          break;

        case 0x1:  // ADDS
          result = op + op2;
          STATE.pstate.n = (result >> (sf ? 63 : 31)) & 0x1;
          STATE.pstate.z = (result == 0);
          STATE.pstate.c = (result < op) || (result < op2);
          STATE.pstate.v = (op < 0 && op2 < 0 && result >= 0) || (op >= 0 && op2 >= 0 && result < 0);
          break;

        case 0x2:  // SUB
          result = op - op2;
          break;

        case 0x3:  // SUBS
          result = op - op2;
          STATE.pstate.n = (result >> (sf ? 63 : 31)) & 0x1;
          STATE.pstate.z = (result == 0);
          STATE.pstate.c = (op >= op2);
          STATE.pstate.v = (((op ^ result) >> (sf ? 63 : 31)) & 0x1) && (((op ^ op2) >> (sf ? 63 : 31)) & 0x1);
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

    uint64_t op;
    uint64_t op1;
    uint64_t op2;
    
    if (sf == 0) { // 32-bit access mode
      op = (ra == 31 ? 0 : STATE.registers[ra]) & 0xffffffff;
      op1 = (rn == 31 ? 0 : STATE.registers[rn]) & 0xffffffff;
      op2 = (rm == 31 ? 0 : STATE.registers[rm]) & 0xffffffff;
    } else { // 64-bit access mode
      op = (ra == 31 ? 0 : STATE.registers[ra]);
      op1 = (rn == 31 ? 0 : STATE.registers[rn]);
      op2 = (rm == 31 ? 0 : STATE.registers[rm]);
    }

    if (x == 0) {
      // MADD
      result = op + op1 * op2;
    } else {
      // MSUB
      result = op - op1 - op2;
    }
  }
  
  if (rd != 31) { // Encodes zero register
    if (sf == 0) { // 32-bit mode
      STATE.registers[rd] = result & 0xffffffff;
    } else { // 64-bit mode
      STATE.registers[rd] = result;
    }
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
  } else {
    // Unsigned Immediate Offset
    if (U == 1) {
      // Extract bits 21 to 10
      int imm12 = (instr >> 10) & 0xfff;

      // Zero extend to 64 bits
      sign_bit = (imm12 >> 11) & 0x1;
      mask = sign_bit << 11;
      extended = (imm12 ^ mask) - mask;
    

      address = STATE.registers[xn] + (extended * 8);
    // Register Offset
    } else if (reg_offset == 0x1a) {
      // Extract bits 20 to 16
      byte xm = (instr >> 16) & 0x1f;

      address = STATE.registers[xn] + STATE.registers[xm];
    // Pre-Indexed
    } else if (I == 1) {
      // Extract bits 20 to 12
      int simm9 = (instr >> 12) & 0x1ff;

      // Sign extend to 64 bits
      sign_bit = (simm9 >> 8) & 0x1;
      mask = sign_bit << 8;
      extended = (simm9 ^ mask) - mask;

      address = STATE.registers[xn] + extended;
      STATE.registers[xn] = address;
    // Post-Indexed
    } else if (I == 0) {
      // Extract bits 20 to 12
      int simm9 = (instr >> 12) & 0x1ff;

      // Sign extend to 64 bits
      sign_bit = (simm9 >> 8) & 0x1;
      mask = sign_bit << 8;
      extended = (simm9 ^ mask) - mask;

      address = STATE.registers[xn];
      STATE.registers[xn] += extended;
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

        // The least significant (mod) bytes at (address + 7) are to be stored as the (mod) most significant bytes
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
    if (sf == 1) {
      reg result = STATE.registers[rt];

      // If address isn't a multiple of 4 then will have to access 3 different memory addresses to get
      // the whole 8 bytes to be stored
      if (address % 4 != 0) {
        // Least significiant (4 - mod) bytes in rt stored in the most significant (4 - mod) bytes of address
        int mod = address % 4; // mod/4 of the way into address

        // Need to create a mask to decide which bits of the address that we are writing to, to keep and which ones
        // to overwrite. Any bits that we are not writing to should be kept as they were before
        int mask;

        if (mod == 1) {
          mask = 0xff;
        } else if (mod == 2) {
          mask = 0xffff;
        } else {
          mask = 0xffffff;
        }

        STATE.memory[address / 4] = (result << (mod * 8)) | (STATE.memory[address / 4] & mask);

        // The next 4 bytes after the least significant (4 - mod) bytes in rt are to be stored in (address + 4) 
        STATE.memory[(address + 4) / 4] =  (result >> ((4 - mod) * 8) & 0xffffffff);

        // Need to create a mask to decide which bits of the address that we are writing to, to keep and which ones
        // to overwrite. Any bits that we are not writing to should be kept as they were before
        int mask1;

        if (mod == 1) {
          mask1 = 0xffffff00;
        } else if (mod == 2) {
          mask1 = 0xffff0000;
        } else {
          mask1 = 0xff000000;
        }

        // The most significant (mod) bytes in rt are to be stored as the (mod) least significant bytes
        // at (address + 8)
        STATE.memory[(address + 8) / 4] =  (result >> ((8 - mod) * 8)) | (STATE.memory[(address + 8) / 4] & mask1);

      // If address is a multiple of 4 then the 8 bytes to store from rt into memory need to be stored at
      // the current address and the following address, the lower half of rt into the current, and the upper half
      // into the following address
      } else {
        STATE.memory[address / 4] = result & 0xffffffff;
        STATE.memory[(address + 4) / 4] = result >> 32;
      }
    } else {
      // 32-bit access mode so we only want to use the lowest 32 bits of the register
      reg result = STATE.registers[rt] & 0xffffffff;

      if (address % 4 == 0) {
        STATE.memory[address / 4] = result;
      } else {
        // Least significant (4 - mod) bytes in rt stored in the most significant (4 - mod) bytes of address
        int mod = address % 4; // mod/4 of the way into address

        // Need to create a mask to decide which bits of the address that we are writing to, to keep and which ones
        // to overwrite. Any bits that we are not writing to should be kept as they were before
        int mask;

        if (mod == 1) {
          mask = 0xff;
        } else if (mod == 2) {
          mask = 0xffff;
        } else {
          mask = 0xffffff;
        }

        STATE.memory[address / 4] = (result << (mod * 8)) | (STATE.memory[address / 4] & mask);

        // Need to create a mask to decide which bits of the address that we are writing to, to keep and which ones
        // to overwrite. Any bits that we are not writing to should be kept as they were before
        int mask1;

        if (mod == 1) {
          mask1 = 0xffffff00;
        } else if (mod == 2) {
          mask1 = 0xffff0000;
        } else {
          mask1 = 0xff000000;
        }

        // The most significant (mod) bytes in rt are to be stored as the (mod) least significant bytes
        // at (address + 4)
        STATE.memory[(address + 4) / 4] = (result >> ((4 - mod) * 8)) | (STATE.memory[(address + 4) / 4] & mask1);
      }
    }
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
    // Register
    case 0x35:
      // Extract bits 9 to 5
      byte xn = (instr >> 5) & 0x1f;
      STATE.pc = STATE.registers[xn];
      break;
    // Conditional
    case 0x15:
      // Extract bits 23 to 5
      int simm19 = (instr >> 5) & 0x7ffff;

      // Sign extend to 64 bits
      bool sign_bit = (simm19 >> 18) & 0x1;
      int mask = sign_bit << 18;
      int extended = (simm19 ^ mask) - mask;

      // Extract bits 3 to 0
      byte cond = instr & 0xf;
      bool condition;

      switch (cond) {
        // EQ
        case 0x0:
          condition = (STATE.pstate.z == 1);
          break;
        // NE
        case 0x1:
          condition = (STATE.pstate.z == 0);
          break;
        // GE
        case 0xa:
          condition = STATE.pstate.n == STATE.pstate.v;
          break;
        // LT
        case 0xb:
          condition = STATE.pstate.n != STATE.pstate.v;
          break;
        // GT
        case 0xc:
          condition = (STATE.pstate.z == 0) && (STATE.pstate.n == STATE.pstate.v);
          break;
        // LE
        case 0xd:
          condition = !(STATE.pstate.z == 0 && STATE.pstate.n == STATE.pstate.v);
          break;
        // AL
        case 0xe:
          condition = true;
          break;
      }

      if (condition) {
        STATE.pc = STATE.pc + extended * 4;
      } else {
        STATE.pc += 4;
      }
  }
}

void nop(instruction instr) {
  STATE.pc += 4;
}

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

int main(int argc, char **argv) {

  if (argc == 2 || argc == 3) {

    // Initialise the state of the memory
    initialise_memory();

    // Load binary file into memory
    read_from_file(argv[1]);

    // Process all of the read-in instructions
    process_instructions();

    if (argc == 2) {
      // Output the state of the memory
      print_output();
    } else {
      // Write the state of the memory to the file
      write_to_file(argv[2]);
    }

    return EXIT_SUCCESS;
  } else {
    printf("Usage: ./emulate <input-filename> <output-filename>\n");
    return EXIT_FAILURE;
  }
}
