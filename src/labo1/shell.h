#define false 0
#define true 1

// Prompt for this shell.
char* PROMPT = "$ %s ";

// Default directory if cd command is used parameterless.
const char* DEFAULT_DIRECTORY = "/";

// Default directory if cd command is used parameterless.
const char* RUN_COMMAND_LOCATION = "/export/LOG710/home/AJ50440/labo1/run";

const int UNKNOWN_ERROR_ERRNO = -1;

// Constant for a successful execution.
const int SUCCESSFUL_EXEC = 0;

// Constant for the error number for illegal arguments.
const int ILLEGAL_ARGUMENTS_ERRNO = 1;

// Constant for the error number when exec() family functions fail.
const int COMMAND_EXEC_FAIL_ERRNO = 2;

// Constant for the error number if a required exe does not exist.
const int EXEC_UNEXISTING_ERRNO = 3;

// Constant for the error number if a required directory does not exist.
const int DIRECTORY_UNEXISTING_ERRNO = 4;

// Constant for the error number if standard input read cannot occur.
const int STDIN_ERROR_ERRNO = 5;

// Constant for the error number if standard output write cannot occur.
const int STDOUT_ERROR_ERRNO = 6;

// Default buffer size. If input exceeds, expect the program to return an error code.
const int BUFF_SIZE = 1024;

// Splits a command into argc and argv.
void splitCommand(char *command, int* argc, char** argv);

// Change current directory of the shell.
int changeDirectory(const char* path);