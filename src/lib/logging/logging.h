#ifndef LIB_LOGGING_LOGGING_H
#define LIB_LOGGING_LOGGING_H

// Define all logging levels. 
// Values above those will be treated as the maximum logging level.
#define TRACE_LVL 0
#define DEBUG_LVL 1
#define INFO_LVL 2
#define WARN_LVL 3
#define ERROR_LVL 4
#define FATAL_LVL 5

// Implement shortcut as macros for performance sake.
#define log_format(level, format, ...) if (log_level >= level) logging_format_tofile(stdout, level, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)
#define flog_format(stream, level, format, ...) if (log_level >= level) logging_format_tofile(stream, level, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)
#define vlog_format(level, format, args) if (log_level >= level) logging_format_varargs_tofile(stdout, level, __FILE__, __LINE__, __func__, format, args)
#define vflog_format(stream, level, format, args) if (log_level >= level) logging_format_varargs_tofile(stream, level, __FILE__, __LINE__, __func__, format, args)

#define log_trace(format, ...) if (log_level >= TRACE_LVL) logging_format_tofile(stdout, TRACE_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)
#define flog_trace(stream, format, ...) if (log_level >= TRACE_LVL) logging_format_tofile(stream, TRACE_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)

#define log_debug(format, ...) if (log_level >= DEBUG_LVL) logging_format_tofile(stdout, DEBUG_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)
#define flog_debug(stream, format, ...) if (log_level >= DEBUG_LVL) logging_format_tofile(stream, DEBUG_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)

#define log_info(format, ...) if (log_level >= INFO_LVL) logging_format_tofile(stdout, INFO_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)
#define flog_info(stream, format, ...) if (log_level >= INFO_LVL) logging_format_tofile(stream, INFO_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)

#define log_warn(format, ...) if (log_level >= WARN_LVL) logging_format_tofile(stdout, WARN_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)
#define flog_warn(stream, format, ...) if (log_level >= WARN_LVL) logging_format_tofile(stream, WARN_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)

#define log_error(format, ...) if (log_level >= ERROR_LVL) logging_format_tofile(stdout, ERROR_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)
#define flog_error(stream, format, ...) if (log_level >= ERROR_LVL) logging_format_tofile(stream, ERROR_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)

#define log_fatal(format, ...) if (log_level >= FATAL_LVL) logging_format_tofile(stdout, FATAL_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)
#define flog_fatal(stream, format, ...) if (log_level >= FATAL_LVL) logging_format_tofile(stream, FATAL_LVL, __FILE__, __LINE__, __func__, format, ## __VA_ARGS__)

// The log level of the application. 
// All logs of this level or above will be logged.
extern unsigned int log_level;

// The buffer size to use for logging.
extern const unsigned int LOG_BUFFER_SIZE;

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
void logging_format_tofile(FILE* stream, const unsigned int level, const char *file, int line, const char *func, const char* format, ...) ;

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
void logging_format_varargs_tofile(FILE* stream, const unsigned int level, const char *file, int line, const char *func, const char* format, va_list args);

#endif