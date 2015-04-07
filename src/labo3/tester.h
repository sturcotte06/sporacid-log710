#ifndef TESTER_H
#define TESTER_H

// Structure for the options of the tester.
typedef struct tester_options_t {
	mem_address_t address_space_first_address;
	sz_t address_space_size;
	mem_allocation_strategy_t allocation_strategy;
} tester_options_t;

/// <summary>
/// Parse the command line arguments into an options structure.
/// </summary>
/// <param name="argc">The number of arguments in argv.</param>
/// <param name="argv">The argument vector.</param>
/// <param name="options">The options to parse into.</param>
/// <returns>The state code.</returns>
int parse_args(const int argc, char* argv[], tester_options_t* options);

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