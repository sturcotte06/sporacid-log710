#ifndef MALLOC_COMMONS_H
#define MALLOC_COMMONS_H

// Structure for the location of a memory address.
typedef unsigned long mem_adress_t;

// Structure for the size of a memory address.
typedef unsigned int sz_t;

// Structure for a memory pointer.
struct ptr_t {
    mem_adress_t adress;
    sz_t size;
} ptr_t;

#endif