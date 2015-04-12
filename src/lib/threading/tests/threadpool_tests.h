#ifndef LIB_THREADING_TESTS_THREADPOOL_TESTS_H
#define LIB_THREADING_TESTS_THREADPOOL_TESTS_H

#include "../future.h"
#include "../threadpool.h"

// Unit test methods for the thread pool.
void test_threadpool_init(testresults_t* results, threadpool_t* threadpool);
void test_threadpool_submit_all_tasks(testresults_t* results, threadpool_t* threadpool);
void test_threadpool_wait_all_tasks(testresults_t* results, threadpool_t* threadpool);
void test_threadpool_close(testresults_t* results, threadpool_t* threadpool);
void test_threadpool_destroy_future(testresults_t* results, future_t* future);
void test_threadpool_destroy(testresults_t* results, threadpool_t* threadpool);

// Utility methods relative to tests.
void test_threadpool_print(threadpool_t* threadpool);

#endif