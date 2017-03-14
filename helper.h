
#ifndef USHELL_HELPER_H
#define USHELL_HELPER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Find out, if this key is printable or not
 */
bool is_printable(uint8_t b);

/**
 * @brief Generate a binary representation of a 32-bit integer
 *
 * @param value: Value to represent
 * @param buffer: Pointer to at least 33 bytes of buffer (32 bits + string terminator)
 */
void word2binary(uint32_t value, char buffer[]);

/**
 * @brief Convert 4 bits to equivalent hexadecimal character
 *
 * @param n: Lower four bits (nibble) to represent
 */
char nibble2hex(uint8_t n);

/**
 * @brief Generate a hexadecimal representation of a byte
 *
 * @param b: Byte to represent
 * @param buffer: Pointer to at least 5 bytes (4 + string terminator)
 * @param prefix: Whether to append 0x to beginning of string
 */
void byte2hex(uint8_t b, char buffer[], bool prefix);

#endif // USHELL_HELPER_H
