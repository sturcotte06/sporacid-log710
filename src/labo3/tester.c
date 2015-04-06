#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#include "lib/logging.h"
#include "lib/collections.h"
#include "malloc/strategies.h"
#include "malloc/allocator.h"
#include "tester.h"

// Set the logging level to whatever we need for debugging purposes.
unsigned int loglevel = DEBUG_LVL;

// Constant for a successful execution.
const int SUCCESSFUL_EXEC = 0;

// Constant for the error number for illegal arguments.
const int ILLEGAL_ARGUMENTS_ERRNO = 1;

/// <summary>
/// Starts the memory allocation tests.
/// </summary>
void main (void) {
	exit(SUCCESSFUL_EXEC);
}

/// <summary>
/// Logs the current state of the memory.
/// </summary>
/// <param name="level">The logging level to use.</param>
/// <returns>The state code.</returns>
int log_mem_state(const int level) {
	log_format(level, "");
	return SUCCESSFUL_EXEC;
}

/// <summary>
/// Logs the current state of the memory.
/// </summary>
/// <param name="level">The logging level to use.</param>
/// <returns>The state code.</returns>
int log_mem_parameters(const int level) {
	log_format(level, "");
	return SUCCESSFUL_EXEC;
}