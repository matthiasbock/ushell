/**
 * ANSI escape codes
 *
 * https://en.wikipedia.org/wiki/ANSI_escape_code
 * http://bluesock.org/~willg/dev/ansi.html
 * http://ascii-table.com/ansi-escape-sequences-vt-100.php
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * This file is hereby released into the public domain.
 */

#ifndef ANSI_H
#define ANSI_H

#ifndef STR
// http://stackoverflow.com/questions/5459868/c-preprocessor-concatenate-int-to-string
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#endif

#define ANSI_ESC            "\x1b"

#define ANSI_CLEAR_LINE     ANSI_ESC "[K"
#define ANSI_CLEAR_SCREEN   ANSI_ESC "[2J"

#define ANSI_HIDE_CURSOR    ANSI_ESC "[?25l"
#define ANSI_SHOW_CURSOR    ANSI_ESC "[?25h"

#define ANSI_RESET          ANSI_ESC "[0m"

#define ANSI_FG_RED         ANSI_ESC "[31m"
#define ANSI_FG_GREEN       ANSI_ESC "[32m"
#define ANSI_FG_YELLOW      ANSI_ESC "[33m"
#define ANSI_FG_BLUE        ANSI_ESC "[34m"
#define ANSI_FG_MAGENTA     ANSI_ESC "[35m"
#define ANSI_FG_CYAN        ANSI_ESC "[36m"
#define ANSI_FG_WHITE       ANSI_ESC "[37m"

#define ANSI_BG_BLACK       ANSI_ESC "[40m"
#define ANSI_BG_RED         ANSI_ESC "[41m"
#define ANSI_BG_GREEN       ANSI_ESC "[42m"
#define ANSI_BG_YELLOW      ANSI_ESC "[43m"
#define ANSI_BG_BLUE        ANSI_ESC "[44m"
#define ANSI_BG_MAGENTA     ANSI_ESC "[45m"
#define ANSI_BG_CYAN        ANSI_ESC "[46m"
#define ANSI_BG_WHITE       ANSI_ESC "[47m"

#define ANSI_BEGIN_GRAPHICAL    ANSI_ESC "(0"
#define ANSI_END_GRAPHICAL      ANSI_ESC "(B"

#define ANSI_CURSOR_UP(i)       ANSI_ESC "[" STR(i) "A"
#define ANSI_CURSOR_DOWN(i)     ANSI_ESC "[" STR(i) "B"
#define ANSI_CURSOR_RIGHT(i)    ANSI_ESC "[" STR(i) "C"
#define ANSI_CURSOR_LEFT(i)     ANSI_ESC "[" STR(i) "D"
#define ANSI_CURSOR_TO(x, y)    ANSI_ESC "[" STR(y) ";" STR(x) "H"

#endif
