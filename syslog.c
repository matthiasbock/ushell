
#include <syslog.h>

/*
 * Includes required for implementation
 */
#include <ansi.h>
#include <ushell.h>
#include <helper.h>


extern keystroke_handler_t current_keystroke_handler;
extern char command_line[];

void syslog(loglevel_t loglevel, char* filename, uint32_t line, char* message)
{
    // ushell application running?
    if (current_keystroke_handler == 0)
    {
        // goto beginning of line, clear line
        write(ANSI_CURSOR_LEFT(80) ANSI_CLEAR_LINE);
    }

    // only print filename, if provided
    if (filename != 0)
    {
        writec('[');
        write(filename);
        writec(':');
        char buf[5];
        int2str(line, buf);
        write(buf);
        write("] ");
    }

    // print message preamble depending on the message's loglevel
    switch (loglevel)
    {
        case LOGLEVEL_DEBUG:
            write(ANSI_RESET "[Debug] ");
            break;

        case LOGLEVEL_INFO:
            write(ANSI_FG_BRIGHT_WHITE "[Info] " ANSI_RESET);
            break;

        case LOGLEVEL_NOTE:
            write(ANSI_FG_BRIGHT_CYAN "[Note] " ANSI_RESET);
            break;

        case LOGLEVEL_WARNING:
            write(ANSI_FG_BRIGHT_YELLOW "[Warning] " ANSI_RESET);
            break;

        case LOGLEVEL_ERROR:
            write(ANSI_FG_BRIGHT_RED "[Error] " ANSI_RESET);
            break;

        case LOGLEVEL_SUCCESS:
            write(ANSI_FG_BRIGHT_GREEN "[Success] " ANSI_RESET);
            break;
    }

    // print message
    writeln(message);

    // reprint shell
    if (current_keystroke_handler == 0)
    {
        // reprint shell
        ushell_prompt();
        write(command_line);
    }
}
