/**
 * Microshell for microcontrollers
 * ---------------------------------------------
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 */

#ifndef USHELL_H
#define USHELL_H

#include <stdint.h>
#include <stdbool.h>

// depends on newlib
#include <string.h>

// depends on ucurses: https://github.com/matthiasbock/ucurses
#include <ansi.h>

#include "helper.h"

// output macros
#define writec(c)   terminal_output_char(c);
#define write(s)    terminal_output_string(s);
#define crlf()      write("\n\r");
#define writeln(s)  write(s); crlf();
#define warning(s)  write(ANSI_RESET ANSI_FG_YELLOW "Warning: " ANSI_RESET) writeln(s);
#define error(s)    write(ANSI_RESET ANSI_FG_RED "Error: " ANSI_RESET); writeln(s);

// if enabled, non-printable characters are accepted as input
//#define USHELL_ACCEPT_NONPRINTABLE

// if enabled, prints entire command line as hexadecimal characters
//#define USHELL_DEBUG_INPUT

// application-like functions must have the following structure
typedef void (*ushell_application_t)(uint8_t, char*[]);

// maximum length of the command line
#define MAX_LENGTH 64

// maximum number of space-separated substrings in command line
#define MAX_SUBSTRINGS 6

// maximum number of registered applications (i.e. functions)
#define MAX_APPS 16

// setup structure to connect commands to functions
// plus help texts
typedef struct
{
    const uint8_t count;
    const char* command[MAX_APPS];
    const ushell_application_t function[MAX_APPS];
    const char* help_brief[MAX_APPS];
} ushell_application_list_t;


/**
 * @brief Clear terminal screen
 */
#define ushell_clear()  write(ANSI_CLEAR_SCREEN ANSI_CURSOR_TO(0,0));


/**
 * @brief Initialize microshell
 */
void ushell_init(const ushell_application_list_t*);


/*
 * The output methods must be defined
 * in the main code depending on what
 * sort of interface is used (e.g. UART).
 */
extern void terminal_output_char(uint8_t);
extern void terminal_output_string(char*);


/**
 * @brief Event handler for user input (e.g. keystrokes via UART)
 * @param b: Received byte
 */
void ushell_input_char(uint8_t);

/**
 * @brief Event handler for user input (e.g. keystrokes via UART)
 * @param s: String of received bytes
 */
void ushell_input_string(char*);


/**
 * @brief Output a list of supported commands with help text
 */
void ushell_help();

/**
 * @brief Output user input prompt
 */
void ushell_prompt();


// referenced here, since it appears to be necessary for the function to be usable in ushell.c
void autocomplete();


/*
 * Terminal hooks allow console programs
 * to remain "running" after their initial invocation
 * i.e. to receive all of the following terminal inputs.
 * The shell will only return to the prompt
 * after the hook is released by the console program.
 */
typedef void (*terminal_input_handler_t)(uint8_t);
void ushell_attach_input_handler(terminal_input_handler_t*);
void ushell_release_input_handler();

#endif // USHELL_H
