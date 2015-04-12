#ifndef LIB_COLLECTIONS_TESTS_QUEUE_TESTS_H
#define LIB_COLLECTIONS_TESTS_QUEUE_TESTS_H

#include "../queue.h"

// Unit test methods for the queue collection.
void test_queue_init(testresults_t* results, queue_t* queue);
void test_queue_queueall(testresults_t* results, queue_t* queue);
void test_queue_dequeueall(testresults_t* results, queue_t* queue);
void test_queue_destroy(testresults_t* results, queue_t* queue);

// Utility methods relative to tests.
void test_queue_print(queue_t* queue);

#endif