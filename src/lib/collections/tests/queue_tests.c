#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../logging/logging.h"
#include "../../tests/tests.h"
#include "../queue.h"
#include "queue_tests.h"

#define BUFFER_SIZE 256
#define TEST_SIZE 1053
#define BUFFER_SIZE 256

unsigned int log_level = INFO_LVL;
const unsigned int LOG_BUFFER_SIZE = 1024;

int main(void) {
    // Tests queue.
    testresults_t results;
    queue_t queue;
    tests_start(&results, stdout);
    test_queue_init(&results, &queue);
    test_queue_queueall(&results, &queue);
	test_queue_print(&queue);
    test_queue_dequeueall(&results, &queue);
	test_queue_print(&queue);
    test_queue_destroy(&results, &queue);
    tests_end(&results);
    exit(0);
}

void test_queue_init(testresults_t* results, queue_t* queue) {
    tests_assert(results, 
        queue_init(queue) == queue_success,
        "test_queue_init(): Initialization of queue returned wrong value.");
}

void test_queue_queueall(testresults_t* results, queue_t* queue) {
    int i; for (i = 0; i < TEST_SIZE; i++) {
        char* element = malloc(BUFFER_SIZE * sizeof(char));
        sprintf(element, "Element %d", i);
        
        tests_assert(results, 
            queue_enqueue(queue, element) == queue_success,
            "test_queue_queueall(): Queuing of element \"%s\" returned the wrong value.", element);
    }
}

void test_queue_dequeueall(testresults_t* results, queue_t* queue) {
    int i = 0;
    void* element;
    do {
		queue_state_t state = queue_dequeue(queue, &element);
		tests_assert(results, 
            state == queue_success || (state == queue_empty && queue->inner_list.length == 0 && element == NULL),
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

void test_queue_destroy(testresults_t* results, queue_t* queue) {
    tests_assert(results, 
        queue_destroy(queue) == queue_success,
        "test_queue_destroy(): Destroyal of queue returned wrong value.");
}

void test_queue_print(queue_t* queue) {
    log_debug("test_queue_print(): Printing list of elements.");
    node_t *cnode = queue->inner_list.head;
    while (cnode != NULL) {
        log_debug("%s", (char*) cnode->element);
        cnode = cnode->next;
    }
}