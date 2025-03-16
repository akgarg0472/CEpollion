#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

/**
 * @brief Logs a formatted message with a specified log level.
 *
 * This function prints a timestamped log message with the given severity level.
 * The message format follows printf-style formatting.
 *
 * @param level The log level (e.g., "INFO", "ERROR", "DEBUG").
 * @param format The printf-style format string.
 * @param ... Variable arguments corresponding to the format specifiers.
 */
void log_message(const char *level, const char *format, ...);

#endif // LOGGER_H
