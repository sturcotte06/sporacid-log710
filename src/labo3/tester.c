#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <math.h>

#include "lib/logging.h"
#include "lib/collections.h"
#include "malloc/commons.h"
#include "malloc/strategies.h"
#include "malloc/allocator.h"
#include "tester.h"

#define true 1
#define false 0
#define LARGE_BUFFER_SIZE 4096
#define SMALL_BUFFER_SIZE 128
#define DEFAULT_SMALL_BLOCK_SIZE 64
#define DEFAULT_MAXIMUM_ALLOC 1000
#define DEFAULT_ALLOCATE_TO_FREE_RATIO 3

// Set the logging level to whatever we need for debugging purposes.
unsigned int log_level = INFO_LVL;

// The buffer size to use for logging.
const unsigned int LOG_BUFFER_SIZE = 5*LARGE_BUFFER_SIZE;

// Constant for a successful execution.
const int SUCCESSFUL_EXEC = 0;

// Error number for illegal arguments.
const int ILLEGAL_ARGUMENTS_ERRNO = 1;

// Error number for the impossibility to allocate memory.
const int OUT_OF_MEMORY_ERRNO = 2;

// Error number when trying to dequeue an empty queue.
const int EMPTY_QUEUE_ERRNO = 3;

// Error number when trying to access or modify an out of bound index.
const int OUT_OF_BOUNDS_ERRNO = 4;

// Error number when trying to modify a null linked list.
const int NULL_LINKED_LIST_ERRNO = 5;

// Error number when trying to modify a null queue.
const int NULL_QUEUE_ERRNO = 6;

// Error number for a generic error with collection handling.
const int COLLECTIONS_ERRNO = 7;

// The tester options activated currently.
tester_options_t tester_options;

/// <summary>
/// Starts the memory allocation tests.
/// </summary>
int main (int argc, char* argv[]) {
	unsigned int result, is_allocated_flag = false;
	srand(time(NULL));

	// Parse the arguments.
	result = parse_args(argc, argv, &tester_options);
	if (result != SUCCESSFUL_EXEC) {
		exit(result);
	}

	// Set default values if applicable.
	if (tester_options.verbose) log_level = TRACE_LVL;
	if (!tester_options.small_block_size) tester_options.small_block_size = DEFAULT_SMALL_BLOCK_SIZE;
	if (!tester_options.max_alloc_size) tester_options.max_alloc_size = DEFAULT_MAXIMUM_ALLOC;
	if (!tester_options.alloc_to_free_ratio) tester_options.alloc_to_free_ratio = DEFAULT_ALLOCATE_TO_FREE_RATIO;

	// Initialize the list into which we add alocated pointers.
	linkedlist_t allocated_pointer_list;
	linkedlist_init(&allocated_pointer_list);

	// Initialize the allocator.
	allocator_options_t allocator_options = { .address_space_first_address = tester_options.address_space_first_address, .address_space_size = tester_options.address_space_size };
	result = init_allocator(tester_options.allocation_strategy, &allocator_options);
	if (result != SUCCESSFUL_EXEC) {
		log_error("Allocator could not be initialized. init_allocator() returned %d.", result);
		exit(result);
	}
	
	// Log the initial state of the memory.
	log_mem_state(INFO_LVL);
	log_mem_parameters(INFO_LVL);

	// Allocate until first out of memory error.
	unsigned int is_oom = false, i_allocate = 0, j_allocate = 0;
	while (!is_oom) {
		// Allocate n pointers for one free.
		for (j_allocate = 0; j_allocate < tester_options.alloc_to_free_ratio; j_allocate++) {
			// Allocate one pointer of semi random size.
			int pointer_index = (i_allocate * tester_options.alloc_to_free_ratio) + j_allocate;
			ptr_t* pointer = malloc(sizeof(ptr_t));
			sz_t size = ((pointer_index + 43) * 4373 / 63 * 21) % (tester_options.max_alloc_size - 1) + 1;

			// Allocate it and act on result.
			result = mem_allocate(&size, pointer);
			if (result == OUT_OF_MEMORY_ERRNO) {
				log_error("Memory could not be allocated because the allocator is out of memory.", result);
				is_oom = true;
				free(pointer);
				break;
			} else if (result != SUCCESSFUL_EXEC) {
				log_error("Memory could not be allocated. mem_allocate() returned %d.", result);
				exit(result);
			} else {
				linkedlist_add(&allocated_pointer_list, 0, pointer);
                
                // Make sure the memory was allocated.
				mem_is_allocated(&pointer->address, &is_allocated_flag);
				if (is_allocated_flag) log_info("Memory was allocated: [%lu, %u]", pointer->address, pointer->size);
                else log_warn("Memory was allocated by mem_allocate() ([%lu, %u]) but flagged as unallocated by mem_is_allocated(). Might be a bug.", 
                    pointer->address, pointer->size);
                    
                // Log the state of the memory after allocation.
				log_mem_state(INFO_LVL);
				log_mem_parameters(INFO_LVL);
			}
		}

		// Get some random pointer from allocated pointers.
		void* element;
		int random_index = rand() % allocated_pointer_list.length;
		linkedlist_get(&allocated_pointer_list, random_index, &element);

		ptr_t* random_pointer = element;
		log_info("Random index: %d, Random pointer: [%lu, %u].", random_index, random_pointer->address, random_pointer->size);
		linkedlist_remove(&allocated_pointer_list, random_index);

        // Free the random pointer to create some fragmentation.
		int result = mem_free(random_pointer);
		if (result != SUCCESSFUL_EXEC) {
			log_error("Memory could not be freed. mem_free() returned %d.", result);
		} else {
            // Make sure the memory was deallocated.
			mem_is_allocated(&random_pointer->address, &is_allocated_flag);
			if (!is_allocated_flag) log_info("Memory was freed");
            else log_warn("Memory was freed by mem_free() but flagged as allocated by mem_is_allocated(). Might be a bug.");
		}

        // Log the state of the memory after deallocation.
		log_mem_state(INFO_LVL);
		log_mem_parameters(INFO_LVL);
		free(random_pointer);
        
		i_allocate++;
	}

	// Deallocate all currently allocated pointers.
	node_t* current = allocated_pointer_list.head;
	while (current != NULL) {
		ptr_t* current_pointer = current->element;
        
        // Try to deallocate the pointer.
		result = mem_free(current_pointer);
		if (result != SUCCESSFUL_EXEC) {
			log_error("Memory could not be freed. mem_free() returned %d.", result);
		} else {
            // Make sure the memory was deallocated.
			mem_is_allocated(&current_pointer->address, &is_allocated_flag);
			if (!is_allocated_flag) log_info("Memory was freed");
			else log_warn("Memory was freed by mem_free() but flagged as allocated by mem_is_allocated(). Might be a bug.");

            // Log the state of the memory after deallocation.
			log_mem_state(INFO_LVL);
			log_mem_parameters(INFO_LVL);
		}

		free(current_pointer);
		current = current->next;
	}
	
	linkedlist_destroy(&allocated_pointer_list);
	destroy_allocator();
	exit(SUCCESSFUL_EXEC);
}

/// <summary>
/// Parse the command line arguments into an options structure.
/// </summary>
/// <param name="argc">The number of arguments in argv.</param>
/// <param name="argv">The argument vector.</param>
/// <param name="options">The options to parse into.</param>
/// <returns>The state code.</returns>
int parse_args(const int argc, char* argv[], tester_options_t* options) {
	log_debug("Entering parse_args().");

	char help_buffer[LARGE_BUFFER_SIZE];
	memset(help_buffer, 0, LARGE_BUFFER_SIZE);

	if (options == NULL || argv == NULL) {
		sprint_help(help_buffer);
		log_fatal(help_buffer);
		return ILLEGAL_ARGUMENTS_ERRNO;
	}

	if (argc < 5) {
		sprint_help(help_buffer);
		log_fatal(help_buffer);
		return ILLEGAL_ARGUMENTS_ERRNO;
	}
	
	int i_arg = 1;
	int first_address_set = false, size_set = false, strategy_set = false;
	while (i_arg < argc) {
		char* option_name = argv[i_arg];
		if (option_name[0] == '-') {
			if (option_name[1] == '-') {
                // Flag options handlers.
				if (strcmp(option_name, "--verbose") == 0) {
					options->verbose = true;
				}

                i_arg++;
            } else {
				// Normal options handlers.
				char* option_value = argv[i_arg + 1];
				if (strcmp(option_name, "-first-address") == 0) {
					options->address_space_first_address = atoi(option_value);
					first_address_set = true;
				} else if (strcmp(option_name, "-size") == 0) {
					options->address_space_size = atoi(option_value);
					size_set = true;
				} else if (strcmp(option_name, "-small-block-size") == 0) {
					options->small_block_size = atoi(option_value);
				} else if (strcmp(option_name, "-alloc-to-free-ratio") == 0) {
					options->alloc_to_free_ratio = atoi(option_value);
				} else if (strcmp(option_name, "-max-allocation") == 0) {
					options->max_alloc_size = atoi(option_value);
				} else if (strcmp(option_name, "-strategy") == 0) {
					if (strcmp(option_value, "first") == 0) {
						options->allocation_strategy = &mem_allocation_strategy_first_fit;
						strategy_set = true;
					} else if (strcmp(option_value, "best") == 0) {
						options->allocation_strategy = &mem_allocation_strategy_best_fit;
						strategy_set = true;
					} else if (strcmp(option_value, "worst") == 0) {
						options->allocation_strategy = &mem_allocation_strategy_worst_fit;
						strategy_set = true;
					} else if (strcmp(option_value, "next") == 0) {
						options->allocation_strategy = &mem_allocation_strategy_next_fit;
						strategy_set = true;
					} else {
						sprint_help(help_buffer);
						log_fatal(help_buffer);
						return ILLEGAL_ARGUMENTS_ERRNO;
					}
				}

				i_arg += 2;
			}
		} else {
			// Option scheme not recognized, skip option.
			log_warn("Option %s is not recognized.", option_name);
			i_arg++;
		}
	}

	if (!first_address_set) {
		options->address_space_first_address = 0;
	}

	if (!size_set) {
		log_fatal("-size option is required.");
		sprint_help(help_buffer);
		log_fatal(help_buffer);
		return ILLEGAL_ARGUMENTS_ERRNO;
	}

	if (!strategy_set) {
		log_fatal("-strategy option is required.");
		sprint_help(help_buffer);
		log_fatal(help_buffer);
		return ILLEGAL_ARGUMENTS_ERRNO;
	}

	log_debug("Exiting parse_args().");
	return SUCCESSFUL_EXEC;
}

/// <summary>
/// Prints the help in the given buffer.
/// </summary>
/// <param name="buffer">The buffer to use.</param>
/// <returns>The state code.</returns>
int sprint_help(char* buffer) {
	strcat(buffer, "\n\tRequired Arguments\n");
	strcat(buffer, "\t  -size {int > 0} The address space size.\n");
	strcat(buffer, "\t  -strategy {string} The strategy to use. Values are first, best, worst, next.\n");
	strcat(buffer, "\tOptional Arguments\n");
	strcat(buffer, "\t  -first-address {int} The address space initial value.\n");
	strcat(buffer, "\t  -small-block-size {int > 0} The size of what is considered a small block.\n");
	strcat(buffer, "\t  -alloc-to-free-ratio {int > 0} How many allocation for a single free for the test.\n");
	strcat(buffer, "\t  -max-allocation {int > 0} The maximum allocation for the test.\n");
	strcat(buffer, "\t  --verbose {flag} Whether to log everything.\n");
	return SUCCESSFUL_EXEC;
}

/// <summary>
/// Logs the current state of the memory.
/// </summary>
/// <param name="level">The logging level to use.</param>
/// <returns>The state code.</returns>
int log_mem_state(const int level) {
	log_debug("Entering log_mem_state()."); 
	char mem_block_buffer[SMALL_BUFFER_SIZE];
	char mem_state_buffer[LARGE_BUFFER_SIZE];
	memset(&mem_state_buffer, 0, LARGE_BUFFER_SIZE);

	node_t* current = free_block_list->head;
	ptr_t* current_pointer;
	while (current != NULL) {
		current_pointer = current->element;

		// Concatenate state of current block.
		sprintf(mem_block_buffer, "[%lu, %u] -> ", current_pointer->address, current_pointer->size);
		strncat(mem_state_buffer, mem_block_buffer, LARGE_BUFFER_SIZE - strlen(mem_state_buffer));

		// Move to the next node.
		current = current->next;
	}

	log_format(level, "Memory state: %s", mem_state_buffer);
    log_debug("Exiting log_mem_state().");
	return SUCCESSFUL_EXEC;
}

/// <summary>
/// Logs the current state of the memory.
/// </summary>
/// <param name="level">The logging level to use.</param>
/// <returns>The state code.</returns>
int log_mem_parameters(const int level) {
	log_debug("Entering log_mem_parameters().");
	char mem_parameter_buffer[SMALL_BUFFER_SIZE];
	char mem_parameters_buffer[LARGE_BUFFER_SIZE];
	memset(&mem_parameters_buffer, 0, LARGE_BUFFER_SIZE);

	unsigned int allocated_blocks;
	mem_count_allocated_block(&allocated_blocks);
	sprintf(mem_parameter_buffer, "\tAllocated Blocks: %u\n", allocated_blocks);
	strcat(mem_parameters_buffer, mem_parameter_buffer);

	unsigned int free_blocks;
	mem_count_free_block(&free_blocks);
	sprintf(mem_parameter_buffer, "\tFree Blocks: %u\n", free_blocks);
	strcat(mem_parameters_buffer, mem_parameter_buffer);

	unsigned long free_memory;
	mem_count_free(&free_memory);
	sprintf(mem_parameter_buffer, "\tFree memory: %lu\n", free_memory);
	strcat(mem_parameters_buffer, mem_parameter_buffer);

	sz_t greatest_block;
	mem_greatest_free_block(&greatest_block);
	sprintf(mem_parameter_buffer, "\tGreatest block: %u\n", greatest_block);
	strcat(mem_parameters_buffer, mem_parameter_buffer);

	unsigned int small_blocks;
	mem_count_free_block_smaller_than(&tester_options.small_block_size, &small_blocks);
	sprintf(mem_parameter_buffer, "\tBlock smaller than %d: %u", tester_options.small_block_size, small_blocks);
	strcat(mem_parameters_buffer, mem_parameter_buffer);

	log_format(level, "Memory parameters: \n%s", mem_parameters_buffer);
	log_debug("Exiting log_mem_parameters().");
	return SUCCESSFUL_EXEC;
}