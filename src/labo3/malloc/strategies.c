#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#include "../lib/collections.h"
#include "../lib/logging.h"
#include "strategies.h"

#define true 1
#define false 0

// Current node in the next fit algorithm.
node_t* next_fit_current;

// Current index of current node in next fit algorithm.
int i_next_fit_current;

/// <summary>
/// Allocates a block of memory into the pointer argument from the list of free blocks using the first fit strategy.
/// </summary>
/// <param name="free_block_list">The list of free block.</param>
/// <param name="pointer">The pointer into which to allocate.</param>
/// <returns>The state code.</returns>
int mem_allocation_strategy_first_fit (linkedlist_t* free_block_list, ptr_t* pointer) {
	if (free_block_list == NULL || pointer == NULL) {
		return ILLEGAL_ARGUMENTS_ERRNO;
	}

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
		current_pointer->size -= pointer->size;
	} else {
		// Size matched perfectly. Remove the block from the free block and return it.
		pointer->address = current_pointer->address;
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
	if (free_block_list == NULL || pointer == NULL) {
		return ILLEGAL_ARGUMENTS_ERRNO;
	}

	log_debug("Entering mem_allocation_strategy_best_fit().");

	int i_current = 0, i_best_fit = 0;
	node_t* current = free_block_list->head;
	ptr_t* current_pointer;
	ptr_t* best_fit_pointer;
	while (current != NULL) {
		current_pointer = current->element;
		if ((best_fit_pointer == NULL && pointer->size <= current_pointer->size) || 
			(best_fit_pointer != NULL && pointer->size <= current_pointer->size && (current_pointer->size - pointer->size) < (best_fit_pointer->size - pointer->size))) {
			// Better fit than previous best fit.
			best_fit_pointer = current_pointer;
			i_best_fit = i_current;
		}

		// Move to the next node.
		current = current->next;
		i_current++;
	}
	
	if (best_fit_pointer == NULL) {
		return OUT_OF_MEMORY_ERRNO;
	}

	// If the found block has more memory than required, split it.
	if (best_fit_pointer->size > pointer->size) {
		pointer->address = best_fit_pointer->address;
		best_fit_pointer->address += pointer->size;
		best_fit_pointer->size -= pointer->size;
	} else {
		// Size matched perfectly. Remove the block from the free block and return it.
		pointer->address = best_fit_pointer->address;
		linkedlist_remove(free_block_list, i_best_fit);
		free(best_fit_pointer);
	}

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
	if (free_block_list == NULL || pointer == NULL) {
		return ILLEGAL_ARGUMENTS_ERRNO;
	}

	log_debug("Entering mem_allocation_strategy_worst_fit().");

	int i_current = 0, i_worst_fit = 0;
	node_t* current = free_block_list->head;
	ptr_t* current_pointer;
	ptr_t* worst_fit_pointer;
	while (current != NULL) {
		current_pointer = current->element;
		if ((worst_fit_pointer == NULL && pointer->size <= current_pointer->size) ||
			(worst_fit_pointer != NULL && pointer->size <= current_pointer->size && (current_pointer->size - pointer->size) > (worst_fit_pointer->size - pointer->size))) {
			// Worst fit than previous worst fit.
			worst_fit_pointer = current_pointer;
			i_worst_fit = i_current;
		}

		// Move to the next node.
		current = current->next;
		i_current++;
	}
	
	if (worst_fit_pointer == NULL) {
		return OUT_OF_MEMORY_ERRNO;
	}

	// If the found block has more memory than required, split it.
	if (worst_fit_pointer->size > pointer->size) {
		pointer->address = worst_fit_pointer->address;
		worst_fit_pointer->address += pointer->size;
		worst_fit_pointer->size -= pointer->size;
	} else {
		// Size matched perfectly. Remove the block from the free block and return it.
		pointer->address = worst_fit_pointer->address;
		linkedlist_remove(free_block_list, i_worst_fit);
		free(worst_fit_pointer);
	}

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
	if (free_block_list == NULL || pointer == NULL) {
		return ILLEGAL_ARGUMENTS_ERRNO;
	}
	
	log_debug("Entering mem_allocation_strategy_next_fit().");
	
	// Initialize next fit global variables.
	if (next_fit_current == NULL) {
		next_fit_current = free_block_list->head;
		i_next_fit_current = 0;
	}

	int i_current = i_next_fit_current, next_fit_found = false;
	node_t* current = next_fit_current;
	ptr_t* current_pointer;
	do {
		current_pointer = current->element;
		if (pointer->size < current_pointer->size) {
			next_fit_current = current;
			i_next_fit_current = i_current;
			next_fit_found = true;
			break;
		}
	
		if (current->next == NULL) {
			current = free_block_list->head;
			i_current = 0;
		} else {
			current = current->next;
			i_current++;
		}
	} while (i_current != i_next_fit_current);
		
	if (!next_fit_found) {
		return OUT_OF_MEMORY_ERRNO;
	}

	// int i_current = i_next_fit_current + 1;
	// node_t* current = next_fit_current->next;
	// ptr_t* current_pointer;
	// while (i_current != i_next_fit_current) {
	// 	if (current == NULL) {
	// 
	// 	}
	// }


	// If the found block has more memory than required, split it.
	log_info("pointer: [%lu, %u]", pointer->address, pointer->size);
	log_info("current_pointer: [%lu, %u]", current_pointer->address, current_pointer->size);

	ptr_t* next_fit_current_pointer = next_fit_current->element;
	log_info("next_fit_current_pointer: [%lu, %u]", next_fit_current_pointer->address, next_fit_current_pointer->size);
	if (current_pointer->size > pointer->size) {
		pointer->address = current_pointer->address;
		current_pointer->address += pointer->size;
		current_pointer->size -= pointer->size;
	} else {
		// Size matched perfectly. Remove the block from the free block and return it.
		pointer->address = current_pointer->address;
		linkedlist_remove(free_block_list, i_current);

		next_fit_current = NULL;
		free(current_pointer);
	}

    log_debug("Exiting mem_allocation_strategy_next_fit().");
    return SUCCESSFUL_EXEC;
}