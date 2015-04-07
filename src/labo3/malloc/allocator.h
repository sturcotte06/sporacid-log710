#ifndef MALLOC_ALLOCATOR_H
#define MALLOC_ALLOCATOR_H

#include "../lib/collections.h"
#include "commons.h"
#include "strategies.h"

// Structure for the options of the allocator.
typedef struct allocator_options_t {
	mem_address_t address_space_first_address;
	sz_t address_space_size;
} allocator_options_t;

// The linked list of all free blocks.
extern linkedlist_t* free_block_list;

/// <summary>
/// Initializes the allocator.
/// </summary>
/// <param name="strategy">Function pointer for memory allocation strategy to use.</param>
/// <param name="options">Options for the allocator.</param>
/// <returns>The state code.</returns>
int init_allocator(mem_allocation_strategy_t strategy, allocator_options_t* options);

/// <summary>
/// Destroys the allocator.
/// </summary>
/// <returns>The state code.</returns>
int destroy_allocator();

/// <summary>
/// Allocates a memory block of at least size bytes.
/// The allocated memory location will be put into the pointer struct.
/// </summary>
/// <param name="size">The size of the allocation.</param>
/// <param name="pointer">The pointer into which to allocate.</param>
/// <returns>The state code.</returns>
int mem_allocate(sz_t* size, ptr_t* pointer);

/// <summary>
/// Frees a memory pointer and put the memory back into the allocator.
/// </summary>
/// <param name="pointer">The pointer from which to free.</param>
/// <returns>The state code.</returns>
int mem_free(ptr_t* pointer);

/// <summary>
/// Puts the number of allocated blocks into the count argument.
/// </summary>
/// <param name="count">The out argument for the count.</param>
/// <returns>The state code.</returns>
int mem_count_allocated_block(unsigned int* count);

/// <summary>
/// Puts the number of free blocks into the count argument.
/// </summary>
/// <param name="count">The out argument for the count.</param>
/// <returns>The state code.</returns>
int mem_count_free_block(unsigned int* count);

/// <summary>
/// Puts the number of free bytes into the count argument.
/// </summary>
/// <param name="count">The out argument for the count.</param>
/// <returns>The state code.</returns>
int mem_count_free(unsigned long* count);

/// <summary>
/// Puts the size of the greatest free block into the size argument.
/// </summary>
/// <param name="size">The out argument for the size.</param>
/// <returns>The state code.</returns>
int mem_greatest_free_block(sz_t* size);

/// <summary>
/// Puts the count of free blocks smaller than the given size into the count argument.
/// </summary>
/// <param name="size">The maximum size that can be considered small.</param>
/// <param name="count">The out argument for the count.</param>
/// <returns>The state code.</returns>
int mem_count_free_block_smaller_than(sz_t* size, unsigned int* count);

/// <summary>
/// Puts whether the given address is allocated into the flag argument.
/// </summary>
/// <param name="address">The address to check.</param>
/// <param name="flag">The out argument for whether it is allocated.</param>
/// <returns>The state code.</returns>
int mem_is_allocated(mem_address_t* address, unsigned int* flag);

/// <summary>
/// Merges adjacent nodes of the current node if pointer are contiguous.
/// </summary>
/// <param name="current">The current pointer node in the linked list.</param>
/// <returns>The state code.</returns>
int mem_merge_contiguous(unsigned int i_current, node_t* current);

#endif