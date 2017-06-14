/**
 * Some generic functions used in the microshell
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 */

#ifndef USHELL_HELPER_H
#define USHELL_HELPER_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "ushell.h"

/**
 * @brief Find out, if this key is printable or not
 */
bool is_printable(uint8_t b);

/**
 * Returns the character representing one digit (0-9)
 */
char digit2char(uint8_t);

/**
 * Fill buffer with string representation of signed integer
 */
void int2str(int, char*);

/**
 * Fill buffer with string representation of floating point integer
 */
void float2str(float*, char*);

/**
 * Fill buffer with string representation of unsigned integer
 */
void uint2str(uint32_t, char*);

/**
 * @brief Generate a binary representation of an 8-bit integer
 *
 * @param value: Value to represent
 * @param buffer: Pointer to at least 9 bytes of buffer (8 bits + string terminator)
 */
void byte2binary(uint32_t value, char buffer[]);

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

/**
 * @brief Generate a hexadecimal representation of 32-bit word
 *
 * @param b: Word to represent
 * @param buffer: Pointer to at least 11 bytes (10 + string terminator)
 * @param prefix: Whether to append 0x to beginning of string
 */
void word2hex(uint32_t w, char buffer[], bool prefix);

/**
 * @brief Check, whether the user input matches the beginning of a command (string)
 */
bool beginning_matches(char* user_input, char* complete_command);

#endif // USHELL_HELPER_H
