#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "emulateutils.h"

/**
 * Given the "sf" bit of an instruction, turns the given operand into 32 bits or 64 bits.
 *
 * @param sf A single bit which decides if the operand should be accessed as a 32-bit value or 64-bit
 *           value. If sf == 1, 64-bit, else 32-bit.
 * @param operand The operand to be accessed.
 * @return The operand as a 32-bit number or 64-bit number based on the value of sf.
 */
reg choose_access_mode(byte sf, reg operand) {
  return (sf ? operand : operand & 0xffffffff);
}

/**
 * Sign or zero extends the given operand based on the type_of_extension specified.
 *
 * @param operand The number to be extended.
 * @param size_in_bits The size of the number to be extended in bits.
 * @param type_of_extension The type of extension to be perform. 0 if zero extension or 1 if sign extension
 * @return The operand either sign or zero extended
 */
reg sign_or_zero_extend(uint32_t operand, byte size_in_bits, bool type_of_extension) {
  bool sign_bit;

  if (type_of_extension == 0) {
    sign_bit = 0;
  } else {
    sign_bit = (operand >> (size_in_bits - 1)) & 0x1;
  }
  int mask = sign_bit << (size_in_bits - 1);
  long int extended = (operand ^ mask) - mask;

  return extended;
}

/**
 * Retrieves the bits within the given range of the input.
 *
 * @param input The number that the bits are to be extracted from.
 * @param start The beginning of the range of bits to extract (counting from 0).
 * @param end The end of the range of bits to extact.
 * @return The extracted bits from the input.
 */
reg retrieve_bits(reg input, reg start, reg end) {
  reg mask = 0;
  for (int i = start; i <= end; i++) {
    mask = mask | (reg) 1 << i;
  }

  return (input & mask) >> start;
}

/**
 * Set bits within the range provided within the input to to the given value.
 *
 * @param input The number that the bits are to be inserted into.
 * @param start The beginning where to insert the bits within the number.
 * @param end The end of where to insert the bits within the number.
 * @param value The value to be inserted into the given input.
 * @return The input with the value inserted into the appropriate bit positions.
 */
reg set_bits(reg input, reg start, reg end, reg value) {
  reg mask = 0;
  for (int i = start; i <= end; i++) {
    mask = mask | (reg) 1 << i;
  }

  return (input & ~mask) | (value << start);
}