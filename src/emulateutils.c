#include <stdbool.h>
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