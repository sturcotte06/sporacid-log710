#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "logging.h"
#include "tests.h"

// Constant for the error number when trying to starts a null test results.
const int NULL_RESULTS_ERRNO = 1;

/// <summary>
/// Starts a unit test suite. The results will be initialized and some reference metrics will be saved.
/// </summary>
/// <param name="results">The results to initialize. This cannot be null.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int tests_start(testresults_t* results, FILE* stream) {
    log_debug("Entering tests_start().");
    if (results == NULL) {
        // Queue cannot be null.
        return NULL_RESULTS_ERRNO;
    }
    
    if (stream == NULL) {
        // Default to stdout.
        stream = stdout;
    }
    
    results->passedcnt = 0;
    results->failcnt = 0;
    results->timestamp_start = (unsigned int) time(NULL);
    results->timestamp_end = 0;
    results->stream = stream;
    
    log_debug("Exiting tests_start().");
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Asserts a condition. If the assertion returns false, the message will be streamed into results' stdout.
/// </summary>
/// <param name="results">The results of the tests. This cannot be null.</param>
/// <param name="condition">The condition to assert.</param>
/// <param name="format">The message, with its format, if assertion fails.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int tests_assert(testresults_t* results, int condition, const char* format, ...) {
    log_debug("Entering tests_assert().");
    if (results == NULL) {
        // Queue cannot be null.
        return NULL_RESULTS_ERRNO;
    }
    
    if (!condition) {
        flog_error(results->stream, format);
        results->failcnt++;
    } else {
        results->passedcnt++;
    }
    
    log_debug("Exiting tests_assert().");
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Ends a unit test suite. Reference metrics will be saved.
/// </summary>
/// <param name="results">The results of the tests. This cannot be null.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int tests_end(testresults_t* results) {
    log_debug("Entering tests_end().");
    if (results == NULL) {
        // Queue cannot be null.
        return NULL_RESULTS_ERRNO;
    }
    
    // Save end timestamp and log a summary.
    results->timestamp_end = (unsigned int) time(NULL);
    flog_info(results->stream, "Tests passed: %d, Tests failed: %d, Time taken: %ds.",
        results->passedcnt, results->failcnt, (unsigned int) difftime(results->timestamp_start, results->timestamp_end));
    
    log_debug("Exiting tests_end().");
    return SUCCESSFUL_EXEC;
}