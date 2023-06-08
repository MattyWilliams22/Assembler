#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "emulatesetup.h"
#include "emulateutils.h"
#include "emulateinstructions.h"

extern state STATE;

/**
 * Given an instruction of the type "Data Processing Immediate", processes and executes the instruction.
 *
 * @param instr The "Data Processing Immediate" instruction to be executed
 */
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

    uint64_t op = (sh ? (imm12 << 12) : imm12);
    reg reg_op = choose_access_mode(sf, STATE.registers[rn]);

    // Decide which type of operation to perform based on the opcode
    switch (opc) {
      case 0x0:  // ADD
        result = reg_op + op;
        break;

      case 0x1:  // ADDS
        result = reg_op + op;
        STATE.pstate.n = (result >> (sf ? 63 : 31)) & 0x1;
        STATE.pstate.z = (result == 0);
        STATE.pstate.c = (result < reg_op);
        STATE.pstate.v = (((reg_op ^ result) >> (sf ? 63 : 31)) & 0x1) && (((reg_op ^ op) >> (sf ? 63 : 31)) & 0x1);
        break;

      case 0x2:  // SUB
        result = reg_op - op;
        break;

      case 0x3:  // SUBS
        result = reg_op - op;
        STATE.pstate.n = (result >> (sf ? 63 : 31)) & 0x1;
        STATE.pstate.z = (result == 0);
        STATE.pstate.c = (reg_op >= op);
        STATE.pstate.v = (((reg_op ^ result) >> (sf ? 63 : 31)) & 0x1) && (((reg_op ^ op) >> (sf ? 63 : 31)) & 0x1);
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
      case 0x3: {
        // MOVK
        uint64_t mask = 0xffff;
        result = (STATE.registers[rd] & ~(mask << (hw * 16))) | (imm16 << (hw * 16));
        break;
      }
      default:
        perror("Invalid Instruction Detected!\n");
        exit(EXIT_FAILURE);
    }
  }

  // If rd == 31 then it encodes the zero register and so the result should not be stored as
  // cannot write to the zero register
  if (rd != 31) {
    // Store the result in the destination register
    STATE.registers[rd] = choose_access_mode(sf, result);
  }
}

/**
 * Given an instruction of the type "Data Processing Register", processes and executes the instruction.
 *
 * @param instr The "Data Processing Register" instruction to be executed
 */
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
    reg op2 = choose_access_mode(sf, (rm == 31 ? 0 : STATE.registers[rm]));
  
    // Decide which type of shift needs to be performed
    switch (shift_type) {
      case 0x0:
        // LSL
        op2 = op2 << operand;
        break;
      case 0x1:
        // LSR
        op2 = op2 >> operand;
        break;
      case 0x2: {
        // ASR
        long int sign_bit = (op2 >> (sf ? 63 : 31)) & 0x1;
        
        op2 = (op2 >> operand);

        // Sign extend
        long int mask = sign_bit << ((sf ? 63 : 31) - operand);
        op2 = (op2 ^ mask) - mask;
        break;
      }
      case 0x3: {
        // ROR
        reg rotated = op2 << ((sf ? 64 : 32) - operand);

        op2 = (op2 >> operand) | rotated;
        break;
      }
    }

    // Logical Instructions
    if (((instr >> 24) & 0x1) == 0) {
      // Extract bit 21
      bool N = (instr >> 21) & 0x1;
      
      long int op1 = choose_access_mode(sf, (rn == 31 ? 0 : STATE.registers[rn]));
      
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
      long int op = choose_access_mode(sf, (rn == 31 ? 0 : STATE.registers[rn]));

      // Decide which type of operation to perform based on the opcode
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
          if (sf == 1) {
            STATE.pstate.c = (op >= op2);
          } else {
            STATE.pstate.c = (unsigned) op >= (unsigned) op2;
          }
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

    uint64_t op = choose_access_mode(sf, (ra == 31 ? 0 : STATE.registers[ra]));
    uint64_t op1 = choose_access_mode(sf, (rn == 31 ? 0 : STATE.registers[rn]));
    uint64_t op2 = choose_access_mode(sf, (rm == 31 ? 0 : STATE.registers[rm]));

    if (x == 0) {
      // MADD
      result = op + op1 * op2;
    } else {
      // MSUB
      result = op - op1 * op2;
    }
  }
  
  // If rd == 31 then it encodes the zero register and so the result should not be stored as
  // cannot write to the zero register
  if (rd != 31) {
    // Store the result in the destination register
    STATE.registers[rd] = choose_access_mode(sf, result);
  }
}

/**
 * Given an instruction of the type "Single Data Transfer", processes and executes the instruction.
 *
 * @param instr The "Single Data Transfer" instruction to be executed
 */
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

  reg address;

  // Load Literal
  if (load_lit == 0) {
    // Extract bits 23 to 5
    int simm19 = (instr >> 5) & 0x7ffff;

    // Sign extend to 64 bits
    int extended = sign_or_zero_extend(simm19, 19, 1);

    address = STATE.pc + (extended * 4);
  } else {
    // Unsigned Immediate Offset
    if (U == 1) {
      // Extract bits 21 to 10
      int imm12 = (instr >> 10) & 0xfff;

      // Zero extend to 64 bits
      int extended = sign_or_zero_extend(imm12, 12, 0);

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
      int extended = sign_or_zero_extend(simm9, 9, 1);

      address = STATE.registers[xn] + extended;
      STATE.registers[xn] = address;
    // Post-Indexed
    } else if (I == 0) {
      // Extract bits 20 to 12
      int simm9 = (instr >> 12) & 0x1ff;

      // Sign extend to 64 bits
      int extended = sign_or_zero_extend(simm9, 9, 1);

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
        reg address1 = retrieve_bits(STATE.memory[address / 4], (mod * 8), 31);
        STATE.registers[rt] = set_bits(STATE.registers[rt], 0, ((4 - mod) * 8) - 1, address1);

        // All 4 bytes stored at (address + 3) are to be stored in rt after the lowest (4 - mod) bytes
        reg address2 = STATE.memory[(address + 3)/ 4];
        STATE.registers[rt] = set_bits(STATE.registers[rt], ((4 - mod) * 8), ((8 - mod) * 8) - 1, address2);

        // The least significant (mod) bytes at (address + 7) are to be stored as the (mod) most significant bytes
        // in rt
        reg address3 = retrieve_bits(STATE.memory[(address + 7) / 4], 0, (mod * 8) - 1);
        STATE.registers[rt] = set_bits(STATE.registers[rt], ((8 - mod) * 8), 63, address3);

      // If address is a multiple of 4 then the 8 bytes to load in are in the current address and the next one,
      // so only two memory addresses to be accessed
      } else {
        STATE.registers[rt] = set_bits(STATE.registers[rt], 0, 31, STATE.memory[address / 4]);
        STATE.registers[rt] = set_bits(STATE.registers[rt], 32, 63, STATE.memory[(address + 4) / 4]);
      }
    } else {
      if (address % 4 == 0) {
        STATE.registers[rt] = STATE.memory[address];
      } else {
        // Most significiant (4 - mod) bytes at address stored in the lowest (4 - mod) bytes of rt
        int mod = address % 4; // mod/4 of the way into address
        reg address1 = retrieve_bits(STATE.memory[address / 4], (mod * 8), 31);
        STATE.registers[rt] = set_bits(STATE.registers[rt], 0, ((4 - mod) * 8) - 1, address1);

        // The least significant (mod) bytes at (address + 4) are to be stored as the (8 - mod) most significant bytes
        // in rt
        reg address2 = retrieve_bits(STATE.memory[(address + 4) / 4], 0, (mod * 8) - 1);
        STATE.registers[rt] = set_bits(STATE.registers[rt], ((4 - mod) * 8), 31, address2);
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
        int mod = address % 4; // mod/4 of the way into register
        reg result1 = retrieve_bits(result, 0, ((4 - mod) * 8) - 1);
        STATE.memory[address / 4] = set_bits(STATE.memory[address / 4], (mod * 8), 31, result1);

        // The next 4 bytes after the least significant (4 - mod) bytes in rt are to be stored in (address + 4) 
        reg result2 = retrieve_bits(result, ((4 - mod) * 8), ((8 - mod) * 8) - 1);
        STATE.memory[(address + 4) / 4] =  result2;

        // The most significant (mod) bytes in rt are to be stored as the (mod) least significant bytes
        // at (address + 8)
        reg result3 = retrieve_bits(result, ((8 - mod) * 8), 63);
        STATE.memory[(address + 8) / 4] = set_bits(STATE.memory[(address + 8) / 4], 0, (mod * 8) - 1, result3);

      // If address is a multiple of 4 then the 8 bytes to store from rt into memory need to be stored at
      // the current address and the following address, the lower half of rt into the current, and the upper half
      // into the following address
      } else {
        STATE.memory[address / 4] = retrieve_bits(result, 0, 31);
        STATE.memory[(address + 4) / 4] = retrieve_bits(result, 32, 63);
      }
    } else {
      // 32-bit access mode so we only want to use the lowest 32 bits of the register
      reg result = STATE.registers[rt] & 0xffffffff;

      if (address % 4 == 0) {
        STATE.memory[address / 4] = result;
      } else {
        // Least significant (4 - mod) bytes in rt stored in the most significant (4 - mod) bytes of address
        int mod = address % 4; // mod/4 of the way into register
        reg result1 = retrieve_bits(result, 0, ((4 - mod) * 8) - 1);
        STATE.memory[address / 4] = set_bits(STATE.memory[address / 4], (mod * 8), 31, result1);

        // The most significant (mod) bytes in rt are to be stored as the (mod) least significant bytes
        // at (address + 4)
        reg result2 = retrieve_bits(result, ((4 - mod) * 8), 31);
        STATE.memory[(address + 4) / 4] = set_bits(STATE.memory[(address + 4) / 4], 0, (mod * 8) - 1, result2);
      }
    }
  }  
}

/**
 * Given an instruction of the type "Branch", processes and executes the instruction.
 *
 * @param instr The "Branch" instruction to be executed
 */
void branch_instructions(instruction instr) {
  // Extract bits 31 to 26
  byte sf = (instr >> 26) & 0x3f;
  switch (sf) {
    // Unconditional
    case 0x5: {
      // Extract bits 25 to 0
      int simm26 = instr & 0x3ffffff;

      // Sign extend to 64 bits
      int extended = sign_or_zero_extend(simm26, 26, 1);

      STATE.pc = STATE.pc + extended * 4;
      break;
    }
    // Register
    case 0x35: {
      // Extract bits 9 to 5
      byte xn = (instr >> 5) & 0x1f;
      STATE.pc = STATE.registers[xn];
      break;
    }
    // Conditional
    case 0x15: {
      // Extract bits 23 to 5
      int simm19 = (instr >> 5) & 0x7ffff;

      // Sign extend to 64 bits
      int extended = sign_or_zero_extend(simm19, 19, 1);

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

      break;
    }
    default:
      perror("Invalid Instruction Detected!\n");
      exit(EXIT_FAILURE);
  }
}

/**
 * Given an instruction of the type "NOP", processes and executes the instruction.
 *
 * @param instr The "NOP" instruction to be executed
 */
void nop(instruction instr) {
  STATE.pc += 4;
}