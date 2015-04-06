#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#include "lib/logging.h"
#include "lib/collections.h"
#include "malloc/commons.h"
#include "malloc/strategies.h"
#include "malloc/allocator.h"
#include "tester.h"

#define LARGE_BUFFER_SIZE 1024
#define SMALL_BUFFER_SIZE 128
#define SMALL_BLOCK 64

// Set the logging level to whatever we need for debugging purposes.
unsigned int loglevel = INFO_LVL;

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

/// <summary>
/// Starts the memory allocation tests.
/// </summary>
int main (void) {
	int result;
	allocator_options_t options = { .address_space_first_address = 0, .address_space_size = 5000 };
	result = init_allocator(&mem_allocation_strategy_first_fit, &options);
	if (result != SUCCESSFUL_EXEC) {
		log_error("Allocater could not be initialized. init_allocator() returned %d.", result);
		exit(result);
	}

	// Log the initial state of the memory.
	log_mem_state(INFO_LVL);
	log_mem_parameters(INFO_LVL);

	// Allocate until first out of memory error.
	ptr_t pointer_buffer[LARGE_BUFFER_SIZE];
	int i;
	for (i = 0;; i++) {
		ptr_t pointer;
		sz_t size = (((i + 43) * 4373 / 63 * 21) % 999) + 1;

		int result = mem_allocate(&size, &pointer);
		if (result != SUCCESSFUL_EXEC) {
			log_error("Memory could not be allocated. mem_allocate() returned %d.", result);
			break;
		}

		pointer_buffer[i] = pointer;
		log_info("Memory allocated: [%lu, %u]", pointer.address, pointer.size);
		log_mem_state(INFO_LVL);
		log_mem_parameters(INFO_LVL);
	}

	loglevel = TRACE_LVL;
	// Deallocate evrything.
	int j;
	while (j < i) {
		int result = mem_free(&pointer_buffer[j]);
		if (result != SUCCESSFUL_EXEC) {
			log_error("Memory could not be freed. mem_free() returned %d.", result);
		}

		log_info("Memory freed.");
		log_mem_state(INFO_LVL);
		log_mem_parameters(INFO_LVL);
		j++;
	}

	// Log the current memory parameters.
	log_mem_parameters(INFO_LVL);

	exit(SUCCESSFUL_EXEC);
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
		strcat(mem_state_buffer, mem_block_buffer);

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

	sz_t small_block_size = SMALL_BLOCK;
	unsigned int small_blocks;
	mem_count_free_block_smaller_than(&small_block_size, &small_blocks);
	sprintf(mem_parameter_buffer, "\tBlock smaller than %d: %u", SMALL_BLOCK, small_blocks);
	strcat(mem_parameters_buffer, mem_parameter_buffer);

	log_format(level, "Memory parameters: \n%s", mem_parameters_buffer);
	log_debug("Exiting log_mem_parameters().");
	return SUCCESSFUL_EXEC;
}