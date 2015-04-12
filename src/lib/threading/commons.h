#ifndef LIB_THREADING_COMMONS_H
#define LIB_THREADING_COMMONS_H

// Type for a timeout of a blocking queue.
typedef unsigned int threading_timeout_t;

// Type for a cancellation token.
typedef struct threading_cancel_token_t {
	volatile unsigned int is_cancelled;
} threading_cancel_token_t;

// Timeout value for an infinite timeout.
extern const threading_timeout_t INFINITE;

/// <summary>
/// Gets a timespec object from a timeout object.
/// </summary>
/// <param name="timeout"></param>
/// <param name="timespec"></param>
/// <returns>The state enum value.</returns>
int timespec_from_timeout(threading_timeout_t timeout, struct timespec* timespec);

#endif