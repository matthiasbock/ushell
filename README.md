# uShell

uShell (microshell) is a very small and simple shell library.
It is primarily intended to be used in microcontroller projects.

uShell provides your project with an interface-independent textual console,
e.g. via the UART interface.
It's parses a user's input into command and arguments
and uses those to execute command-associated functions.
It thus tries to implement the core functionality of a bash or similar PC shell
and aims to provide a similar yet simplified user experience.

## Configuration

In order to use uShell,
you must first define a list of programs/apps in your project,
i.e. a list of programs that shall be accessible through the shell.
The commands
"help"
and
"clear"
can't be used, as they implement fixed functions.
You may configure a help text for those commands though.

The following code example defines a list of 3 apps,
the last of which enables the user to invoke the hello_world() function:
```C
#include &lt;ushell.h&gt;

// dummy program
void hello_world(uint8_t argc, char* argv[])
{
    writeln("Hello world!");
}

// example program list
const ushell_app_list_t apps =
{
    count: 3,
    apps:
    {
        {
            name: "help",
            help_brief: "Show this help",
        },
        {
            name: "clear",
            help_brief: "Clear screen",
        },
        {
            name: "test",
            help_brief: "Just a dummy program",
            function: &hello_world
        },
    }
};
```

## Initialization

You must initialize the uShell in your main() function.
You might want to start by clearing the screen,
show the list of commands supported by your shell
alongside their corresponding help text
and
present the user an initial input prompt:
```C
void main()
{
    ushell_init(&apps);
    ushell_clear();
    ushell_help();
    ushell_prompt();

    ...
}
```

## Interface

uShell does not implement a physical interface.
Instead it expects you to implement the following functions for the shell's outputs:
```C
void terminal_output_char(uint8_t);
```
and optionally:
```C
void terminal_output_string(char*);
```
aswell as to invoke, e.g. in your UART reception (interrupt) handler:
```C
ushell_input_char(uint8_t);
```
or
```C
ushell_input_string(char*);
```
to feed the shell with input (strings are assumed to be null-terminated).

## Advanced shell programs

Usually the shell returns to the input prompt
when a function completes execution.
With uShell you can also implement
(the impression of)
a constantly running application
in a non-blocking way
by redirecting shell inputs to your own handler.

In order to do so, use:
```C
ushell_attach_keystroke_handler(keystroke_handler_t*);
```
within your program/app.
This will redirect all following user inputs to the provided handler function.
Once when your application has "completed" and you wish to return to the shell, simply run:
```C
ushell_release_keystroke_handler();
```

# Licensing

This project is free and open source licensed under the GNU GPL v3.

In layman's terms:
You may use this software in your private and commercial projects,
if you comply with the terms and conditions of the above license.
This most notably means,
that if you use this software in your project,
you must publish your project's source code
(e.g. here on GitHub).

If you don't wish to do that,
you must contact the author and buy a proprietary license.
Such a license typically costs EUR 200 per seat and year.
Usage of this software without a license is illegal and will be persecuted.

For the complete and legally binding license text please refer to the <a href="https://github.com/matthiasbock/ushell/blob/master/LICENSE">LICENSE</a> file.
