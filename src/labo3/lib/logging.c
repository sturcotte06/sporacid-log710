#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "logging.h"

#define LEVEL_COUNT 6
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// Constants for logging levels associated strings.
const char* LEVEL_STRINGS[LEVEL_COUNT] = { "Trace", "Debug", "Info", "Warning", "Error", "Fatal" };
const char* LEVEL_COLOURS[LEVEL_COUNT] = { ANSI_COLOR_RESET, ANSI_COLOR_RESET, ANSI_COLOR_GREEN, ANSI_COLOR_YELLOW, ANSI_COLOR_RED, ANSI_COLOR_RED };

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
void flog_format(FILE* stream, const unsigned int level, const char *file, int line, const char *func, const char* format, ...) {
    if (level < log_level) {
        return; // This message does not need to be logged.
    }

    if (stream == NULL) {
        return;  // Can't log to null stream (no-op).
    }

    // Load the variable args list.
    va_list args;
    va_start(args, format);

    // Get the formatted message for the event.
    char fmessage[LOG_BUFFER_SIZE];
	get_fmessage(fmessage, level, file, line, func, format, args);

    // Print the message in the stream.
    fputs(fmessage, stream);

    // Umload variable args list.
    va_end(args);
}

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
void vflog_format(FILE* stream, const unsigned int level, const char *file, int line, const char *func, const char* format, va_list args) {
    if (level < log_level) {
        return; // This message does not need to be logged.
    }

    if (stream == NULL) {
        return;  // Can't log to null stream (no-op).
    }

    // Get the formatted message for the event.
    char fmessage[LOG_BUFFER_SIZE];
    get_fmessage(fmessage, level, file, line, func, format, args);

    // Print the message in the stream.
    fputs(fmessage, stream);
}

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
void get_fmessage(char* fmessage, const unsigned int level, const char *file, int line, const char *func, const char* format, va_list args) {    
    // Preformat the current timestamp.
    time_t now = time(NULL);
    char* nowstr = ctime(&now);
    nowstr[strlen(nowstr) - 1] = '\0';

    // Preformat the location in the file.
    char location[LOG_BUFFER_SIZE];
    sprintf(location, "%s %d %s()", file, line, func);
    
    // Format the header of the log.
    int len;
    len = sprintf(fmessage, " %s%-7s | %s | %-50.50s | ",
		get_level_colour(level),
        get_level_string(level),
        nowstr,
        location);

    // Advance the pointer.
    fmessage += len;

    // Format the message.
    len = vsprintf(fmessage, format, args);

    // Advance the pointer.
    fmessage += len;

    // Add a carriage by default.
    sprintf(fmessage, "%s\n", ANSI_COLOR_RESET);
}

/// <summary>
/// Gets the string representation of the logging level.
/// </summary>
/// <param name="level">The level of the event.</param>
/// <returns>The string representation.</returns>
const char* get_level_string(const unsigned int level) {
    // All levels above the maximum level will be considered at the maximum level.
    return LEVEL_STRINGS[level > LEVEL_COUNT - 1 ? LEVEL_COUNT - 1 : level];
}

/// <summary>
/// Gets the string representation of the colour for the level.
/// </summary>
/// <param name="level">The level of the event.</param>
/// <returns>The string representation.</returns>
const char* get_level_colour(const unsigned int level) {
    // All levels above the maximum level will be considered at the maximum level.
    return LEVEL_COLOURS[level > LEVEL_COUNT - 1 ? LEVEL_COUNT - 1 : level];
}