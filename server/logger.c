#include "logger.h"

static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Logs a formatted message with a timestamp, log level, and thread information.
 *
 * This function generates a log message with the current timestamp (with milliseconds),
 * the specified log level, and the thread name (or ID if unavailable). The message
 * supports printf-style formatting.
 *
 * @param level The log level (e.g., "INFO", "ERROR", "DEBUG").
 * @param format The printf-style format string.
 * @param ... Variable arguments for formatting.
 */
void log_message(const char *level, const char *format, ...)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct tm t;
    localtime_r(&tv.tv_sec, &t);

    char timestamp[24];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &t);

    char timestamp_ms[30];
    snprintf(timestamp_ms, sizeof(timestamp_ms), "%s.%03d", timestamp, (int)(tv.tv_usec / 1000));

    pthread_t tid = pthread_self();
    char thread_name[16] = "(unknown)";

    if (pthread_getname_np(tid, thread_name, sizeof(thread_name)) != 0)
    {
        snprintf(thread_name, sizeof(thread_name), "%lu", (unsigned long)tid);
    }

    char log_msg[256];
    va_list args;
    va_start(args, format);
    int needed_size = vsnprintf(log_msg, sizeof(log_msg), format, args);
    va_end(args);

    if (needed_size < 0)
    {
        snprintf(log_msg, sizeof(log_msg), "LOG ERROR: Formatting failed");
    }

    pthread_mutex_lock(&log_mutex);
    printf("[%s] [%s] [Thread: %s] %s\n", timestamp_ms, level, thread_name, log_msg);
    pthread_mutex_unlock(&log_mutex);
}
