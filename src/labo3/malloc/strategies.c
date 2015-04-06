#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#include "../lib/collections.h"
#include "../lib/logging.h"
#include "strategies.h"

/// <summary>
/// Allocates a block of memory into the pointer argument from the list of free blocks using the first fit strategy.
/// </summary>
/// <param name="free_block_list">The list of free block.</param>
/// <param name="pointer">The pointer into which to allocate.</param>
/// <returns>The state code.</returns>
int mem_allocation_strategy_first_fit (linkedlist_t* free_block_list, ptr_t* pointer) {
	log_debug("Entering mem_allocation_strategy_first_fit().");

	int i_current = 0;
	node_t* current = free_block_list->head;
	ptr_t* current_pointer;
	while (current != NULL) {
		current_pointer = current->element;
		if (pointer->size < current_pointer->size) {
			break;
		}

		// Move to the next node.
		current = current->next;
		i_current++;
	}

	if (current == NULL) {
		return OUT_OF_MEMORY_ERRNO;
	}

	// If the found block has more memory than required, split it.
	if (current_pointer->size > pointer->size) {
		pointer->address = current_pointer->address;
		current_pointer->address += pointer->size;
		current_pointer-> size -= pointer->size;
	} else {
		// Size matched perfectly. Remove the block from the free block and return it.
		*pointer = *current_pointer;
		linkedlist_remove(free_block_list, i_current);
		free(current_pointer);
	}

    log_debug("Exiting mem_allocation_strategy_first_fit().");
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Allocates a block of memory into the pointer argument from the list of free blocks using the best fit strategy.
/// </summary>
/// <param name="free_block_list">The list of free block.</param>
/// <param name="pointer">The pointer into which to allocate.</param>
/// <returns>The state code.</returns>
int mem_allocation_strategy_best_fit (linkedlist_t* free_block_list, ptr_t* pointer) {
	log_debug("Entering mem_allocation_strategy_best_fit().");
    log_debug("Exiting mem_allocation_strategy_best_fit().");
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Allocates a block of memory into the pointer argument from the list of free blocks using the worst fit strategy.
/// </summary>
/// <param name="free_block_list">The list of free block.</param>
/// <param name="pointer">The pointer into which to allocate.</param>
/// <returns>The state code.</returns>
int mem_allocation_strategy_worst_fit (linkedlist_t* free_block_list, ptr_t* pointer) {
	log_debug("Entering mem_allocation_strategy_worst_fit().");
    log_debug("Exiting mem_allocation_strategy_worst_fit().");
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Allocates a block of memory into the pointer argument from the list of free blocks using the next fit strategy.
/// </summary>
/// <param name="free_block_list">The list of free block.</param>
/// <param name="pointer">The pointer into which to allocate.</param>
/// <returns>The state code.</returns>
int mem_allocation_strategy_next_fit (linkedlist_t* free_block_list, ptr_t* pointer) {
	log_debug("Entering mem_allocation_strategy_next_fit().");
    log_debug("Exiting mem_allocation_strategy_next_fit().");
    return SUCCESSFUL_EXEC;
}