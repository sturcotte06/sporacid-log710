#ifndef MALLOC_STRATEGIES_H
#define MALLOC_STRATEGIES_H

#include "../lib/collections.h"
#include "commons.h"

// Function pointer for a memory allocation strategy.
typedef int (*mem_allocation_strategy_t)(linkedlist_t* free_block_list, ptr_t* pointer);

/// <summary>
/// Allocates a block of memory into the pointer argument from the list of free blocks using the first fit strategy.
/// </summary>
/// <param name="free_block_list">The list of free block.</param>
/// <param name="pointer">The pointer into which to allocate.</param>
/// <returns>The state code.</returns>
int mem_allocation_strategy_first_fit (linkedlist_t* free_block_list, ptr_t* pointer);

/// <summary>
/// Allocates a block of memory into the pointer argument from the list of free blocks using the best fit strategy.
/// </summary>
/// <param name="free_block_list">The list of free block.</param>
/// <param name="pointer">The pointer into which to allocate.</param>
/// <returns>The state code.</returns>
int mem_allocation_strategy_best_fit (linkedlist_t* free_block_list, ptr_t* pointer);

/// <summary>
/// Allocates a block of memory into the pointer argument from the list of free blocks using the worst fit strategy.
/// </summary>
/// <param name="free_block_list">The list of free block.</param>
/// <param name="pointer">The pointer into which to allocate.</param>
/// <returns>The state code.</returns>
int mem_allocation_strategy_worst_fit (linkedlist_t* free_block_list, ptr_t* pointer);

/// <summary>
/// Allocates a block of memory into the pointer argument from the list of free blocks using the next fit strategy.
/// </summary>
/// <param name="free_block_list">The list of free block.</param>
/// <param name="pointer">The pointer into which to allocate.</param>
/// <returns>The state code.</returns>
int mem_allocation_strategy_next_fit (linkedlist_t* free_block_list, ptr_t* pointer);

#endif