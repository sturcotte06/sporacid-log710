#ifndef LIB_LOGGING_H
#define LIB_LOGGING_H

// Define all logging levels. 
// Values above those will be treated as the maximum logging level.
#define TRACE_LVL 0
#define DEBUG_LVL 1
#define INFO_LVL 2
#define WARN_LVL 3
#define ERROR_LVL 4
#define FATAL_LVL 5

// Implement shortcut as macros for performance sake.
#define log_format(level, format, ...) flog_format(stdout, level, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)

#define log_trace(format, ...) flog_format(stdout, TRACE_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)
#define flog_trace(stream, format, ...) flog_format(stream, TRACE_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)

#define log_debug(format, ...) flog_format(stdout, DEBUG_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)
#define flog_debug(stream, format, ...) flog_format(stream, DEBUG_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)

#define log_info(format, ...) flog_format(stdout, INFO_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)
#define flog_info(stream, format, ...) flog_format(stream, INFO_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)

#define log_warn(format, ...) flog_format(stdout, WARN_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)
#define flog_warn(stream, format, ...) flog_format(stream, WARN_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)

#define log_error(format, ...) flog_format(stdout, ERROR_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)
#define flog_error(stream, format, ...) flog_format(stream, ERROR_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)

#define log_fatal(format, ...) flog_format(stdout, FATAL_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)
#define flog_fatal(stream, format, ...) flog_format(stream, FATAL_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)

// The log level of the application. 
// All logs of this level or above will be logged.
extern unsigned int loglevel;

/// <summary>
/// Logs an event to a stream.
/// If the event level is lower than the application log level, the event won't be logged.
/// </summary>
/// <param name="stream">The log output stream. If null, the event will not be logged.</param>
/// <param name="level">The level of the event.</param>
/// <param name="file">The file in which the event occured.</param>
/// <param name="line">The line at which the event occured.</param>
/// <param name="func">the function in which the event occured.</param>
/// <param name="format">The format string of the message.</param>
/// <param name="...">The variable number of arguments for the format.</param>
void flog_format(FILE* stream, const unsigned int level, const char *file, int line, const char *func, const char* format, ...) ;

/// <summary>
/// Logs an event to a stream. 
/// Takes an argument list struct instead of a variable number of argument.
/// If the event level is lower than the application log level, the event won't be logged.
/// </summary>
/// <param name="stream">The log output stream. If null, the event will not be logged.</param>
/// <param name="level">The level of the event.</param>
/// <param name="file">The file in which the event occured.</param>
/// <param name="line">The line at which the event occured.</param>
/// <param name="func">the function in which the event occured.</param>
/// <param name="format">The format string of the message.</param>
/// <param name="args">The variable number of arguments for the format.</param>
void vflog_format(FILE* stream, const unsigned int level, const char *file, int line, const char *func, const char* format, va_list args);

/// <summary>
/// Gets the formatted message for a logging event.
/// </summary>
/// <param name="fmessage">The output formatted message</param>
/// <param name="level">The level of the event.</param>
/// <param name="file">The file in which the event occured.</param>
/// <param name="line">The line at which the event occured.</param>
/// <param name="func">the function in which the event occured.</param>
/// <param name="format">The format string of the message.</param>
/// <param name="args">The variable number of arguments for the format.</param>
/// <returns>The formatted message.</returns>
void get_fmessage(char* fmessage, const unsigned int level, const char *file, int line, const char *func, const char* format, va_list args);

/// <summary>
/// Gets the string representation of the logging level.
/// </summary>
/// <param name="level">The level of the event.</param>
/// <returns>The string representation.</returns>
const char* get_level_string(const unsigned int level);

#endif