/**
 * Microshell for microcontrollers
 * ---------------------------------------------
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 */

#ifndef USHELL_H
#define USHELL_H

// depends on ucurses: https://github.com/matthiasbock/ucurses
#include <ansi.h>

// application-like functions must have the following structure
typedef void(uint8_t argc, char** argv) ushell_application_t;

// setup structure to connect commands to functions
typedef struct
{
    uint32_t count;
    char* command[];
    ushell_application_t* function[];
} ushell_application_list_t;

/**
 * @brief Initialize microshell
 */
void ushell_init(ushell_application_list_t*);

/*
 * Abstract user interface methods
 */

/*
 * The output methods must be defined
 * in the main code depending on what
 * sort of interface is used (e.g. UART).
 */
extern void ushell_output_char(uint8_t);
extern void ushell_output_string(char*);

/*
 * Abstract methods to provide
 * the microshell with the user input
 */
void ushell_input_char(uint8_t);
void ushell_input_string(char*);

#endif // USHELL_H
