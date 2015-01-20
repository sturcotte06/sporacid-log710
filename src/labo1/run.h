#define false 0
#define true 1

// Constant for a successful execution.
const int SUCCESSFUL_EXEC = 0;

// Constant for the error number for illegal arguments.
const int ILLEGAL_ARGUMENTS_ERRNO = 1;

// Constant for the error number when exec() family functions fail.
const int COMMAND_EXEC_FAIL_ERRNO = 2;

// Get milliseconds from a timeval struct.
long getMilliseconds(struct timeval time);