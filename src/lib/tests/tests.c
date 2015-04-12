#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "../logging/logging.h"
#include "tests.h"

/// <summary>
/// Starts a unit test. The results will be initialized and some reference metrics will be saved.
/// </summary>
/// <param name="results">The results to initialize. This cannot be null.</param>
/// <param name="output_stream">The output of the tests.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
tests_state_t tests_start(testresults_t* results, FILE* output_stream) {
    log_debug("Entering tests_start().");
    if (results == NULL) {
        return tests_invalid_args;
    }
    
    if (output_stream == NULL) {
        // Default to stdout.
        output_stream = stdout;
    }
    
	// Initialize the results.
    results->passed_test_count = 0;
    results->failed_test_count = 0;
    results->timestamp_start = time(NULL);
    results->timestamp_end = 0;
    results->output_stream = output_stream;
    
    log_debug("Exiting tests_start().");
    return tests_success;
}

/// <summary>
/// Asserts a condition. If the assertion returns false, the message will be streamed into results' stdout.
/// </summary>
/// <param name="results">The results of the tests. This cannot be null.</param>
/// <param name="condition">The condition to assert.</param>
/// <param name="format">The message, with its format, if assertion fails.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
tests_state_t tests_assert(testresults_t* results, int condition, const char* format, ...) {
    log_debug("Entering tests_assert().");
    if (results == NULL) {
        return tests_invalid_args;
    }
    
    if (!condition) {
		va_list args;
		va_start(args, format);
        vflog_format(results->output_stream, ERROR_LVL, format, args);
		va_end(args);
        results->failed_test_count++;
    } else {
        results->passed_test_count++;
    }
    
    log_debug("Exiting tests_assert().");
    return condition ? tests_assert_true : tests_assert_false;
}

/// <summary>
/// Ends a unit test suite. Reference metrics will be saved.
/// </summary>
/// <param name="results">The results of the tests. This cannot be null.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
tests_state_t tests_end(testresults_t* results) {
    log_debug("Entering tests_end().");
    if (results == NULL) {
        return tests_invalid_args;
    }
    
    // Save end timestamp and log a summary.
    results->timestamp_end = time(NULL);
    flog_info(results->output_stream, "Tests passed: %u, Tests failed: %u, Time taken: %.2f sec.",
        results->passed_test_count, results->failed_test_count, difftime(results->timestamp_end, results->timestamp_start));
    
    log_debug("Exiting tests_end().");
    return tests_success;
}