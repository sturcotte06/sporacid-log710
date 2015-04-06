#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#include "lib/logging.h"
#include "strategies.h"

/// <summary>
/// Allocates a block of memory into the pointer argument from the list of free blocks using the first fit strategy.
/// </summary>
/// <param name="free_block_list">The list of free block.</param>
/// <param name="pointer">The pointer into which to allocate.</param>
/// <returns>The state code.</returns>
int mem_allocation_strategy_first_fit (linked_list_t* free_block_list, ptr_t* pointer) {
	log_debug("Entering mem_allocation_strategy_first_fit().");
    log_debug("Exiting mem_allocation_strategy_first_fit().");
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Allocates a block of memory into the pointer argument from the list of free blocks using the best fit strategy.
/// </summary>
/// <param name="free_block_list">The list of free block.</param>
/// <param name="pointer">The pointer into which to allocate.</param>
/// <returns>The state code.</returns>
int mem_allocation_strategy_best_fit (linked_list_t* free_block_list, ptr_t* pointer) {
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
int mem_allocation_strategy_worst_fit (linked_list_t* free_block_list, ptr_t* pointer) {
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
int mem_allocation_strategy_next_fit (linked_list_t* free_block_list, ptr_t* pointer) {
	log_debug("Entering mem_allocation_strategy_next_fit().");
    log_debug("Exiting mem_allocation_strategy_next_fit().");
    return SUCCESSFUL_EXEC;
}