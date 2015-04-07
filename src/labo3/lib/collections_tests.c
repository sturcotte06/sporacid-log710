#define BUFFER_SIZE 256

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logging.h"
#include "tests.h"
#include "collections.h"
#include "collections_tests.h"

unsigned int log_level = INFO_LVL;

const int TEST_SIZE = 100;
const int SUCCESSFUL_EXEC = 0;
const int EMPTY_QUEUE_ERRNO = 2;
const int OUT_OF_BOUNDS_ERRNO = 3;
const int NULL_LINKED_LIST_ERRNO = 4;
const int NULL_QUEUE_ERRNO = 5;

int main(void) {
    struct testresults_t results;

    // Tests linked list.
    struct linkedlist_t linkedlist;
    tests_start(&results, stdout);
    test_linkedlist_init(&results, &linkedlist);
    test_linkedlist_addall(&results, &linkedlist);
    test_linkedlist_printlist(&linkedlist);
    test_linkedlist_rprintlist(&linkedlist);
    test_linkedlist_getall(&results, &linkedlist);
    test_linkedlist_addone(&results, &linkedlist, 2);
    test_linkedlist_addone(&results, &linkedlist, 8);
    test_linkedlist_removeone(&results, &linkedlist, 3);
    test_linkedlist_removeone(&results, &linkedlist, 7);
    test_linkedlist_removeall(&results, &linkedlist);
    test_linkedlist_destroy(&results, &linkedlist);
    tests_end(&results);
    
    // Tests queue.
    struct queue_t queue;
    tests_start(&results, stdout);
    test_queue_init(&results, &queue);
    test_queue_queueall(&results, &queue);
    test_queue_dequeueall(&results, &queue);
    test_queue_destroy(&results, &queue);
    tests_end(&results);
    
    exit(0);
}

// @LinkedListTest
void test_linkedlist_init(testresults_t* results, linkedlist_t* linkedlist) {
    tests_assert(results, 
        linkedlist_init(linkedlist) == SUCCESSFUL_EXEC,
        "test_linkedlist_init(): Initialization of linked list returned wrong value.");
}

// @LinkedListTest
void test_linkedlist_addall(testresults_t* results, linkedlist_t* linkedlist) {    
    int i; for (i = 0; i < TEST_SIZE; i++) {
        char* element = malloc(BUFFER_SIZE * sizeof(char));
        sprintf(element, "Element %d", i);
    
        tests_assert(results, 
            linkedlist_add(linkedlist, i, element) == SUCCESSFUL_EXEC,
            "test_linkedlist_addall(): Queuing of element \"%s\" returned the wrong value.", element);
    }
    
    tests_assert(results, 
        linkedlist->length == TEST_SIZE,
        "test_linkedlist_addall(): Discrepancy in linked list length.");
}

// @LinkedListTest
void test_linkedlist_addone(testresults_t* results, linkedlist_t* linkedlist, int index) {
    int length = linkedlist->length;
    char element[BUFFER_SIZE];
    sprintf(element, "Element %d (added)", index);
    tests_assert(results, 
        linkedlist_add(linkedlist, index, element) == SUCCESSFUL_EXEC,
        "test_linkedlist_addone(): Add of linked list element at index %d returned wrong value.", index);

    tests_assert(results, 
        linkedlist->length == length + 1,
        "test_linkedlist_addone(): Discrepancy in linked list length.", index);
        
    void* elemento;
    tests_assert(results, 
        linkedlist_get(linkedlist, index, &elemento) == SUCCESSFUL_EXEC,
        "test_linkedlist_addone(): Get of linked list element at index %d returned wrong value.", index);
    
    tests_assert(results, 
        strcmp((char*) elemento, element) == 0,
        "test_linkedlist_addone(): Discrepancy in linked list element at index %d.", index);
}

// @LinkedListTest
void test_linkedlist_getall(testresults_t* results, linkedlist_t* linkedlist) {
    void* element;
    int i; for (i = 0; i < linkedlist->length; i++) {            
        tests_assert(results, 
            linkedlist_get(linkedlist, i, &element) == SUCCESSFUL_EXEC,
            "test_linkedlist_getall(): Get of element \"%s\" returned the wrong value.", element);

        char comparator[BUFFER_SIZE];
        sprintf(comparator, "Element %d", i);
        
        tests_assert(results, 
            strcmp((char*) element, comparator) == 0,
            "test_linkedlist_getall(): Got element \"%s\". Expected \"%s\".", element, comparator);
    }
}

// @LinkedListTest
void test_linkedlist_removeone(testresults_t* results, linkedlist_t* linkedlist, int index) {
    int length = linkedlist->length;    
    tests_assert(results, 
        linkedlist_remove(linkedlist, index) == SUCCESSFUL_EXEC,
        "test_linkedlist_removeone(): Removal of linked list element at index %d returned wrong value.", index);

    tests_assert(results, 
        linkedlist->length == length - 1,
        "test_linkedlist_removeone(): Discrepancy in linked list length.", index);
}

// @LinkedListTest
void test_linkedlist_removeall(testresults_t* results, linkedlist_t* linkedlist) {
    for (;0 < linkedlist->length;) {
        tests_assert(results, 
            linkedlist_remove(linkedlist, 0) == SUCCESSFUL_EXEC,
            "test_linkedlist_removeall(): Removal of linked list element at index %d returned wrong value.", index);
    }
    
    tests_assert(results, 
        linkedlist->length == 0,
        "test_linkedlist_removeall(): Discrepancy in linked list length.", index);
}

// @LinkedListTest
void test_linkedlist_destroy(testresults_t* results, linkedlist_t* linkedlist) {    
    tests_assert(results, 
        linkedlist_destroy(linkedlist) == SUCCESSFUL_EXEC,
        "test_queue_destroy(): Destroyal of linked list returned wrong value.");
}

// @QueueTest
void test_queue_init(testresults_t* results, queue_t* queue) {
    tests_assert(results, 
        queue_init(queue) == SUCCESSFUL_EXEC,
        "test_queue_init(): Initialization of queue returned wrong value.");
}

// @QueueTest
void test_queue_queueall(testresults_t* results, queue_t* queue) {
    int i; for (i = 0; i < TEST_SIZE; i++) {
        char* element = malloc(BUFFER_SIZE * sizeof(char));
        sprintf(element, "Element %d", i);
        
        tests_assert(results, 
            queue_enqueue(queue, element) == SUCCESSFUL_EXEC,
            "test_queue_queueall(): Queuing of element \"%s\" returned the wrong value.", element);
    }
}

// @QueueTest
void test_queue_dequeueall(testresults_t* results, queue_t* queue) {
    int i = 0;
    void* element;
    do {
        tests_assert(results, 
            queue_dequeue(queue, &element) == SUCCESSFUL_EXEC,
            "test_queue_dequeueall(): Dequeue of element \"%s\" returned the wrong value.", element);
            
        if (element != NULL) {
            char comparator[BUFFER_SIZE];
            sprintf(comparator, "Element %d", i);
            
            tests_assert(results, 
                strcmp((char*) element, comparator) == 0,
                "test_queue_dequeueall(): Dequeued element \"%s\". Expected \"%s\".", element, comparator);
        }
            
        i++;
    } while (element != NULL);
}

// @QueueTest
void test_queue_destroy(testresults_t* results, queue_t* queue) {
    tests_assert(results, 
        queue_destroy(queue) == SUCCESSFUL_EXEC,
        "test_queue_destroy(): Destroyal of queue returned wrong value.");
}

void test_linkedlist_printlist(linkedlist_t* linkedlist) {
    log_debug("test_linkedlist_printlist(): Printing list of elements.");
    node_t *cnode = linkedlist->head;
    while (cnode != NULL) {
        log_debug("%s", (char*) cnode->element);
        cnode = cnode->next;
    }
}

void test_linkedlist_rprintlist(linkedlist_t* linkedlist) {
    log_debug("test_linkedlist_rprintlist(): Printing reverse list of elements.");
    node_t *cnode = linkedlist->tail;
    while (cnode != NULL) {
        log_debug("%s", (char*) cnode->element);
        cnode = cnode->previous;
    }
}