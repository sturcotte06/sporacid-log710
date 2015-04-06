#ifndef TESTER_H
#define TESTER_H

// Constant for a successful execution.
extern const int SUCCESSFUL_EXEC;

// Constant for the error number for illegal arguments.
extern const int ILLEGAL_ARGUMENTS_ERRNO;

/// <summary>
/// Logs the current state of the memory.
/// </summary>
/// <param name="level">The logging level to use.</param>
/// <returns>The state code.</returns>
int log_mem_state(const int level);

/// <summary>
/// Logs the current state of the memory.
/// </summary>
/// <param name="level">The logging level to use.</param>
/// <returns>The state code.</returns>
int log_mem_parameters(const int level);

#endif