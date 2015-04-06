#ifndef MALLOC_COMMONS_H
#define MALLOC_COMMONS_H

// Constant for a successful execution.
extern const int SUCCESSFUL_EXEC;

// Error number for illegal arguments.
extern const int ILLEGAL_ARGUMENTS_ERRNO;

// Error number for a generic error with collection handling.
extern const int COLLECTIONS_ERRNO;

// Error number for the impossibility to allocate memory.
extern const int OUT_OF_MEMORY_ERRNO;

// Structure for the location of a memory address.
typedef unsigned long mem_address_t;

// Structure for the size of a memory address.
typedef unsigned int sz_t;

// Structure for a memory pointer.
typedef struct ptr_t {
    mem_address_t address;
    sz_t size;
} ptr_t;

#endif