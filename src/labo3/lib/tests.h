#ifndef LIB_TESTS_H
#define LIB_TESTS_H

// Constant for a successful execution.
extern const int SUCCESSFUL_EXEC;

// Constant for the error number when trying to starts a null test results.
extern const int NULL_RESULTS_ERRNO;

// Structure for a test results.
typedef struct testresults_t testresults_t;
struct testresults_t {
    int passedcnt;
    int failcnt;
    unsigned int timestamp_start;
    unsigned int timestamp_end;
    FILE* stream;
};

/// <summary>
/// Starts a unit test. The results will be initialized and some reference metrics will be saved.
/// </summary>
/// <param name="results">The results to initialize. This cannot be null.</param>
/// <param name="stream">The output of the tests.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int tests_start(testresults_t* results, FILE* stream);

/// <summary>
/// Asserts a condition. If the assertion returns false, the message will be streamed into results' stdout.
/// </summary>
/// <param name="results">The results of the tests. This cannot be null.</param>
/// <param name="condition">The condition to assert.</param>
/// <param name="format">The message if assertion fails.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int tests_assert(testresults_t* results, int condition, const char* format, ...);

/// <summary>
/// Ends a unit test. Reference metrics will be saved.
/// </summary>
/// <param name="results">The results of the tests. This cannot be null.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int tests_end(testresults_t* results);

#endif