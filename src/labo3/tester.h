#ifndef TESTER_H
#define TESTER_H

// Structure for the options of the tester.
typedef struct tester_options_t {
	mem_allocation_strategy_t allocation_strategy;
	mem_address_t address_space_first_address;
	sz_t address_space_size;
	sz_t small_block_size;
	sz_t max_alloc_size;
	unsigned int alloc_to_free_ratio;
	unsigned int verbose;
} tester_options_t;

/// <summary>
/// Allocates n pointers (defined by alloc_to_free_ratio option) then deallocates one random pointer until an out of memory error happens.
/// </summary>
/// <param name="allocated_pointer_list">The linked list of currently allocated pointers.</param>
/// <returns>The state code.</returns>
int test_allocate_until_out_of_mem(linkedlist_t* allocated_pointer_list);

/// <summary>
/// Deallocates one random pointer within the given allocated pointer list.
/// </summary>
/// <param name="allocated_pointer_list">The linked list of currently allocated pointers.</param>
/// <returns>The state code.</returns>
int test_deallocate_random_pointer(linkedlist_t* allocated_pointer_list);

/// <summary>
/// Deallocates all pointers within the given allocated pointer list.
/// </summary>
/// <param name="allocated_pointer_list">The linked list of currently allocated pointers.</param>
/// <returns>The state code.</returns>
int test_deallocate_all(linkedlist_t* allocated_pointer_list);

/// <summary>
/// Parse the command line arguments into an options structure.
/// </summary>
/// <param name="argc">The number of arguments in argv.</param>
/// <param name="argv">The argument vector.</param>
/// <param name="options">The options to parse into.</param>
/// <returns>The state code.</returns>
int parse_args(const int argc, char* argv[], tester_options_t* options);

/// <summary>
/// Prints the help in the given buffer.
/// </summary>
/// <param name="buffer">The buffer to use.</param>
/// <returns>The state code.</returns>
int sprint_help(char* buffer);

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