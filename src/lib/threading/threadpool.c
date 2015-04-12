#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../collections/synchronized/blocking_queue.h"
#include "../logging/logging.h"
#include "commons.h"
#include "future.h"
#include "threadpool.h"

#define true 1
#define false 0

/// <summary>
/// Routine for a thread pool worker thread. The thread will dequeue task and run them.
/// </summary>
/// <param name="uncasted_threadpool">The thread pool to which this thread belongs.</param>
/// <returns>The comparison result.</returns>
void* threadpool_worker_routine(void* uncasted_threadpool);

/// <summary>
/// Function that compares two thread pool tasks and return a -1 if comparee &lt; comparand, 0 if comparee = comparand and 1 if comparee &gt; comparand.
/// </summary>
/// <param name="comparee">The comparee thread pool task.</param>
/// <param name="comparee">The comparand thread pool task.</param>
/// <returns>The comparison result.</returns>
int threadpool_task_priority_comparer(void* comparee, void* comparand);

/// <summary>
/// Initializes a thread pool.
/// </summary>
/// <param name="threadpool">Struct in which to put the created thread pool.</param>
/// <param name="options">Options for the threadpool.</param>
/// <returns>The state enum value.</returns>
threadpool_state_t threadpool_init(threadpool_t* threadpool, threadpool_options_t options) {
	log_debug("Entering threadpool_init()");

	if (threadpool == NULL) {
		return threadpool_invalid_args;
	}

	if (!options.threadpool_size) {
		return threadpool_invalid_args;
	}

	// Initialize the blocking queue.
	blocking_queue_options_t blocking_queue_options = { .priority_comparer = &threadpool_task_priority_comparer, .timeout = options.timeout };
	blocking_queue_state_t blocking_queue_state = blocking_queue_init(&threadpool->task_blocking_queue, blocking_queue_options);
	if (blocking_queue_state != blocking_queue_success) return threadpool_blocking_collection_error;

	// Just put the options in the structure. Nothing else to do for now.
	threadpool->threadpool_options = options;

	// Initialize the thread buffer and all its threads.
	unsigned int i_available_thread;
	threadpool->available_threads = malloc(options.threadpool_size * sizeof(pthread_t));
	for (i_available_thread = 0; i_available_thread < options.threadpool_size; i_available_thread++) {
		// Create the thread with the task consuming routine.
		int result = pthread_create(&threadpool->available_threads[i_available_thread], NULL, &threadpool_worker_routine, threadpool);
		if (result != 0) return threadpool_thread_error;
	}

	log_debug("Exiting threadpool_init().");
	return threadpool_success;
}

/// <summary>
/// Closes the given thread pool. If the timeout is reached, the thread pool will be closed by force.
/// </summary>
/// <param name="threadpool">Thread pool to close.</param>
/// <returns>The state enum value.</returns>
threadpool_state_t threadpool_close(threadpool_t* threadpool) {
	log_debug("Entering threadpool_close().");

	if (threadpool == NULL) {
		return threadpool_invalid_args;
	}

	// Close the blocking queue.
	blocking_queue_state_t blocking_queue_state = blocking_queue_close(&threadpool->task_blocking_queue);
	if (blocking_queue_state != blocking_queue_success) return threadpool_blocking_collection_error;

	// Set the closing flag to true and wait for all workers to end.
	int i_available_thread;
	for (i_available_thread = 0; i_available_thread < threadpool->threadpool_options.threadpool_size; i_available_thread++) {
		int result = pthread_join(threadpool->available_threads[i_available_thread], NULL);
		if (result != 0) return threadpool_thread_error;
	}

	// TODO honor timeout.

	log_debug("Exiting threadpool_close().");
	return threadpool_success;
}

/// <summary>
/// Destroys the thread pool with the given id.
/// </summary>
/// <param name="threadpool">Thread pool to destroy.</param>
/// <returns>The state enum value.</returns>
threadpool_state_t threadpool_destroy(threadpool_t* threadpool) {
	log_debug("Entering threadpool_destroy().");

	if (threadpool == NULL) {
		return threadpool_invalid_args;
	}

	// Wait for thread pool to close.
	threadpool_close(threadpool);

	// Destroy the blocking queue.
	blocking_queue_destroy(&threadpool->task_blocking_queue);

	// Destroy the thread buffer.
	free(threadpool->available_threads);

	log_debug("Exiting threadpool_destroy().");
	return threadpool_success;
}

/// <summary>
/// Submits an action with a return value into the thread pool.
/// The given future structure will be modified and waiting on its mutex will wait for the result.
/// </summary>
/// <param name="threadpool">Thread pool in which to submit.</param>
/// <param name="func">The action with a return value to execute.</param>
/// <param name="arguments">The arguments of the action.</param>
/// <param name="future">The future structure that holds the return value.</param>
/// <returns>The state enum value.</returns>
threadpool_state_t threadpool_submit_task(threadpool_t* threadpool, threadpool_func_t func, void* arguments, future_t* future) {
	return threadpool_submit_priorized_task(threadpool, func, arguments, future, normal);
}

/// <summary>
/// Submits an action with a return value with a certain priority into the thread pool.
/// The given future structure will be modified and waiting on its mutex will wait for the result.
/// </summary>
/// <param name="threadpool">Thread pool in which to submit.</param>
/// <param name="func">The action with a return value to execute.</param>
/// <param name="arguments">The arguments of the action.</param>
/// <param name="future">The future structure that holds the return value.</param>
/// <param name="priority">Priority of the action.</param>
/// <returns>The state enum value.</returns>
threadpool_state_t threadpool_submit_priorized_task(threadpool_t* threadpool, threadpool_func_t func, void* arguments, future_t* future, threadpool_priority_t priority) {
	log_debug("Entering threadpool_submit_priorized_task().");

	if (threadpool == NULL) {
		return threadpool_invalid_args;
	}

	// Initialize the future.
	future_state_t state = future_init(future);
	if (state != future_success) return threadpool_future_error;

	// Create a new func task.
	threadpool_task_t* threadpool_task = malloc(sizeof(threadpool_task_t));
	threadpool_task->priority = priority;
	threadpool_task->func = func;
	threadpool_task->func_future = future;
	threadpool_task->arguments = arguments;

	// Enqueue the action.
	blocking_queue_state_t blocking_queue_state = blocking_queue_enqueue(&threadpool->task_blocking_queue, threadpool_task);
	if (blocking_queue_state != blocking_queue_success) return threadpool_blocking_collection_error;

	log_debug("Exiting threadpool_submit_priorized_task().");
	return threadpool_success;
}

/// <summary>
/// Function that compares two thread pool tasks and return a -1 if comparee &lt; comparand, 0 if comparee = comparand and 1 if comparee &gt; comparand.
/// </summary>
/// <param name="comparee">The comparee thread pool task.</param>
/// <param name="comparee">The comparand thread pool task.</param>
/// <returns>The comparison result.</returns>
int threadpool_task_priority_comparer(void* comparee, void* comparand) {
	threadpool_task_t* comparee_task = comparee;
	threadpool_task_t* comparand_task = comparand;
	return comparee_task->priority < comparand_task->priority 
		? -1 : comparee_task->priority > comparand_task->priority
		? 1 : 0;
}

/// <summary>
/// Routine for a thread pool worker thread. The thread will dequeue task and run them.
/// </summary>
/// <param name="uncasted_threadpool">The thread pool to which this thread belongs.</param>
/// <returns>The comparison result.</returns>
void* threadpool_worker_routine(void* uncasted_threadpool) {
	threadpool_t* threadpool = uncasted_threadpool;

	while (true) {
		// Dequeue one task.
		void* uncasted_threadpool_task;
		blocking_queue_state_t blocking_queue_state = blocking_queue_dequeue(&threadpool->task_blocking_queue, &uncasted_threadpool_task);

		if (blocking_queue_state != blocking_queue_success) {
			if (blocking_queue_state == blocking_queue_closed) {
				// Could not dequeue a task because the queue is empty and closed. End this routine.
				log_info("Threadpool worker is done.");
				break;
			} else if (blocking_queue_state == blocking_queue_timeout) {
				// Timed out. The timeout simply prevent deadlocks from happening; continue.
				log_debug("Thread pool worker timed out on blocking queue.");
				continue;
			} else {
				// Could not dequeue a task because of some other threading issue. Just continue.
				log_warn("Unable to dequeue element. Blocking queue returned %d. Continuing execution.", blocking_queue_state);
				continue;
			}
		}

		// Cast the task.
		threadpool_task_t* threadpool_task = uncasted_threadpool_task;

		// Make sure the func and func's future are set.
		if (threadpool_task->func == NULL || threadpool_task->func_future == NULL) {
			log_warn("Task element did not have any func nor future to compute. Continuing execution.");
			continue;
		}

		// Get the future.
		future_t* func_future = threadpool_task->func_future;
		
		// Acquire future mutex.
		pthread_mutex_lock(&func_future->mutex);

		// If the future is cancelled, skip it.
		if (func_future->cancel_token.is_cancelled) {
			func_future->is_computed = false;
			func_future->return_value = -1;
			func_future->out_value = NULL;
			
			// Release future mutex.
			pthread_cond_broadcast(&func_future->computed_condition);
			pthread_mutex_unlock(&func_future->mutex);
			continue;
		}

		// Execute the task's func. The future's value and state will be set then the future mutex will be unlocked.
		func_future->return_value = threadpool_task->func(func_future->cancel_token, threadpool_task->arguments, &func_future->out_value);

		// Notify threads waiting on this future.
		func_future->is_computed = true;
		pthread_cond_broadcast(&func_future->computed_condition);

		// Release future mutex.
		pthread_mutex_unlock(&func_future->mutex);

		// Destroy the task.
		free(threadpool_task);
	}

	return threadpool_success;
}