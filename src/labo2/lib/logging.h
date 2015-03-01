#define LEVEL_COUNT 6
#define TRACE_LVL 0
#define DEBUG_LVL 1
#define INFO_LVL 2
#define WARN_LVL 3
#define ERROR_LVL 4
#define FATAL_LVL 5

// The log level of the application. 
// All logs of this level or above will be logged.
extern int loglevel;

// Constants for logging levels associated strings.
extern const char* LEVEL_STRINGS[LEVEL_COUNT];

// Logs an event to standard output.
void logft(const int level, const char* format, ...);

// Logs an event to a stream.
void flogft(FILE* stream, const int level, const char* format, ...);

// Get the string representation of the logging level.
const char* lvltostr(const int level);