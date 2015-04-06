#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#include "lib/logging.h"
#include "allocator.h"

#define true 1
#define false 0

/// <summary>
/// Initializes the allocator.
/// </summary>
/// <returns>The state code.</returns>
int init_allocator() {
    log_debug("Entering init_allocator().");
    log_debug("Exiting init_allocator().");
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
    log_debug("Exiting mem_allocate(). Address value: %lu.", pointer->address);
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Frees a memory pointer and put the memory back into the allocator.
/// </summary>
/// <param name="pointer">The pointer from which to free.</param>
/// <returns>The state code.</returns>
int mem_free(ptr_t* pointer) {
    log_debug("Entering mem_free(). Pointer address: %lu.", pointer->address);
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