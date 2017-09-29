/**
 * @brief Library provides a unified way to print log messages to the shell
 */

#ifndef SYSLOG_H
#define SYSLOG_H

/*
 * Includes required for declaration
 */
#include <stdint.h>
#include <stdbool.h>


typedef enum
{
    /*
     * The message is only relevent when debugging a problem.
     */
    LOGLEVEL_DEBUG,

    /*
     * The user might be interested in this detail,
     * but it is not necessary in order to use the program.
     */
    LOGLEVEL_INFO,

    /*
     * The user should be made aware of this detail.
     */
    LOGLEVEL_NOTE,

    /*
     * Notification about a potential problem.
     */
    LOGLEVEL_WARNING,

    /*
     * Notification about a definitive problem.
     */
    LOGLEVEL_ERROR,

    /*
     * Notification about a succeeded action.
     */
    LOGLEVEL_SUCCESS,
} loglevel_t;


/**
 * Log a message with loglevel and code line
 */
void syslog(loglevel_t loglevel, char* filename, uint32_t line, char* message);

/**
 * Log a message with loglevel and code line
 *
 * Macros, which automatically include file name and line number of invocation
 */
#define __syslog(loglevel, message)     syslog(loglevel, __FILE__, __LINE__, message)
#define debug(message)                  __syslog(LOGLEVEL_DEBUG, message)
#define warning(message)                __syslog(LOGLEVEL_WARNING, message)
#define error(message)                  __syslog(LOGLEVEL_ERROR, message)
#define success(message)                __syslog(LOGLEVEL_SUCCESS, message)

#endif
