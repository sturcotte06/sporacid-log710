#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include "../logging/logging.h"
#include "commons.h"
#include "future.h"

#define true 1
#define false 0

/// <summary>
/// Initializes a future.
/// </summary>
/// <param name="future">The future to initialize.</param>
/// <returns>The state enum value.</returns>
future_state_t future_init(future_t* future) {
	if (future == NULL) {
		return future_invalid_args;
	}

	log_debug("Entering future_init().");

	// Create a new future.
	future->is_computed = false;
	future->cancel_token.is_cancelled = false;
	future->return_value = -1;
	future->out_value = NULL;

	// Initialize the mutex.
	int result = pthread_mutex_init(&future->mutex, NULL);
	if (result != 0) return future_synchronization_error;

	// Initialize the future condition.
	result = pthread_cond_init(&future->computed_condition, NULL);
	if (result != 0) return future_synchronization_error;

	log_debug("Exiting future_init().");
    return future_success;
}

/// <summary>
/// Waits on a future to be computed or cancelled.
/// </summary>
/// <param name="future">The future to wait on.</param>
/// <param name="timeout">The timeout to use for wait operation.</param>
/// <returns>The state enum value.</returns>
future_state_t future_wait(future_t* future, threading_timeout_t timeout) {
	if (future == NULL) {
		return future_invalid_args;
	}

	log_debug("Entering future_wait().");

	// Create absolute timeout.
	struct timespec absolute_timeout;
	timespec_from_timeout(timeout, &absolute_timeout);

	// Acquire mutex.
	int result = pthread_mutex_timedlock(&future->mutex, &absolute_timeout);
	if (result != 0) return future_cannot_acquire_lock;

	while (!future->is_computed && !future->cancel_token.is_cancelled) {
		log_debug("Waiting on future...");
		result = pthread_cond_timedwait(&future->computed_condition, &future->mutex, &absolute_timeout);

		if (result != 0) {
			// Unlock mutex and return the result.
			pthread_mutex_unlock(&future->mutex);

			log_error("Condition wait on computed_condition returned %d.", result);
			return result == ETIMEDOUT ? future_timeout : future_synchronization_error;
		}

		log_debug("Finished waiting. Computed: %d, Cancelled: %d.", future->is_computed, future->cancel_token.is_cancelled);
	}

	// Release mutex.
	pthread_mutex_unlock(&future->mutex);

	log_debug("Exiting future_wait().");
    return future->cancel_token.is_cancelled ? future_cancelled : future_success;
}

/// <summary>
/// Destroys a future.
/// The future value will not be freed; it is the responsibility of the caller.
/// </summary>
/// <param name="future">The future to destroy.</param>
/// <returns>The state enum value.</returns>
future_state_t future_destroy(future_t* future) {
	if (future == NULL) {
		return future_invalid_args;
	}

	log_debug("Entering future_destroy().");
	
	// Destroy the mutex condition.
	pthread_mutex_destroy(&future->mutex);

	// Destroy the future condition.
	pthread_cond_destroy(&future->computed_condition);

	log_debug("Exiting future_destroy().");
    return future_success;
}