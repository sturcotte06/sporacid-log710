#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../../logging/logging.h"
#include "../../../tests/tests.h"
#include "../../../threading/commons.h"
#include "../blocking_queue.h"
#include "blocking_queue_tests.h"

#define true 1
#define false 0
#define BUFFER_SIZE 256
#define TEST_SIZE 1053
#define BUFFER_SIZE 256
#define THREAD_COUNT 25

unsigned int log_level = INFO_LVL;
const unsigned int LOG_BUFFER_SIZE = 1024;

void* test_dequeue_routine(void* uncasted_test_dequeue_thread_args);

struct test_dequeue_thread_args_t {
	testresults_t* results;
	blocking_queue_t* blocking_queue;
};

pthread_mutex_t mutex;
pthread_t threads[THREAD_COUNT];
int dequeue_count;

int main(void) {
    // Tests blocking queue.
    testresults_t results;
    blocking_queue_t blocking_queue;
    tests_start(&results, stdout);
    test_blocking_queue_init(&results, &blocking_queue);
    test_blocking_queue_queueall(&results, &blocking_queue);
	test_blocking_queue_print(&blocking_queue);
    test_blocking_queue_dequeueall(&results, &blocking_queue);
	test_blocking_queue_close(&results, &blocking_queue);
    test_blocking_queue_destroy(&results, &blocking_queue);
    tests_end(&results);
    exit(0);
}

void test_blocking_queue_init(testresults_t* results, blocking_queue_t* blocking_queue) {
	blocking_queue_options_t options = { .timeout = 30 * 1000 };
	tests_assert(results, 
        blocking_queue_init(blocking_queue, options) == blocking_queue_success,
        "test_blocking_queue_init(): Initialization of blocking queue returned wrong value.");
}

void test_blocking_queue_queueall(testresults_t* results, blocking_queue_t* blocking_queue) {
	int i_element;
	for (i_element = 0; i_element < TEST_SIZE; i_element++) {
        char* element = malloc(BUFFER_SIZE * sizeof(char));
        sprintf(element, "Element %d", i_element);
        
        tests_assert(results, 
            blocking_queue_enqueue(blocking_queue, element) == blocking_queue_success,
            "test_blocking_queue_queueall(): Queuing of element \"%s\" returned the wrong value.", element);
    }
}

void test_blocking_queue_dequeueall(testresults_t* results, blocking_queue_t* blocking_queue) {
	pthread_mutex_init(&mutex, NULL);
	dequeue_count = 0;

	int i_thread;
	for (i_thread = 0; i_thread < THREAD_COUNT; i_thread++) {
		struct test_dequeue_thread_args_t* test_dequeue_thread_args = malloc(sizeof(struct test_dequeue_thread_args_t));
		test_dequeue_thread_args->results = results;
		test_dequeue_thread_args->blocking_queue = blocking_queue;

		pthread_create(&threads[i_thread], NULL, &test_dequeue_routine, test_dequeue_thread_args);
	}
}

void test_blocking_queue_close(testresults_t* results, blocking_queue_t* blocking_queue) {
	tests_assert(results, 
        blocking_queue_close(blocking_queue) == blocking_queue_success,
        "test_blocking_queue_close(): Closure of queue returned wrong value.");

	int i_thread;
	for (i_thread = 0; i_thread < THREAD_COUNT; i_thread++) {
		pthread_join(threads[i_thread], NULL);
	}
}

void test_blocking_queue_destroy(testresults_t* results, blocking_queue_t* blocking_queue) {
	tests_assert(results, 
        blocking_queue_destroy(blocking_queue) == blocking_queue_success,
        "test_blocking_queue_destroy(): Destroyal of queue returned wrong value.");
}

void* test_dequeue_routine(void* uncasted_test_dequeue_thread_args) {
	struct test_dequeue_thread_args_t* test_dequeue_thread_args = uncasted_test_dequeue_thread_args;
	blocking_queue_t* blocking_queue = test_dequeue_thread_args->blocking_queue;
	testresults_t* results = test_dequeue_thread_args->results;

	void* element;
	while (true) {
		pthread_mutex_lock(&mutex);

		blocking_queue_state_t state = blocking_queue_dequeue(blocking_queue, &element);
		tests_assert(results, 
            state == blocking_queue_success || 
			(state == blocking_queue_closed && blocking_queue->inner_queue.inner_list.length == 0 && element == NULL && blocking_queue->is_closed),
            "test_dequeue_routine(): Dequeue of element returned the wrong value %d.", state);
		if (state == blocking_queue_closed) {
			pthread_mutex_unlock(&mutex);
			break;
		} 
		if (state != blocking_queue_success) {
			pthread_mutex_unlock(&mutex);
			continue;
		}

		char* comparee = element;
		char* comparand = malloc(BUFFER_SIZE * sizeof(char));
        sprintf(comparand, "Element %d", dequeue_count);
		dequeue_count++;

		pthread_mutex_unlock(&mutex);
            
        tests_assert(results, 
            strcmp(comparee, comparand) == 0,
            "test_dequeue_routine(): Dequeued element \"%s\". Expected \"%s\".", comparee, comparand);
	}

	return 0;
}

// Utility methods relative to tests.
void test_blocking_queue_print(blocking_queue_t* blocking_queue) {
	log_debug("test_blocking_queue_print(): Printing list of elements.");
    node_t *cnode = blocking_queue->inner_queue.inner_list.head;
    while (cnode != NULL) {
        log_debug("%s", (char*) cnode->element);
        cnode = cnode->next;
    }
}