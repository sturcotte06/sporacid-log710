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
extern const char* LEVEL_STRINGS[];

// Logs an event to standard output.
void log_format(const int level, const char* format, ...);

// Logs an event to a stream.
void flog_format(FILE* stream, const int level, const char* format, ...);

// Logs an event to standard output. Takes an argument list struct instead of a variable number of argument.
void vlog_format(const int level, const char* format, va_list args);

// Logs an event to a stream. Takes an argument list struct instead of a variable number of argument.
void vflog_format(FILE* stream, const int level, const char* format, va_list args);

// Logs an event to standard output at trace level.
void log_trace(const char* format, ...);

// Logs an event to a stream at trace level.
void flog_trace(FILE* stream, const char* format, ...);

// Logs an event to standard output at debug level.
void log_debug(const char* format, ...);

// Logs an event to a stream at debug level.
void flog_debug(FILE* stream, const char* format, ...);

// Logs an event to standard output at information level.
void log_info(const char* format, ...);

// Logs an event to a stream at information level.
void flog_info(FILE* stream, const char* format, ...);

// Logs an event to standard output at warning level.
void log_warn(const char* format, ...);

// Logs an event to a stream at warning level.
void flog_warn(FILE* stream, const char* format, ...);

// Logs an event to standard output at error level.
void log_error(const char* format, ...);

// Logs an event to a stream at error level.
void flog_error(FILE* stream, const char* format, ...);

// Logs an event to standard output at fatal level.
void log_fatal(const char* format, ...);

// Logs an event to a stream at fatal level.
void flog_fatal(FILE* stream, const char* format, ...);

// Get the string representation of the logging level.
const char* lvltostr(const int level);