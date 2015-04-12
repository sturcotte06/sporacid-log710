#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../logging/logging.h"
#include "commons.h"

// Timeout value for an infinite timeout.
const threading_timeout_t INFINITE = 0;

/// <summary>
/// Gets a timespec object from a timeout object.
/// </summary>
/// <param name="timeout"></param>
/// <param name="timespec"></param>
/// <returns>The state enum value.</returns>
int timespec_from_timeout(threading_timeout_t timeout, struct timespec* timespec) {
	log_debug("Entering timespec_from_timeout()");
	if (timeout == INFINITE) {
		// Just set all bits to 1s. We better not reach that date...
		memset(timespec, 0xff, sizeof(struct timespec));
	} else {
		// Timespec timeout are absolute.
		// Milliseconds = 10^-3 seconds, Nanoseconds = 10^-9 seconds.
		// With that in mind, add the number of nanoseconds of now to the number of nanoseconds from the timeout.
		// This will give us the absolute time at which the timeout occurs.
		time(&timespec->tv_sec);
		timespec->tv_sec += (timeout / 1000);
		timespec->tv_nsec = (timeout % 1000) * 1000000;
	}
	
	log_debug("Exiting timespec_from_timeout()");
    return 0;
}