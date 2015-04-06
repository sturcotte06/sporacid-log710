#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#include "../lib/collections.h"
#include "../lib/logging.h"
#include "allocator.h"

#define true 1
#define false 0

// The count of blocks still allocated.
unsigned int allocated_block_count;

// The current options applied.
allocator_options_t* allocation_options;

// The current memory allocation strategy applied.
mem_allocation_strategy_t allocation_strategy;

// The linked list of all free blocks.
linkedlist_t* free_block_list;

/// <summary>
/// Initializes the allocator.
/// </summary>
/// <param name="strategy">Function pointer for memory allocation strategy to use.</param>
/// <param name="options">Options for the allocator.</param>
/// <returns>The state code.</returns>
int init_allocator(mem_allocation_strategy_t strategy, allocator_options_t* options) {
	if (strategy == NULL || options == NULL) {
		return ILLEGAL_ARGUMENTS_ERRNO;
	}

	log_debug("Entering init_allocator(). First address: %lu, Adress space size: %u.", 
		options->address_space_first_address, options->address_space_size);

	// Initialize all globals.
	allocation_strategy = strategy;
	allocation_options = options;
	free_block_list = malloc(sizeof(linkedlist_t));
	if (linkedlist_init(free_block_list) != SUCCESSFUL_EXEC) {
		return COLLECTIONS_ERRNO;
	}

	// Create the initial block.
	ptr_t* initial_block = malloc(sizeof(ptr_t));
	initial_block->address = options->address_space_first_address;
	initial_block->size = options->address_space_size;
	if (linkedlist_add(free_block_list, 0, initial_block) != SUCCESSFUL_EXEC) {
		return COLLECTIONS_ERRNO;
	}
	
    log_debug("Exiting init_allocator().");
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Destroys the allocator.
/// </summary>
/// <returns>The state code.</returns>
int destroy_allocator() {
	log_debug("destroy_allocator init_allocator().");

	// Free all pointers.
	node_t* current = free_block_list->head;
	while (current != NULL) {
		free(current->element);
		current = current->next;
	}

	// Destroy all globals.
	allocation_strategy = NULL;
	linkedlist_destroy(free_block_list);
	free(free_block_list);

    log_debug("Exiting destroy_allocator().");
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Allocates a memory block of at least size bytes.
/// The allocated memory location will be put into the pointer struct.
/// </summary>
/// <param name="size">The size of the allocation.</param>
/// <param name="pointer">The pointer into which to allocate.</param>
/// <returns>The state code.</returns>
int mem_allocate(sz_t* size, ptr_t* pointer) {
    log_debug("Entering mem_allocate(). Size value: %u.", *size);
	if (size == NULL || pointer == NULL) {
		return ILLEGAL_ARGUMENTS_ERRNO;
	}

	// Call the allocation strategy.
	pointer->size = *size;
	int result = allocation_strategy(free_block_list, pointer);
	if (result == SUCCESSFUL_EXEC) {
		allocated_block_count++;
		log_debug("Exiting mem_allocate(). Address value: %lu.", pointer->address);
	}

    return result;
}

/// <summary>
/// Frees a memory pointer and put the memory back into the allocator.
/// </summary>
/// <param name="pointer">The pointer from which to free.</param>
/// <returns>The state code.</returns>
int mem_free(ptr_t* pointer) {
    log_debug("Entering mem_free(). Pointer address: %lu, Pointer size: %u.", pointer->address, pointer->size);
	if (pointer == NULL) {
		return ILLEGAL_ARGUMENTS_ERRNO;
	}

	// Find the node where to insert.
	unsigned int i_current = 0;
	ptr_t* next_pointer;
	node_t* next = free_block_list->head;
	while (next != NULL) {
		next_pointer = next->element;
		log_trace("next_pointer->address: %lu, pointer->address: %lu", next_pointer->address, pointer->address);

		if (next_pointer->address >= pointer->address) {
			break;
		}

		// Move to the next node.
		next = next->next;
		i_current++;
	}

	// Add the pointer to the linked list at the given position.
	linkedlist_add(free_block_list, i_current, pointer);

	// Merge contigous memory from the inserted node (which is the node before next node).
	mem_merge_contiguous(i_current, next->previous);
	
	// ptr_t* previous_pointer = current->previous != NULL ? current->previous->element : NULL;
	// log_trace("current_pointer: [%lu, %u]", current_pointer->address, current_pointer->size);
	// log_trace("previous_pointer: [%lu, %u]", 
	// 	previous_pointer != NULL ? previous_pointer->address : 0, 
	// 	previous_pointer != NULL ? previous_pointer->size : 0);
	// 
	// // Check if the pointer would contiguous with its previous pointer.
	// if (previous_pointer != NULL && (previous_pointer->address + previous_pointer->size == pointer->address)) {
	// 	// Contiguous: append size to the current pointer.
	// 	previous_pointer->size += pointer->size;
	// 
	// } 
	// // Check if the pointer would be contiguous with its next pointer.
	// if (pointer->address + pointer->size == current_pointer->address) {
	// 	current_pointer->address -= pointer->size;
	// 	current_pointer->size += pointer->size;
	// }	
	// // Non contiguous: insert pointer before current pointer.
	// else {
	// 	linkedlist_add(free_block_list, i_block, pointer);
	// }

	allocated_block_count--;
    log_debug("Exiting mem_free().");
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Puts the number of allocated blocks into the count argument.
/// </summary>
/// <param name="count">The out argument for the count.</param>
/// <returns>The state code.</returns>
int mem_count_allocated_block(unsigned int* count) {
    log_debug("Entering mem_count_allocated_block().");
	if (count == NULL) {
		return ILLEGAL_ARGUMENTS_ERRNO;
	}

	*count = allocated_block_count;
    log_debug("Exiting mem_count_allocated_block(). Count value: %u.", *count);
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Puts the number of free blocks into the count argument.
/// </summary>
/// <param name="count">The out argument for the count.</param>
/// <returns>The state code.</returns>
int mem_count_free_block(unsigned int* count) {
    log_debug("Entering mem_count_free_block().");
	if (count == NULL) {
		return ILLEGAL_ARGUMENTS_ERRNO;
	}

	*count = free_block_list->length;
    log_debug("Exiting mem_count_free_block(). Count value: %u.", *count);
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Puts the number of free bytes into the count argument.
/// </summary>
/// <param name="count">The out argument for the count.</param>
/// <returns>The state code.</returns>
int mem_count_free(unsigned long* count) {
    log_debug("Entering mem_count_free().");
	if (count == NULL) {
		return ILLEGAL_ARGUMENTS_ERRNO;
	}

	*count = 0;
	node_t* current = free_block_list->head;
	ptr_t* current_pointer;
	while (current != NULL) {
		current_pointer = current->element;
		*count += current_pointer->size;

		// Move to the next node.
		current = current->next;
	}

    log_debug("Exiting mem_count_free(). Count value: %lu.", *count);
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Puts the size of the greatest free block into the size argument.
/// </summary>
/// <param name="size">The out argument for the size.</param>
/// <returns>The state code.</returns>
int mem_greatest_free_block(sz_t* size) {
    log_debug("Entering mem_greatest_free_block().");
	if (size == NULL) {
		return ILLEGAL_ARGUMENTS_ERRNO;
	}

	*size = 0;
	node_t* current = free_block_list->head;
	ptr_t* current_pointer;
	while (current != NULL) {
		current_pointer = current->element;
		if (current_pointer->size > *size) {
			*size = current_pointer->size;
		}

		// Move to the next node.
		current = current->next;
	}

    log_debug("Exiting mem_greatest_free_block(). Size value: %u.", *size);
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Puts the count of free blocks smaller than the given size into the count argument.
/// </summary>
/// <param name="size">The maximum size that can be considered small.</param>
/// <param name="count">The out argument for the count.</param>
/// <returns>The state code.</returns>
int mem_count_free_block_smaller_than(sz_t* size, unsigned int* count) {
    log_debug("Entering mem_count_free_block_smaller_than(). Size value: %u.", *size);
	if (size == NULL || count == NULL) {
		return ILLEGAL_ARGUMENTS_ERRNO;
	}

	*count = 0;
	node_t* current = free_block_list->head;
	ptr_t* current_pointer;
	while (current != NULL) {
		current_pointer = current->element;
		if (current_pointer->size < *size) {
			*count = *count + 1;
		}

		// Move to the next node.
		current = current->next;
	}

    log_debug("Exiting mem_count_free_block_smaller_than(). Count value: %u.", *count);
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Puts whether the given memory pointer is allocated into the flag argument.
/// </summary>
/// <param name="pointer">The pointer to check.</param>
/// <param name="flag">The out argument for whether it is allocated.</param>
/// <returns>The state code.</returns>
int mem_is_allocated(ptr_t* pointer, unsigned int* flag) {
    log_debug("Entering mem_is_allocated(). Address value: %lu.", pointer->address);
    log_debug("Exiting mem_is_allocated(). Flag value: %s.", *flag ? "true" : "false");
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Merges adjacent nodes of the current node if pointer are contiguous.
/// </summary>
/// <param name="current">The current pointer node in the linked list.</param>
/// <returns>The state code.</returns>
int mem_merge_contiguous(unsigned int i_current, node_t* current) {
	log_debug("Entering mem_merge_contiguous().");
	
	node_t* previous = current->previous;
	node_t* next = current->next;
	ptr_t* previous_pointer = previous != NULL ? previous->element : NULL;
	ptr_t* current_pointer = current->element;
	ptr_t* next_pointer = next != NULL ? next->element : NULL;

	// Check if the current pointer would contiguous with its previous pointer.
	if (previous_pointer != NULL && (previous_pointer->address + previous_pointer->size == current_pointer->address)) {
		// Contiguous with previous node: append size to the previous pointer.
		previous_pointer->size += current_pointer->size;

		linkedlist_remove(free_block_list, i_current);
		mem_merge_contiguous(i_current - 1, previous);
	}
	// Check if the pointer would be contiguous with its next pointer.
	else if (next_pointer != NULL && (current_pointer->address + current_pointer->size == next_pointer->address)) {
		// Contiguous with next node: append size to the next pointer.
		next_pointer->address -= current_pointer->size;
		next_pointer->size += current_pointer->size;

		linkedlist_remove(free_block_list, i_current);
		mem_merge_contiguous(i_current + 1, next);
	}

    log_debug("Exiting mem_merge_contiguous().");
    return SUCCESSFUL_EXEC;
}