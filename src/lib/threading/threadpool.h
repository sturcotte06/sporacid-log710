#ifndef LIB_THREADING_THREADPOOL_H
#define LIB_THREADING_THREADPOOL_H

#include <pthread.h>
#include "future.h"
#include "commons.h"
#include "../collections/synchronized/blocking_queue.h"

// Function pointer for a function to execute on the thread pool.
typedef int (*threadpool_func_t)(threading_cancel_token_t cancel_token, void* arguments, void** out_value);

// Enum for the thread pool possible function states.
typedef enum threadpool_state_t {
	threadpool_success,
	threadpool_invalid_args,
	threadpool_future_error,
	threadpool_blocking_collection_error,
	threadpool_timeout,
	threadpool_thread_error
} threadpool_state_t;

// Enum for the thread pool possible priorities.
typedef enum threadpool_priority_t {
	very_low, low, normal, high, very_high
} threadpool_priority_t;

// Structure for a task to be executed in a thread pool.
typedef struct threadpool_task_t {
	threadpool_priority_t priority;
	threadpool_func_t func;
	future_t* func_future;
	void* arguments;
} threadpool_task_t;

// Structure for the options of a thread pool.
typedef struct threadpool_options_t {
	char* threadpool_name;
	unsigned int threadpool_size;
	threading_timeout_t timeout;
} threadpool_options_t;

// Structure for the thread pool.
typedef struct threadpool_t {
	threadpool_options_t threadpool_options;
	pthread_t* available_threads;
	blocking_queue_t task_blocking_queue;
} threadpool_t;

/// <summary>
/// Initializes a thread pool.
/// </summary>
/// <param name="tpid">Struct in which to put the created thread pool.</param>
/// <param name="options">Options for the threadpool.</param>
/// <returns>The state enum value.</returns>
threadpool_state_t threadpool_init(threadpool_t* threadpool, threadpool_options_t options);

/// <summary>
/// Closes the given thread pool. If the timeout is reached, the thread pool will be closed by force.
/// </summary>
/// <param name="threadpool">Thread pool to close.</param>
/// <returns>The state enum value.</returns>
threadpool_state_t threadpool_close(threadpool_t* threadpool);

/// <summary>
/// Destroys the thread pool with the given id.
/// </summary>
/// <param name="threadpool">Thread pool to destroy.</param>
/// <returns>The state enum value.</returns>
threadpool_state_t threadpool_destroy(threadpool_t* threadpool);

/// <summary>
/// Submits an action with a return value into the thread pool.
/// The given future structure will be modified and waiting on its mutex will wait for the result.
/// </summary>
/// <param name="threadpool">Thread pool in which to submit.</param>
/// <param name="func">The action with a return value to execute.</param>
/// <param name="arguments">The arguments of the action.</param>
/// <param name="future">The future structure that holds the return value.</param>
/// <returns>The state enum value.</returns>
threadpool_state_t threadpool_submit_task(threadpool_t* threadpool, threadpool_func_t func, void* arguments, future_t* future);

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
threadpool_state_t threadpool_submit_priorized_task(threadpool_t* threadpool, threadpool_func_t func, void* arguments, future_t* future, threadpool_priority_t priority);

#endif