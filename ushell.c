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
char last_command_line[MAX_LENGTH];

// whether to echo received characters back to terminal
bool ushell_echo = true;

// helper macro to empty the command line
#define clear_command_line()  length = 0; command_line[0] = '\0';


// private command setup
ushell_app_list_t* ushell_app_list = 0;

// currently running application's input handler
keystroke_handler_t current_keystroke_handler = 0;


// fallback routine, if no other method is implemented
__attribute__((weak)) void terminal_output_string(char* s)
{
	for (uint8_t i=0; i<strlen(s); i++)
	{
		terminal_output_char(*(uint8_t*)(s+i));
	}
}

inline void ushell_init(ushell_app_list_t* config)
{
    ushell_app_list = config;
    // Determine app count automatically
//    ushell_app_list->count = sizeof(config->apps)/sizeof(config->apps[0]);
    clear_command_line();
}

inline void ushell_echo_on()
{
    ushell_echo = true;
}

inline void ushell_echo_off()
{
    ushell_echo = false;
}

inline void ushell_prompt()
{
    write(
        ANSI_RESET
        ANSI_FG_CYAN
        "ushell"
        ANSI_FG_MAGENTA
        ":~$ "
        ANSI_RESET
        ANSI_CLEAR_LINE
        );
}

void ushell_help()
{
    // command list undefined
    if (ushell_app_list == 0)
        return;

    const uint8_t width_column1 = 30;
    const uint8_t width_column2 = 55;

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
    for (uint8_t i=0; i<ushell_app_list->count; i++)
    {
        ushell_app_t* app = &ushell_app_list->apps[i];
        char* s;
        uint8_t x;

        write("| ");
        if (app->name != 0)
        {
            s = app->name;
            x = strlen(app->name);
        }
        else
        {
            s = "NULL";
            x = 4;
        }
        write(s);
        for (uint8_t j=1+x; j<width_column1; j++)
            writec(' ');
        write("| ");
        if (app->help_brief[i] != 0)
        {
            s = app->help_brief;
            x = strlen(app->help_brief);
        }
        else
        {
            s = "NULL";
            x = 4;
        }
        write(s);
        for (uint8_t j=1+x; j<width_column2; j++)
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
            // cannot have more than MAX_SUBSTRINGS
            if (cc >= MAX_SUBSTRINGS)
            {
                warning("More than " STR(MAX_SUBSTRINGS) " arguments will be ignored.");
                break;
            }
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
    for (uint8_t i=0; i<ushell_app_list->count; i++)
    {
        ushell_app_t* app = &ushell_app_list->apps[i];

        if (app->name != 0
         && app->function != 0
         && strcmp(cv[0], app->name) == 0)
        {
            // command found
            // set dummy keystroke handler to prevent syslog problems
            current_keystroke_handler = (keystroke_handler_t) 1;
            // execute developer-configured function
            (*(app->function))(cc, cv);
            // clear dummy keystroke handler
            if (current_keystroke_handler == (keystroke_handler_t) 1)
                current_keystroke_handler = 0;
            return;
        }
    }

    // command not recognized
    error("Command not recognized");
    writeln(command_line);
    return;
}


/*
 * The following code allows the user
 * to browse through it's command history
 */

// static memory allocation
char* history_entry[] =
{
    "\0" "12345678901234567890123456789",
    "\0" "12345678901234567890123456789",
    "\0" "12345678901234567890123456789",
    "\0" "12345678901234567890123456789",
    "\0" "12345678901234567890123456789",
    "\0" "12345678901234567890123456789",
    "\0" "12345678901234567890123456789",
    "\0" "12345678901234567890123456789",
    "\0" "12345678901234567890123456789",
    "\0" "12345678901234567890123456789",
};

bool history_browser(uint32_t key)
{

    // 0: current command line, 1-9: previously invoked commands
    static uint8_t current_history_position = 0;
    uint8_t previous_history_position = current_history_position;

    if (key == KEY_UP && current_history_position < 9)
        current_history_position++;
    if (key == KEY_DOWN && current_history_position > 0)
        current_history_position--;

    if (current_history_position != previous_history_position)
    {
        // save current command line to history position 0
        strncpy(history_entry[previous_history_position], (char*) &command_line, length);

        // clear line
        terminal_output_string(ANSI_CURSOR_LEFT(MAX_LENGTH) ANSI_CLEAR_LINE);
        ushell_prompt();
        command_line[0] = 0;
        length = 0;

        // print selected history position
        uint8_t l = strlen(history_entry[current_history_position]);
        strncpy((char*) &command_line, history_entry[current_history_position], l);
        length = l;
        terminal_output_string(history_entry[current_history_position]);
    }

    // add current command line to command history
    if (key == KEY_ENTER)
    {
        strncpy(history_entry[1], (char*) &command_line, length);
    }

    // whether key was handled or not
    return key == KEY_UP || key == KEY_DOWN;
}

/**
 * @brief Tries to guess the rest of the user's incomplete input
 */
inline void autocomplete()
{
    // whether the user input matched any known commands
    bool matches = false;

    char *string = "";

    // check user input against all known commands
    for (uint8_t i=0; i<ushell_app_list->count; i++)
    {
        ushell_app_t* app = &ushell_app_list->apps[i];

        // null pointer? this shouldn't happen
        if (app->name == 0)
        {
            // skip
            continue;
        }

        // user input matches beginning of command
        if (beginning_matches(command_line, app->name))
        {
            // no matches yet
            if (!matches)
            {
                //crlf();
                matches = true;
            }

            string = app->name;
            break;
        }
    }

    if (matches)
    {
    	crlf();
        // redraw command line
        ushell_prompt();
        // Setup the matched command
        strncpy(command_line, string+'\0', strlen(string)+1);
        length = strlen(string);
        // Write the Command for the Prompt
        write(command_line);
    }
}

/**
 * Checks, whether an escape sequence arrived
 *
 * If no, the byte is passed through.
 * If yes, the byte is catched before being processed in ushell_input_char()
 * and returned only, when the complete escape sequence has arrived.
 */
bool catch_special_char_state_machine(uint8_t regular, uint32_t* special)
{
    static bool currently_inside_escape_sequence = false;
    static uint8_t escape_sequence_byte_nr;

    if (regular == KEY_ESC)
    {
        // begin escape sequence
        currently_inside_escape_sequence = true;
        escape_sequence_byte_nr = 1;

        // clear sequence buffer
        *special = KEY_ESC << 24;

        // catch this byte
        return true;
    }

    if (currently_inside_escape_sequence)
    {
        escape_sequence_byte_nr++;

        // append byte to buffer
        *special |= regular << ((4-escape_sequence_byte_nr)*8);

        // return sequence, if complete
        if (escape_sequence_byte_nr >= 3)
        {
            currently_inside_escape_sequence = false;
            return false;
        }
        return true;
    }

    // it was a regular, non-escaped input
    *special = regular;
    return false;
}

/**
 * Input character to microshell
 */
void ushell_input_char(uint8_t c)
{
    static uint32_t b;
    if (catch_special_char_state_machine(c, &b))
        return;

    // a running application requested input forwarding
    if (current_keystroke_handler != 0)
    {
        (*current_keystroke_handler)(b);
        return;
    }

    // allow browsing through command history
//    if (history_browser(b))
//        return;

    if (b == KEY_BACKSPACE)
    {
        if (length > 0)
        {
            length--;
            command_line[length] = '\0';
            write(ANSI_CURSOR_LEFT(1) " " ANSI_CURSOR_LEFT(1));
        }
    }
    else if (b == KEY_ENTER)
    {
        // line forward
        if (ushell_echo)
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

        // Just copy the last command for the KEY_UP feature
        strncpy(last_command_line, command_line+'\0', strlen(command_line)+1);

        // evaluate user input
        command_line_evaluator();

        // only return to command prompt,
        // if application did not request keystroke forwarding
        // i.e. wishes to remain "running"
        if (current_keystroke_handler == 0)
        {
            // clear command line for new input
            clear_command_line();

            // return to input prompt
            ushell_prompt();
        }
    }
    else if (b == KEY_CTRL_C)
    {
        // abort user input
        writeln("^C");
        clear_command_line();

        // return to input prompt
        ushell_prompt();
    }
    else if (b == KEY_TAB)
    {
        // try to autocomplete the user's input
        autocomplete();
    }
    else if (b == KEY_UP)
    {
        clear_command_line();
        // Copy last command to command_line
        strncpy(command_line, last_command_line+'\0', strlen(last_command_line)+1);
        length = strlen(last_command_line);
        // clear command line and Write the Command to the Prompt
        crlf();
        ushell_prompt();
        write(command_line);
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

            // echo char back to terminal
            if (ushell_echo)
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

void ushell_attach_keystroke_handler(keystroke_handler_t h)
{
    current_keystroke_handler = h;
}

void ushell_release_keystroke_handler()
{
    if (current_keystroke_handler != 0)
    {
        current_keystroke_handler = 0;
        clear_command_line();
        ushell_prompt();
    }
}
