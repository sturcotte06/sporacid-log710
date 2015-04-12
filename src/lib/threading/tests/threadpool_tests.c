#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../logging/logging.h"
#include "../../tests/tests.h"
#include "../../collections/linkedlist.h"
#include "../commons.h"
#include "../future.h"
#include "../threadpool.h"
#include "threadpool_tests.h"

#define TEST_SIZE 50
#define BUFFER_SIZE 256
#define VALUES_TO_SUM_COUNT 5

int test_task_func(threading_cancel_token_t cancel_token, void* uncasted_values_to_sum, void** sum);

unsigned int log_level = DEBUG_LVL;
const unsigned int LOG_BUFFER_SIZE = 1024;
linkedlist_t future_list;

int main(void) {
	linkedlist_init(&future_list);

	// Tests threadpool.
    testresults_t results;
    threadpool_t threadpool;
    tests_start(&results, stdout);
    test_threadpool_init(&results, &threadpool);
    test_threadpool_submit_all_tasks(&results, &threadpool);
    test_threadpool_wait_all_tasks(&results, &threadpool);
    // test_threadpool_close(&results, &threadpool);
    test_threadpool_destroy(&results, &threadpool);
    tests_end(&results);
    exit(0);
}

void test_threadpool_init(testresults_t* results, threadpool_t* threadpool) {
	char* threadpool_name = malloc(BUFFER_SIZE * sizeof(char));
	sprintf(threadpool_name, "Test threadpool");

	threadpool_options_t options = { .threadpool_name = threadpool_name, .threadpool_size = 10, .timeout = 30 * 1000 };
	threadpool_state_t state = threadpool_init(threadpool, options);
	tests_assert(results, 
        state == threadpool_success,
        "test_threadpool_init(): Initialization of thread pool returned wrong value %d.", state);
}

void test_threadpool_submit_all_tasks(testresults_t* results, threadpool_t* threadpool) {
	int i_task = 0;
	for (i_task = 0; i_task < TEST_SIZE; i_task++) {
		int i_value;
		int* values_to_sum = malloc(VALUES_TO_SUM_COUNT * sizeof(int));
		for (i_value = 0; i_value < VALUES_TO_SUM_COUNT; i_value++) {
			values_to_sum[i_value] = (i_task * VALUES_TO_SUM_COUNT) + i_value;
		}

		future_t* future = malloc(sizeof(future_t));
		threadpool_state_t state = threadpool_submit_task(threadpool, &test_task_func, values_to_sum, future);
		tests_assert(results, 
			state == threadpool_success,
			"test_threadpool_submit_all_tasks(): Submit task to thread pool returned wrong value %d.", state);

		linkedlist_add(&future_list, future_list.length, future);
	}
}

void test_threadpool_wait_all_tasks(testresults_t* results, threadpool_t* threadpool) {
	int i_current = 0;
	node_t* current = future_list.head;
	while (current != NULL) {
		int i_value;
		int expected_sum = 0;
		for (i_value = 0; i_value < VALUES_TO_SUM_COUNT; i_value++) {
			expected_sum += (i_current * VALUES_TO_SUM_COUNT) + i_value;
		}

		future_t* future = current->element;
		future_state_t state = future_wait(future, 30 * 1000);
		tests_assert(results, 
			state == future_success,
			"test_threadpool_wait_all_tasks(): Wait future returned wrong value %d.", state);

		int* out_value = future->out_value;
		tests_assert(results,
			*out_value == expected_sum,
			"test_threadpool_wait_all_tasks(): Wrong future value %d, expected %d.", *out_value, expected_sum);

		test_threadpool_destroy_future(results, future);
		i_current++;
		current = current->next;
	}
}

void test_threadpool_close(testresults_t* results, threadpool_t* threadpool) {
	threadpool_state_t state = threadpool_close(threadpool);
	tests_assert(results, 
        state == threadpool_success,
        "test_threadpool_close(): Closure of thread pool returned wrong value %d.", state);
}

void test_threadpool_destroy_future(testresults_t* results, future_t* future) {
	future_state_t state = future_destroy(future);
	tests_assert(results, 
        state == future_success,
        "test_threadpool_destroy_future(): Destroyal of future returned wrong value %d.", state);
}

void test_threadpool_destroy(testresults_t* results, threadpool_t* threadpool) {
	threadpool_state_t state = threadpool_destroy(threadpool);
	tests_assert(results, 
        state == threadpool_success,
        "test_threadpool_close(): Destroyal of thread pool returned wrong value %d.", state);
}

void test_threadpool_print(threadpool_t* threadpool) {
}

int test_task_func(threading_cancel_token_t cancel_token, void* uncasted_values_to_sum, void** sum) {
	int* values_to_sum = uncasted_values_to_sum;
	int* partial_sum = malloc (sizeof(int));
	int i_value;
	for (i_value = 0; !cancel_token.is_cancelled && i_value < VALUES_TO_SUM_COUNT; i_value++) {
		*partial_sum += *(values_to_sum + i_value);
	}

	*sum = partial_sum;
	return 0;
}