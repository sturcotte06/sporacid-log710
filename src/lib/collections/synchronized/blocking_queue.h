#ifndef LIB_COLLECTIONS_SYNCHRONIZED_BLOCKING_QUEUE_H
#define LIB_COLLECTIONS_SYNCHRONIZED_BLOCKING_QUEUE_H

#include <pthread.h>
#include "../../threading/commons.h"
#include "../queue.h"

// Function that compares two stream and return a -1 if comparee < comparand, 0 if comparee = comparand and 1 if comparee > comparand.
typedef int (*priority_comparer_t)(void* comparee, void* comparand);

// Enum for the blocking queue possible function states.
typedef enum blocking_queue_state_t {
	blocking_queue_success,
	blocking_queue_invalid_args,
	blocking_queue_out_of_bounds,
	blocking_queue_empty,
	blocking_queue_closed,
	blocking_queue_timeout,
	blocking_queue_cannot_acquire_lock,
	blocking_queue_synchronization_error
} blocking_queue_state_t;

// Sturcture for the options of a blocking queue.
typedef struct blocking_queue_options_t {
	// If equals to zero, the maximum length is infinite.
	unsigned int maximum_length;
	// If null, no comparison for priority will be made.
	priority_comparer_t priority_comparer;
	// The timeout to use for blocking operations.
	threading_timeout_t timeout;
} blocking_queue_options_t;

// Structure for a blocking queue.
typedef struct blocking_queue_t {
	queue_t inner_queue;
	pthread_mutex_t mutex;
	pthread_cond_t element_enqueued_condition;
	pthread_cond_t element_dequeued_condition;
	blocking_queue_options_t options;
	volatile unsigned int is_closed:1;
	volatile unsigned int is_empty:1;
	volatile unsigned int is_full:1;
} blocking_queue_t;

/// <summary>
/// Initializes a blocking queue.
/// </summary>
/// <param name="queue">The blocking queue to initialize. This cannot be null.</param>
/// <param name="options">The blocking queue options.</param>
/// <returns>The state enum value.</returns>
blocking_queue_state_t blocking_queue_init(blocking_queue_t* blocking_queue, blocking_queue_options_t options);

/// <summary>
/// Closes the blocking queue. A closed queue will not block thread waiting on elements to be enqueued.
/// </summary>
/// <param name="queue">The blocking queue to destroy. This cannot be null.</param>
/// <returns>The state enum value.</returns>
blocking_queue_state_t blocking_queue_close(blocking_queue_t* blocking_queue);

/// <summary>
/// Destroys a blocking queue and all used memory. The queue structure does not belong to this module;
/// the callee has to deal with the structure memory itself.
/// </summary>
/// <param name="queue">The blocking queue to destroy. This cannot be null.</param>
/// <returns>The state enum value.</returns>
blocking_queue_state_t blocking_queue_destroy(blocking_queue_t* blocking_queue);

/// <summary>
/// Enqueues an element in the queue. If the maximum length option was set, this could block.
/// </summary>
/// <param name="queue">The queue into which to enqueue. This cannot be null.</param>
/// <param name="element">The element to enqueue.</param>
/// <returns>The state enum value.</returns>
blocking_queue_state_t blocking_queue_enqueue(blocking_queue_t* blocking_queue, void* element);

/// <summary>
/// Dequeues an element from the blocking queue. If the queue is empty, this will block.
/// </summary>
/// <param name="queue">The blocking queue from which to dequeue. This cannot be null.</param>
/// <param name="element">The out parameter for the element.</param>
/// <returns>The state enum value.</returns>
blocking_queue_state_t blocking_queue_dequeue(blocking_queue_t* blocking_queue, void** element);

// /// <summary>
// /// Enqueues an element in the queue.
// /// If the maximum length option was set, instead of blocking, the success flag would be set to false.
// /// </summary>
// /// <param name="queue">The blocking queue into which to enqueue. This cannot be null.</param>
// /// <param name="element">The element to enqueue.</param>
// /// <param name="success_flag">The success flag, if the element could not be enqueued without blocking.</param>
// /// <returns>The state enum value.</returns>
// int blocking_queue_try_enqueue(blocking_queue_t* blocking_queue, void* element, unsigned int* success_flag);

// /// <summary>
// /// Dequeues an element from the blocking queue. If the queue is empty, the success flag will be set to false.
// /// </summary>
// /// <param name="queue">The blocking queue from which to dequeue. This cannot be null.</param>
// /// <param name="element">The out parameter for the element.</param>
// /// <param name="success_flag">The success flag, if an element could not be dequeued without blocking.</param>
// /// <returns>The state enum value.</returns>
// int blocking_queue_try_dequeue(blocking_queue_t* blocking_queue, void** element, unsigned int* success_flag);

#endif