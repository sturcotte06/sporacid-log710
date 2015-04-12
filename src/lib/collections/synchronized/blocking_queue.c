#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include "../../logging/logging.h"
#include "../../threading/commons.h"
#include "../linkedlist.h"
#include "../queue.h"
#include "blocking_queue.h"

#define true 1
#define false 0

/// <summary>
/// Initializes a blocking queue.
/// </summary>
/// <param name="queue">The blocking queue to initialize. This cannot be null.</param>
/// <param name="options">The blocking queue options.</param>
/// <returns>The state enum value.</returns>
blocking_queue_state_t blocking_queue_init(blocking_queue_t* blocking_queue, blocking_queue_options_t options) {
	log_debug("Entering blocking_queue_init()");

	if (blocking_queue == NULL) {
		return blocking_queue_invalid_args;
	}

	int result;
	
	// Initialize the inner queue.
	queue_state_t state = queue_init(&blocking_queue->inner_queue);
	if (state != queue_success) return (blocking_queue_state_t) state;

	// Initialize the mutex.
	result = pthread_mutex_init(&blocking_queue->mutex, NULL);
	if (result != 0) return blocking_queue_synchronization_error;

	// Initialize the conditions.
	result = pthread_cond_init(&blocking_queue->element_enqueued_condition, NULL);
	if (result != 0) return blocking_queue_synchronization_error;
	result = pthread_cond_init(&blocking_queue->element_dequeued_condition, NULL);
	if (result != 0) return blocking_queue_synchronization_error;

	// Set flags values.
	blocking_queue->is_closed = false;
	blocking_queue->is_empty = true;
	blocking_queue->is_full = false;

	// Just put the options in the structure. Nothing else to do for now.
	blocking_queue->options = options;

	log_debug("Exiting blocking_queue_init()");
    return blocking_queue_success;
}

/// <summary>
/// Closes the blocking queue. A closed queue will not block thread waiting on elements to be enqueued.
/// </summary>
/// <param name="queue">The blocking queue to destroy. This cannot be null.</param>
/// <returns>The state enum value.</returns>
blocking_queue_state_t blocking_queue_close(blocking_queue_t* blocking_queue) {
	log_debug("Entering blocking_queue_close()");

	if (blocking_queue == NULL) {
		return blocking_queue_invalid_args;
	}

	int result;

	// Acquire mutex.
	result = pthread_mutex_lock(&blocking_queue->mutex);
	if (result != 0) return blocking_queue_cannot_acquire_lock;

	// Set the closing flag to true and notify all threads so they can unblock. 
	blocking_queue->is_closed = true;
	pthread_cond_broadcast(&blocking_queue->element_enqueued_condition);
	
	// Release mutex.
	pthread_mutex_unlock(&blocking_queue->mutex);

	log_debug("Exiting blocking_queue_close()");
    return blocking_queue_success;
}

/// <summary>
/// Destroys a blocking queue and all used memory. The queue structure does not belong to this module;
/// the callee has to deal with the structure memory itself.
/// </summary>
/// <param name="queue">The blocking queue to destroy. This cannot be null.</param>
/// <returns>The state enum value.</returns>
blocking_queue_state_t blocking_queue_destroy(blocking_queue_t* blocking_queue) {
	log_debug("Entering blocking_queue_destroy()");

	if (blocking_queue == NULL) {
		return blocking_queue_invalid_args;
	}

	int result;

	// Close the blocking queue before destroying it.
	blocking_queue_state_t blocking_queue_state = blocking_queue_close(blocking_queue);
	if (blocking_queue_state != blocking_queue_success) return blocking_queue_state;

	// Destroy the inner queue.
	queue_state_t queue_state = queue_destroy(&blocking_queue->inner_queue);
	if (queue_state != queue_success) return (blocking_queue_state_t) queue_state;

	// Destroy the mutex.
	result = pthread_mutex_destroy(&blocking_queue->mutex);
	if (result != 0) return blocking_queue_synchronization_error;

	// Destroy the blocking condition.
	result = pthread_cond_destroy(&blocking_queue->element_enqueued_condition);
	if (result != 0) return blocking_queue_synchronization_error;
	result = pthread_cond_destroy(&blocking_queue->element_dequeued_condition);
	if (result != 0) return blocking_queue_synchronization_error;

	log_debug("Exiting blocking_queue_destroy()");
    return blocking_queue_success;
}

/// <summary>
/// Enqueues an element in the queue. If the maximum length option was set, this could block.
/// </summary>
/// <param name="queue">The queue into which to enqueue. This cannot be null.</param>
/// <param name="element">The element to enqueue.</param>
/// <returns>The state enum value.</returns>
blocking_queue_state_t blocking_queue_enqueue(blocking_queue_t* blocking_queue, void* element) {
	log_debug("Entering blocking_queue_enqueue()");

	if (blocking_queue == NULL) {
		return blocking_queue_invalid_args;
	}

	int result;
	
	// Create absolute timeout.
	struct timespec absolute_timeout;
	timespec_from_timeout(blocking_queue->options.timeout, &absolute_timeout);
	
	// Acquire mutex.
	result = pthread_mutex_timedlock(&blocking_queue->mutex, &absolute_timeout);
	if (result != 0) return blocking_queue_cannot_acquire_lock;

	// If the maximum length option was set, wait until the queue is not full anymore.
	if (blocking_queue->options.maximum_length) {
		// Wait until the queue is not full anymore.
		while (blocking_queue->is_full) {
			result = pthread_cond_timedwait(&blocking_queue->element_dequeued_condition, &blocking_queue->mutex, &absolute_timeout);
			if (result != 0) {
				// Unlock mutex and return the result.
				pthread_mutex_unlock(&blocking_queue->mutex);
				log_error("Condition wait on element_dequeued_condition returned %d.", result);
				return result == ETIMEDOUT ? blocking_queue_timeout : blocking_queue_synchronization_error;
			}
		}
	}

	if (blocking_queue->options.priority_comparer != NULL) {
		// Handle priority. 
		// Since priorities cannot be handled in a standard queue, fallback to the inner linked list.
		int i_current = 0;
		node_t* current = blocking_queue->inner_queue.inner_list.head;
		while (current != NULL) {
			i_current++;
			if (current->next != NULL && blocking_queue->options.priority_comparer(current->element, element) > 0) {
				// Current element has less priority, i_current is the good index at which to insert.
				break;
			}

			// Move to next element.
			current = current->next;
		}

		// Insert the element at the good index.
		linkedlist_state_t state = linkedlist_add(&blocking_queue->inner_queue.inner_list, i_current, element);
		if (state != linkedlist_success) return (blocking_queue_state_t) state;
	} else {
		// Enqueue the element.
		queue_state_t state = queue_enqueue(&blocking_queue->inner_queue, element);
		if (state != queue_success) return (blocking_queue_state_t) state;
	}
	
	// If the queue was empty, reset the flag.
	if (blocking_queue->is_empty) {
		blocking_queue->is_empty = false;
	}

	// If the queue is now full, set the flag.
	if (blocking_queue->options.maximum_length && blocking_queue->inner_queue.inner_list.length == blocking_queue->options.maximum_length) {
		blocking_queue->is_full = true;
	}

	// Notify a waiting thread.
	pthread_cond_signal(&blocking_queue->element_enqueued_condition);

	// Release mutex.
	pthread_mutex_unlock(&blocking_queue->mutex);

	log_debug("Exiting blocking_queue_enqueue()");
    return blocking_queue_success;
}

/// <summary>
/// Dequeues an element from the blocking queue. If the queue is empty, this will block.
/// </summary>
/// <param name="queue">The blocking queue from which to dequeue. This cannot be null.</param>
/// <param name="element">The out parameter for the element.</param>
/// <returns>The state enum value.</returns>
blocking_queue_state_t blocking_queue_dequeue(blocking_queue_t* blocking_queue, void** element) {
	log_debug("Entering blocking_queue_dequeue()");

	if (blocking_queue == NULL) {
		return blocking_queue_invalid_args;
	}

	int result;

	// Create absolute timeout.
	struct timespec absolute_timeout;
	timespec_from_timeout(blocking_queue->options.timeout, &absolute_timeout);

	// Acquire mutex.
	result = pthread_mutex_timedlock(&blocking_queue->mutex, &absolute_timeout);
	if (result != 0) return blocking_queue_cannot_acquire_lock;

	// Wait until the queue is not empty anymore or is closing.
	while (blocking_queue->is_empty) {
		// Edge case: if the queue is closing, the queue cannot block anymore on empty queue.
		if (blocking_queue->is_closed) {
			*element = NULL;
			pthread_mutex_unlock(&blocking_queue->mutex);
			return blocking_queue_closed;
		}

		// Wait until queue is not empty anymore.
		result = pthread_cond_timedwait(&blocking_queue->element_enqueued_condition, &blocking_queue->mutex, &absolute_timeout);
		if (result != 0) {
			// Unlock mutex and return the result.
			pthread_mutex_unlock(&blocking_queue->mutex);
			log_error("Condition wait on element_enqueued_condition returned %d.", result);
			return result == ETIMEDOUT ? blocking_queue_timeout : blocking_queue_synchronization_error;
		}
	}
	
	// Dequeue an element.
	queue_state_t state = queue_dequeue(&blocking_queue->inner_queue, element);
	if (state != queue_success) return (blocking_queue_state_t) state;

	// If the maximum length option was set and if the queue was full, reset the flag.
	if (blocking_queue->is_full) {
		blocking_queue->is_full = false;
	}

	// If the queue is now empty, set the flag.
	if (blocking_queue->inner_queue.inner_list.length == 0) {
		blocking_queue->is_empty = true;
	}

	// Notify a waiting thread.
	pthread_cond_signal(&blocking_queue->element_dequeued_condition);

	// Release mutex.
	pthread_mutex_unlock(&blocking_queue->mutex);

	log_debug("Exiting blocking_queue_dequeue()");
    return blocking_queue_success;
}