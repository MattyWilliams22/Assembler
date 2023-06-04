#include <stdint.h>

typedef uint64_t reg;
typedef uint8_t byte;

reg choose_access_mode(byte sf, reg operand);
reg sign_or_zero_extend(uint32_t operand, byte size_in_bits, bool type_of_extension);