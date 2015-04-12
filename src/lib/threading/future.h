#ifndef LIB_THREADING_FUTURE_H
#define LIB_THREADING_FUTURE_H

#include <pthread.h>
#include "commons.h"

// Enum for the thread pool possible function states.
typedef enum future_state_t {
	future_success,
	future_invalid_args,
	future_cancelled,
	future_cannot_acquire_lock,
	future_timeout,
	future_synchronization_error
} future_state_t;

// Structure for an asynchronous future value.
typedef struct future_t {
	pthread_mutex_t mutex;
	pthread_cond_t computed_condition;
	threading_cancel_token_t cancel_token;
	volatile unsigned int is_computed;
	int return_value;
	void* out_value;
} future_t;

/// <summary>
/// Initializes a future.
/// </summary>
/// <param name="future">The future to initialize.</param>
/// <returns>The state enum value.</returns>
future_state_t future_init(future_t* future);

/// <summary>
/// Waits on a future to be computed or cancelled.
/// </summary>
/// <param name="future">The future to wait on.</param>
/// <param name="timeout">The timeout to use for wait operation.</param>
/// <returns>The state enum value.</returns>
future_state_t future_wait(future_t* future, threading_timeout_t timeout);

/// <summary>
/// Destroys a future.
/// The future value will not be freed; it is the responsibility of the caller.
/// </summary>
/// <param name="future">The future to destroy.</param>
/// <returns>The state enum value.</returns>
future_state_t future_destroy(future_t* future);

#endif