#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "logging.h"

// Constants for logging levels associated strings.
const char* LEVEL_STRINGS[LEVEL_COUNT] = { "Trace", "Debug", "Info", "Warning", "Error", "Fatal" };

// Logs an event to standard output.
void logft(const int level, const char* format, ...) {
    if (level < loglevel) {
        // This message does not need to be logged.
        return;
    }

    // Preformat the current timestamp.
    time_t now = time(NULL);
    char* nowstr = ctime(&now);
    nowstr[strlen(nowstr) - 1] = '\0';

    // Preformat the message.
    va_list args;
    va_start(args, format);

    // Print the header of the log.
    printf(" [%s] [%s] ", 
        lvltostr(level), 
        /*pthread_self(), */
        nowstr);

    // Print the message.
    vfprintf(stdout, format, args); printf("\n");
    va_end(args);
}

// Logs an event to a stream.
void flogft(FILE* stream, const int level, const char* format, ...) {
    if (level < loglevel) {
        // This message does not need to be logged.
        return;
    }
    
    if (stream == NULL) {
        // Assume stdout.
        logft(level, format);
    }

    // Preformat the current timestamp.
    time_t now = time(NULL);
    char* nowstr = ctime(&now);
    nowstr[strlen(nowstr) - 1] = '\0';

    // Preformat the message.
    va_list args;
    va_start(args, format);

    // Print the header of the log.
    fprintf(stream, " [%s] [%s] ", 
        lvltostr(level), 
        /*pthread_self(), */
        nowstr);
        
    // Print the message.
    vfprintf(stream, format, args); fprintf(stream, "\n");
    va_end(args);
}

// Get the string representation of the logging level.
const char* lvltostr(const int level) {
    return LEVEL_STRINGS[
        level > LEVEL_COUNT - 1 ? LEVEL_COUNT : level];
}