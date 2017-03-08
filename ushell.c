/**
 * Microshell for microcontrollers
 * ---------------------------------------------
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 */

#include "ushell.h"

// maximum length of the command line
#define MAX_LENGTH 32
// length of current command line
uint8_t length = 0;
// command line string
char command_line[MAX_LENGTH];

// maximum number of space-separated substrings in user input
#define MAX_SUBSTRINGS 3

// character constants
#define BACKSPACE   8 // 177 ? 8 ?
#define ENTER      13

// helper macro to empty the command line
#define clear()  length = 0; command_line[0] = '\0';


// private command setup
ushell_application_list_t* command_list = 0;


void ushell_init(ushell_application_list_t* config)
{
    command_list = config;
    clear();
}

void ushell_help()
{
    // command list undefined
    if (command_list == 0)
        return;

    // print help text for all available programs
    writec('+');
    writec('+');
    crlf();
    for (uint8_t i=0; i<command_list->count; i++)
    {
        write(command_list->command[i]);
        write("\t\t| ");
        writeln(command_list->help_brief[i]);
    }
    writec('+');
    writec('+');
    crlf();
}

/**
 * @brief Command input evaluator
 * Run, whenever the user hits the ENTER key.
 *
 * This function cannot be inline, since it contains the static variable enabled_previously.
 */
void command_line_evaluator()
{
    // empty input ?
    if (command_line[0] == '\0')
        return;

    // split input into substrings
    uint8_t cc = 1;
    char* cv[MAX_SUBSTRINGS];
    cv[0] = command_line;

    for (uint8_t i=1; i<MAX_LENGTH; i++)
        if (
            command_line[i] == ' '
         || command_line[i] == '='
            )
        {
            // insert string terminator
            command_line[i] = '\0';
            // begin next command part after the terminator
            cv[cc] = &command_line[i+1];
            // increment command count
            cc++;
        }

    // help
    if (strcmp(cv[0], "?") == 0
     || strcmp(cv[0], "h") == 0
     || strcmp(cv[0], "help") == 0)
    {
        ushell_help();
        return;
    }

    // clear
    if (strcmp(cv[0], "clear") == 0)
    {
        ushell_clear();
        return;
    }

    // search command setup for matching command
    for (uint8_t i=0; i<command_list->count; i++)
    {
        if (strcmp(cv[0], command_list->command[i]) == 0)
        {
            write("Recognized command nr. ");
            writec(0x30+i);
            crlf();
            // command found => execute function
            (*(command_list->function[i]))(cc, cv);
            return;
        }
    }

    // command not recognized
    error("Unrecognized command.");
    return;
}

void ushell_input_char(uint8_t b)
{
    if (is_printable(b))
    {
        if (length < MAX_LENGTH-2)
        {
            // append to command line
            command_line[length++] = b;
            command_line[length] = '\0';

            // echo on
            writec(b);
        }
        else
        {
            // terminate command line input
            writeln("\n\rAborting: Maximum command line length exceed.");
            clear();

            // return to input prompt
            ushell_prompt();
        }
    }
    else if (b == BACKSPACE)
    {
        if (length > 0)
        {
            length--;
            command_line[length] = '\0';
            writec(8);
        }
    }
    else if (b == ENTER)
    {
        // line forward
        crlf();

        // evaluate user input
        command_line_evaluator();

        // clear command line for new input
        clear();

        // return to input prompt
        ushell_prompt();
    }
}

void ushell_input_string(char* s)
{
    uint8_t i = 0;
    while (s[i] != '\0')
    {
        ushell_input_char(s[i++]);
    }
}
