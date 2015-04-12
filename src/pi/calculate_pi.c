#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "../lib/collections/queue.h"
#include "../lib/logging/logging.h"
#include "../lib/threading/threadpool.h"
#include "../lib/threading/commons.h"
#include "../lib/threading/future.h"
#include "calculate_pi.h"

#define true 1
#define false 0
#define TIMEOUT 3600 * 1000
#define THREAD_COUNT 20
#define RANGE_BY_THREAD 0.05
#define FUTURE_BY_REDUCE 3
#define DX 0.000000000001

// The log level of the application. 
// All logs of this level or above will be logged.
unsigned int log_level = DEBUG_LVL;

// The buffer size to use for logging.
const unsigned int LOG_BUFFER_SIZE = 1024;

int main(void) {
	queue_t future_queue;
	queue_init(&future_queue);

	threadpool_options_t threadpool_options = { .threadpool_size = THREAD_COUNT, .timeout = TIMEOUT };
	threadpool_t threadpool;
	threadpool_init(&threadpool, threadpool_options);
	
	long double x_coord;
	for (x_coord = 0; x_coord <= 1; x_coord += RANGE_BY_THREAD) {
		range_t* range = malloc(sizeof(range_t));
		range->initial_value = x_coord;
		range->range = RANGE_BY_THREAD;
	
		future_t* future = malloc(sizeof(future_t));
		threadpool_submit_task(&threadpool, &pi_map, range, future);
		queue_enqueue(&future_queue, future);
	}
	
	long double quarter_area = 0;
	void* current;
	queue_state_t state = queue_dequeue(&future_queue, &current);
	while (state == queue_success) {
		future_t* future = current;
		future_state_t future_state;
		while ((future_state = future_wait(future, TIMEOUT)) != future_success) 
			log_error("future_state: %d", future_state);;

		long double* partial_quarter_area = future->out_value;
		quarter_area += *partial_quarter_area;
		state = queue_dequeue(&future_queue, &current);
	}
	
	threadpool_destroy(&threadpool);
	log_info("Pi value: %.20Lf", 4.0 * quarter_area);

	// queue_t future_queue;
	// queue_init(&future_queue);
	// 
	// threadpool_options_t threadpool_options = { .size = 10, .timeout = 30 * 1000 };
	// threadpool_t threadpool;
	// threadpool_init(&threadpool, threadpool_options);
	// 
	// long double x_coord;
	// for (x_coord = 0; x_coord <= 1; x_coord += RANGE_BY_THREAD) {
	// 	range_t* range = malloc(sizeof(range_t));
	// 	range->initial_value = x_coord;
	// 	range->range = RANGE_BY_THREAD;
	// 
	// 	threadpool_future_t future;
	// 	threadpool_submit_func(&threadpool, &pi_map, range, &future);
	// 	queue_enqueue(&future_queue, &future);
	// }
	// 
	// partial_area_futures_t* partial_area_futures = malloc(sizeof(partial_area_futures_t));
	// partial_area_futures->count = 0;
	// partial_area_futures->futures = malloc((FUTURE_BY_REDUCE + 1) * sizeof(threadpool_future_t));
	// 
	// unsigned int i_element = 0;
	// void* element;
	// void* previous_element;
	// while (true) {
	// 	queue_dequeue(&future_queue, &element);
	// 	if (element == NULL) {
	// 		break;
	// 	}
	// 
	// 	partial_area_futures->count++;
	// 	partial_area_futures->futures[i_element % FUTURE_BY_REDUCE] = *((threadpool_future_t*) element);
	// 
	// 	if (i_element % FUTURE_BY_REDUCE == FUTURE_BY_REDUCE - 1) {
	// 		threadpool_future_t future;
	// 		threadpool_submit_func(&threadpool, &pi_reduce, &partial_area_futures, &future);
	// 
	// 		partial_area_futures = malloc(sizeof(partial_area_futures_t));
	// 		partial_area_futures->count = 0;
	// 		partial_area_futures->futures = malloc((FUTURE_BY_REDUCE + 1) * sizeof(threadpool_future_t));
	// 	}
	// 
	// 	previous_element = element;
	// 	i_element++;
	// }
	// 
	// if (i_element % FUTURE_BY_REDUCE != 0) {
	// 	threadpool_future_t* previous_future = previous_element;
	// 	partial_area_futures->futures[i_element % FUTURE_BY_REDUCE + 1] = *previous_future;
	// 
	// 	threadpool_future_t future;
	// 	threadpool_submit_func(&threadpool, &pi_reduce, &partial_area_futures, &future);
	// 	pthread_mutex_lock(&future.mutex);
	// 	log_info("pi value is %.10f", *((long double *) future.future_value) * 4);
	// } else {
	// 	threadpool_future_t* previous_future = previous_element;
	// 	pthread_mutex_lock(&previous_future->mutex);
	// 	log_info("pi value is %.10f", *((long double *) previous_future->future_value) * 4);
	// }

	exit(0);
}

int pi_map(threading_cancel_token_t cancel_token, void* arguments, void** return_value) {
	range_t* range = arguments;
	long double* partial_area = calloc(1, sizeof(long double));
	long double x_coord;

	for (x_coord = range->initial_value; !cancel_token.is_cancelled && x_coord <= range->initial_value + range->range; x_coord += DX) {
		long double y_value = sqrt(1.0 - pow(x_coord, 2));
		*partial_area += DX * y_value;
	}
	log_warn("%Lf", *partial_area);
	free(range);
	*return_value = partial_area;
	return 0;
}

int pi_reduce(threading_cancel_token_t cancel_token, void* arguments, void** return_value) {
	// partial_area_futures_t* partial_area_futures = arguments;
	// 
	// int i_future;
	// long double* reduced_partial_area = calloc(1, sizeof(long double));
	// for (i_future = 0; !cancel_token && i_future < partial_area_futures->count; i_future++) {
	// 	pthread_mutex_lock(&partial_area_futures->futures[i_future].mutex);
	// 	long double* partial_area = partial_area_futures->futures[i_future].future_value;
	// 	*reduced_partial_area += *partial_area;
	// 	
	// 	free(partial_area_futures->futures[i_future].future_value);
	// 	future_destroy(&partial_area_futures->futures[i_future]);
	// }
	// 
	// free(partial_area_futures->futures);
	// free(partial_area_futures);
	// 
	// *return_value = reduced_partial_area;
	return 0;
}