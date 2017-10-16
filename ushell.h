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

#include <stdio.h>  // setvbuf() and: Inclusion of ushell.h shall implicitly include printf etc.

// depends on newlib
#include <string.h>

// hardlink from ucurses: https://github.com/matthiasbock/ucurses
#include <ansi.h>

#include "helper.h"

// character constants
#define KEY_ESC         0x1B
#ifdef EMBEDDED
    //#define KEY_BACKSPACE   STR(KEY_ESC) "?"
    #define KEY_BACKSPACE   0x7F
    #define KEY_ENTER       0x0D
#else
    #define KEY_BACKSPACE   0x7F
    #define KEY_ENTER       0x0A
#endif
#define KEY_CTRL_C      0x03

// Note: In the embedded world, you can't log out
// of the shell, as it is running forever.
#define KEY_CTRL_D  STR(KEY_ESC) "D"

#define KEY_SPACEBAR        0x20
#define KEY_TAB             0x09

#define KEY_ESCAPE(b,c)    ((KEY_ESC << 24) | (b << 16) | (c << 8))
#define KEY_ESCAPE4(b,c,d) ((KEY_ESC << 24) | (b << 16) | (c << 8) | d)

#define KEY_UP              KEY_ESCAPE('[','A')
#define KEY_DOWN            KEY_ESCAPE('[','B')
#define KEY_RIGHT           KEY_ESCAPE('[','C')
#define KEY_LEFT            KEY_ESCAPE('[','D')
#define KEY_PAGEUP          KEY_ESCAPE('[','5')
#define KEY_PAGEDOWN        KEY_ESCAPE('[','6')
#define KEY_SHIFT_TAB       KEY_ESCAPE('[','Z')
#ifdef EMBEDDED
    #define KEY_POS1        KEY_ESCAPE4('[','1','~')
    #define KEY_END         KEY_ESCAPE4('[','4','~')
#else
    #define KEY_POS1        KEY_ESCAPE('[','H')
    #define KEY_END         KEY_ESCAPE('[','F')
#endif
#define KEY_DEL             KEY_ESCAPE('3','~')

// output macros
#define writec(c)   terminal_output_char(c);
#define write(s)    terminal_output_string(s);
#define crlf()      write("\n\r");
#define writeln(s)  write(s); crlf();
//#define warning(s)  write(ANSI_RESET ANSI_FG_YELLOW "Warning: " ANSI_RESET) writeln(s);
//#define error(s)    write(ANSI_RESET ANSI_FG_RED "Error: " ANSI_RESET); writeln(s);

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
    char* name;
    ushell_application_t function;
    char* help_brief;
} ushell_app_t;

typedef struct
{
    uint8_t count;
    ushell_app_t apps[];
} ushell_app_list_t;


/**
 * @brief Clear terminal screen
 */
#define ushell_clear()  write(ANSI_CLEAR_SCREEN ANSI_CURSOR_TO(0,0));


/**
 * @brief Initialize microshell
 */
void ushell_init(ushell_app_list_t*);


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

/*
 * Turn microshell echo on/off
 */
void ushell_echo_on();
void ushell_echo_off();

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
typedef void (*keystroke_handler_t)(uint32_t);
void ushell_attach_keystroke_handler(keystroke_handler_t);
void ushell_release_keystroke_handler();

#endif // USHELL_H
