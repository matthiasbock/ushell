/**
 * Microshell for microcontrollers
 * ---------------------------------------------
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 */

#include "ushell.h"

// length of current command line
uint8_t length = 0;
// command line string
char command_line[MAX_LENGTH];

// character constants
#define BACKSPACE   0x7F
#define ENTER       0x0D
#define CTRL_C      0x03
#define TAB         0x09

// helper macro to empty the command line
#define clear_command_line()  length = 0; command_line[0] = '\0';


// private command setup
ushell_application_list_t* command_list = 0;

// currently running application's input handler
terminal_input_handler_t current_input_handler = 0;


inline void ushell_init(ushell_application_list_t* config)
{
    command_list = config;
    clear_command_line();
}

inline void ushell_prompt()
{
    write(
        ANSI_RESET
        ANSI_FG_CYAN
        "microcontroller"
        ANSI_FG_MAGENTA
        ":~$ "
        ANSI_RESET
        ANSI_CLEAR_LINE
        );
}

void ushell_help()
{
    // command list undefined
    if (command_list == 0)
        return;

    const uint8_t width_column1 = 20;
    const uint8_t width_column2 = 45;

    // upper table border
    writec('+');
    for (uint8_t i=0; i<width_column1; i++)
        writec('-');
    writec('+');
    for (uint8_t i=0; i<width_column2; i++)
        writec('-');
    writec('+');
    crlf();

    // print help text for all available programs
    for (uint8_t i=0; i<command_list->count; i++)
    {
        // command and help text pointers should never be zero
        if (command_list->command[i] == 0
        ||  command_list->help_brief[i] == 0)
        {
            // skip
            continue;
        }

        write("| ");
        write(command_list->command[i]);
        for (uint8_t j=1+strlen(command_list->command[i]); j<width_column1; j++)
            writec(' ');
        write("| ");
        write(command_list->help_brief[i]);
        for (uint8_t j=1+strlen(command_list->help_brief[i]); j<width_column2; j++)
            writec(' ');
        writec('|');
        crlf();
    }

    // lower table border
    writec('+');
    for (uint8_t i=0; i<width_column1; i++)
        writec('-');
    writec('+');
    for (uint8_t i=0; i<width_column2; i++)
        writec('-');
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
            // command found => execute function
            (*(command_list->function[i]))(cc, cv);
            return;
        }
    }

    // command not recognized
    error("Command not recognized");
    return;
}

/**
 * @brief Tries to guess the rest of the user's incomplete input
 */
inline void autocomplete()
{
    // whether the user input matched any known commands
    bool matches = false;

    // check user input against all known commands
    for (uint8_t i=0; i<command_list->count; i++)
    {
        // null pointer? this shouldn't happen
        if (command_list->command[i] == 0)
        {
            // skip
            continue;
        }

        // user input matches beginning of command
        if (beginning_matches(command_line, command_list->command[i]))
        {
            // no matches yet
            if (!matches)
            {
                crlf();
                matches = true;
            }

            // did you mean this command?
            writeln(command_list->command[i]);
        }
    }

    if (matches)
    {
        // redraw command line
        ushell_prompt();
        write(command_line);
    }
}

void ushell_input_char(uint8_t b)
{
    // a running application requested input forwarding
    if (current_input_handler != 0)
    {
        (*current_input_handler)(b);
        return;
    }

    if (b == BACKSPACE)
    {
        if (length > 0)
        {
            length--;
            command_line[length] = '\0';
            write(ANSI_CURSOR_LEFT(1) " " ANSI_CURSOR_LEFT(1));
        }
    }
    else if (b == ENTER)
    {
        // line forward
        crlf();

        #ifdef USHELL_DEBUG_INPUT
        // print command line as hexadecimal characters
        char buffer[6] = "12345";
        for (uint8_t i=0; i<length; i++)
        {
            byte2hex(command_line[i], buffer, true);
            write(buffer);
            writec(' ');
        }
        crlf();
        #endif

        // evaluate user input
        command_line_evaluator();

        // clear command line for new input
        clear_command_line();

        // return to input prompt
        ushell_prompt();
    }
    else if (b == CTRL_C)
    {
        // abort user input
        writeln("^C");
        clear_command_line();

        // return to input prompt
        ushell_prompt();
    }
    else if (b == TAB)
    {
        // try to autocomplete the user's input
        autocomplete();
    }
    else
    #ifndef USHELL_ACCEPT_NONPRINTABLE
        if (is_printable(b))
    #endif
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
            crlf();
            warning("Aborted. Maximum command line length exceed.");
            clear_command_line();

            // return to input prompt
            ushell_prompt();
        }
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


void ushell_attach_input_handler(terminal_input_handler_t f)
{
    current_input_handler = f;
}

void ushell_release_input_handler()
{
    current_input_handler = 0;
    ushell_prompt();
}
