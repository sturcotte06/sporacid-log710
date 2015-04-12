#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../logging/logging.h"
#include "../../tests/tests.h"
#include "../linkedlist.h"
#include "linkedlist_tests.h"

#define TEST_SIZE 1053
#define BUFFER_SIZE 256

unsigned int log_level = INFO_LVL;
const unsigned int LOG_BUFFER_SIZE = 1024;

int main(void) {
    // Tests linked list.
    testresults_t results;
    linkedlist_t linkedlist;
    tests_start(&results, stdout);
    test_linkedlist_init(&results, &linkedlist);
    test_linkedlist_addall(&results, &linkedlist);
    test_linkedlist_print(&linkedlist);
    test_linkedlist_reverse_print(&linkedlist);
    test_linkedlist_getall(&results, &linkedlist);
    test_linkedlist_addone(&results, &linkedlist, 2);
    test_linkedlist_addone(&results, &linkedlist, 8);
    test_linkedlist_removeone(&results, &linkedlist, 3);
    test_linkedlist_removeone(&results, &linkedlist, 7);
    test_linkedlist_removeall(&results, &linkedlist);
    test_linkedlist_destroy(&results, &linkedlist);
    tests_end(&results);
    exit(0);
}

void test_linkedlist_init(testresults_t* results, linkedlist_t* linkedlist) {
    tests_assert(results, 
        linkedlist_init(linkedlist) == linkedlist_success,
        "test_linkedlist_init(): Initialization of linked list returned wrong value.");
}

void test_linkedlist_addall(testresults_t* results, linkedlist_t* linkedlist) {    
    int i; for (i = 0; i < TEST_SIZE; i++) {
        char* element = malloc(BUFFER_SIZE * sizeof(char));
        sprintf(element, "Element %d", i);
    
        tests_assert(results, 
            linkedlist_add(linkedlist, i, element) == linkedlist_success,
            "test_linkedlist_addall(): Queuing of element \"%s\" returned the wrong value.", element);
    }
    
    tests_assert(results, 
        linkedlist->length == TEST_SIZE,
        "test_linkedlist_addall(): Discrepancy in linked list length.");
}

void test_linkedlist_addone(testresults_t* results, linkedlist_t* linkedlist, int index) {
    int length = linkedlist->length;
    char element[BUFFER_SIZE];
    sprintf(element, "Element %d (added)", index);
    tests_assert(results, 
        linkedlist_add(linkedlist, index, element) == linkedlist_success,
        "test_linkedlist_addone(): Add of linked list element at index %d returned wrong value.", index);

    tests_assert(results, 
        linkedlist->length == length + 1,
        "test_linkedlist_addone(): Discrepancy in linked list length.", index);
        
    void* elemento;
    tests_assert(results, 
        linkedlist_get(linkedlist, index, &elemento) == linkedlist_success,
        "test_linkedlist_addone(): Get of linked list element at index %d returned wrong value.", index);
    
    tests_assert(results, 
        strcmp((char*) elemento, element) == 0,
        "test_linkedlist_addone(): Discrepancy in linked list element at index %d.", index);
}

void test_linkedlist_getall(testresults_t* results, linkedlist_t* linkedlist) {
    void* element;
    int i; for (i = 0; i < linkedlist->length; i++) {            
        tests_assert(results, 
            linkedlist_get(linkedlist, i, &element) == linkedlist_success,
            "test_linkedlist_getall(): Get of element \"%s\" returned the wrong value.", element);

        char comparator[BUFFER_SIZE];
        sprintf(comparator, "Element %d", i);
        
        tests_assert(results, 
            strcmp((char*) element, comparator) == 0,
            "test_linkedlist_getall(): Got element \"%s\". Expected \"%s\".", element, comparator);
    }
}

void test_linkedlist_removeone(testresults_t* results, linkedlist_t* linkedlist, int index) {
    int length = linkedlist->length;    
    tests_assert(results, 
        linkedlist_remove(linkedlist, index) == linkedlist_success,
        "test_linkedlist_removeone(): Removal of linked list element at index %d returned wrong value.", index);

    tests_assert(results, 
        linkedlist->length == length - 1,
        "test_linkedlist_removeone(): Discrepancy in linked list length.", index);
}

void test_linkedlist_removeall(testresults_t* results, linkedlist_t* linkedlist) {
    for (;0 < linkedlist->length;) {
        tests_assert(results, 
            linkedlist_remove(linkedlist, 0) == linkedlist_success,
            "test_linkedlist_removeall(): Removal of linked list element at index %d returned wrong value.", index);
    }
    
    tests_assert(results, 
        linkedlist->length == 0,
        "test_linkedlist_removeall(): Discrepancy in linked list length.", index);
}

void test_linkedlist_destroy(testresults_t* results, linkedlist_t* linkedlist) {    
    tests_assert(results, 
        linkedlist_destroy(linkedlist) == linkedlist_success,
        "test_queue_destroy(): Destroyal of linked list returned wrong value.");
}

void test_linkedlist_print(linkedlist_t* linkedlist) {
    log_debug("test_linkedlist_printlist(): Printing list of elements.");
    node_t *cnode = linkedlist->head;
    while (cnode != NULL) {
        log_debug("%s", (char*) cnode->element);
        cnode = cnode->next;
    }
}

void test_linkedlist_reverse_print(linkedlist_t* linkedlist) {
    log_debug("test_linkedlist_rprintlist(): Printing reverse list of elements.");
    node_t *cnode = linkedlist->tail;
    while (cnode != NULL) {
        log_debug("%s", (char*) cnode->element);
        cnode = cnode->previous;
    }
}