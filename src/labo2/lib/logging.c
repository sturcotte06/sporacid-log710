#define LEVEL_COUNT 6

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "logging.h"

// Constants for logging levels associated strings.
const char* LEVEL_STRINGS[LEVEL_COUNT] = { "Trace", "Debug", "Info", "Warning", "Error", "Fatal" };

// Logs an event to standard output.
void log_format(const int level, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    vlog_format(level, format, args);
    va_end(args);
}

// Logs an event to a stream.
void flog_format(FILE* stream, const int level, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    vflog_format(stream, level, format, args);
    va_end(args);
}

// Logs an event to standard output. Takes an argument list struct instead of a variable number of argument.
void vlog_format(const int level, const char* format, va_list args) {
    vflog_format(stdout, level, format, args);
}

// Logs an event to a stream. Takes an argument list struct instead of a variable number of argument.
void vflog_format(FILE* stream, const int level, const char* format, va_list args) {
    if (level < loglevel) {
        // This message does not need to be logged.
        return;
    }
    
    if (stream == NULL) {
        // Delegate to log_format() default standard output.
        stream = stdout;
    }
    
    // Preformat the current timestamp.
    time_t now = time(NULL);
    char* nowstr = ctime(&now);
    nowstr[strlen(nowstr) - 1] = '\0';
    
    // Print the header of the log.
    fprintf(stream, " %-6s  %s  ", 
        lvltostr(level), 
        /*pthread_self(), */
        nowstr);
        
    // Print the message.
    vfprintf(stream, format, args); 
    fprintf(stream, "\n");
}

// Logs an event to standard output at trace level.
void log_trace(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    vlog_format(TRACE_LVL, format, args);
    va_end(args);
}

// Logs an event to a stream at trace level.
void flog_trace(FILE* stream, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    vflog_format(stream, TRACE_LVL, format, args);
    va_end(args);
}

// Logs an event to standard output at debug level.
void log_debug(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    vlog_format(DEBUG_LVL, format, args);
    va_end(args);
}

// Logs an event to a stream at debug level.
void flog_debug(FILE* stream, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    vflog_format(stream, DEBUG_LVL, format, args);
    va_end(args);
}

// Logs an event to standard output at information level.
void log_info(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    vlog_format(INFO_LVL, format, args);
    va_end(args);
}

// Logs an event to a stream at information level.
void flog_info(FILE* stream, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    vflog_format(stream, INFO_LVL, format, args);
    va_end(args);
}

// Logs an event to standard output at warning level.
void log_warn(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    vlog_format(WARN_LVL, format, args);
    va_end(args);
}

// Logs an event to a stream at warning level.
void flog_warn(FILE* stream, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    vflog_format(stream, WARN_LVL, format, args);
    va_end(args);
}

// Logs an event to standard output at error level.
void log_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    vlog_format(ERROR_LVL, format, args);
    va_end(args);
}

// Logs an event to a stream at error level.
void flog_error(FILE* stream, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    vflog_format(stream, ERROR_LVL, format, args);
    va_end(args);
}

// Logs an event to standard output at fatal level.
void log_fatal(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    vlog_format(FATAL_LVL, format, args);
    va_end(args);
}

// Logs an event to a stream at fatal level.
void flog_fatal(FILE* stream, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    vflog_format(stream, FATAL_LVL, format, args);
    va_end(args);
}

// Get the string representation of the logging level.
const char* lvltostr(const int level) {
    return LEVEL_STRINGS[
        level > LEVEL_COUNT - 1 ? LEVEL_COUNT - 1 : level];
}