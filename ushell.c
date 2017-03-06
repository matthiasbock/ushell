/**
 * Microshell for microcontrollers
 * ---------------------------------------------
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 */

#include "ushell.h"

// private command setup
ushell_application_list_t* command_list = 0;


void ushell_init(ushell_application_list_t* config)
{
    command_list = config;
}

