#ifndef LIB_COLLECTIONS_TESTS_BLOCKING_QUEUE_TESTS_H
#define LIB_COLLECTIONS_TESTS_BLOCKING_QUEUE_TESTS_H

// Unit test methods for the queue collection.
void test_blocking_queue_init(testresults_t* results, blocking_queue_t* blocking_queue);
void test_blocking_queue_queueall(testresults_t* results, blocking_queue_t* blocking_queue);
void test_blocking_queue_dequeueall(testresults_t* results, blocking_queue_t* blocking_queue);
void test_blocking_queue_close(testresults_t* results, blocking_queue_t* blocking_queue);
void test_blocking_queue_destroy(testresults_t* results, blocking_queue_t* blocking_queue);

// Utility methods relative to tests.
void test_blocking_queue_print(blocking_queue_t* blocking_queue);

#endif