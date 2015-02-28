#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "logging.h"

const char* ERROR_LVL =     "Error";
const char* WARN_LVL =      "Warning";
const char* INFO_LVL =      "Info";
const char* DEBUG_LVL =     "Debug";

// Logs an event to standard output.
void logft(const char* level, const char* format, ...) {
    // Preformat the current timestamp.
    time_t now = time(NULL);
    char* nowstr = ctime(&now);
    nowstr[strlen(nowstr) - 1] = '\0';

    // Preformat the message.
    va_list args;
    va_start(args, format);

    // Print the final log message.
    printf(" [%s] [%s] ", level, /*pthread_self(), */nowstr);
    vfprintf(stdout, format, args);
    printf("\n");
    va_end(args);
}

// Logs an event to a stream.
void flogft(FILE* stream, const char* level, const char* format, ...) {
    // Preformat the current timestamp.
    time_t now = time(NULL);
    char* nowstr = ctime(&now);
    nowstr[strlen(nowstr) - 1] = '\0';

    // Preformat the message.
    va_list args;
    va_start(args, format);

    // Print the final log message.
    fprintf(stream, " [%s] [%s] ", level, /*pthread_self(), */nowstr);
    vfprintf(stream, format, args);
    fprintf(stream, "\n");
    va_end(args);
}