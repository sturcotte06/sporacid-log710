// Constants for logging levels.
extern const char* ERROR_LVL;
extern const char* WARN_LVL;
extern const char* INFO_LVL;
extern const char* DEBUG_LVL;

// Logs an event to standard output.
void logft(const char* level, const char* format, ...);

// Logs an event to a stream.
void flogft(FILE* stream, const char* level, const char* format, ...);