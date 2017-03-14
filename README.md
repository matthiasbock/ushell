# uShell

uShell (microshell) is a very small and simple textual terminal emulator,
intended for microcontrollers, where memory is limited.

uShell provides an interface-independent textual terminal (e.g. via UART)
and parses user inputs to invoke configurable functions via function pointers.
It invokes those functions with the argument values,
the user entered via the console.

## Configuration
In your code you must define a list of "programs" to be recognized by the shell.
The keywords
"help"
and
"clear"
are ignored, as they implement fixed functions.
<pre>
#include <ushell.h>

// dummy program
void hello_world(uint8_t argc, char* argv[])
{
    writeln("Hello world!");
}

// example ushell program list
const ushell_application_list_t supported_commands = {
    count: 3,
    command: {
        "help",
        "clear",
        "test"
        },
    function: {
        0,
        0,
        &hello_world
        },
    help_brief: {
        "Display this help message",
        "Clear screen",
        "Just a dummy program"
        }
};
</pre>
Your terminal will now understand the command "test"
and react by invoking the above function.

## Initialization
In your main code you must initialize the shell.
It might also make sense,
to clear the screen,
show the list of supported commands with their corresponding help text
and
to display an initial user input prompt.
<pre>
void main()
{
    ushell_init(&supported_commands);
    ushell_clear();
    ushell_help();
    ushell_prompt();
}
</pre>

## Interface
uShell does not implement an interface.
It expects you to define the functions
 * void terminal_output_char(uint8_t);
 * void terminal_output_string(char*);

aswell as to invoke
 * ushell_input_char(uint8_t); e.g. in your UART reception interrupt handler

or
 * ushell_input_string(char*);

to feed the shell with input.
(Strings are assumed to be null-terminated.)

## Advanced shell programs
Usually the shell returns to the input prompt
after the invocation of a function.
If you wish to implement (the impression of)
a constantly running application,
you can use
* ushell_attach_input_handler(terminal_input_handler_t*);

within the invoked function to
attach a user input handler
of your own.
All following terminal inputs will
be forwarded to this function then.
Once when your application completed, run:
* ushell_release_input_handler();
