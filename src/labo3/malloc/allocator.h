#ifndef MALLOC_ALLOCATOR_H
#define MALLOC_ALLOCATOR_H

#include "malloc/commons.h"
#include "malloc/strategies.h"

// Constant for a successful execution.
extern const int SUCCESSFUL_EXEC;

// Constant for the error number for illegal arguments.
extern const int ILLEGAL_ARGUMENTS_ERRNO;

// The linked list of all free blocks.
extern linkedlist_t* free_block_list;

/// <summary>
/// Initializes the allocator.
/// </summary>
/// <param name="strategy">Function pointer for memory allocation strategy to use.</param>
/// <returns>The state code.</returns>
int init_allocator(mem_allocation_strategy_t* strategy);

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
/// Puts whether the given memory pointer is allocated into the flag argument.
/// </summary>
/// <param name="pointer">The pointer to check.</param>
/// <param name="flag">The out argument for whether it is allocated.</param>
/// <returns>The state code.</returns>
int mem_is_allocated(ptr_t* pointer, unsigned int* flag);

#endif