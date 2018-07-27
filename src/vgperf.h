#ifndef VGPERF_H
#define VGPERF_H

#include <stdlib.h>
#include <float.h>
#include "rnd.h"

#define TESTS_COUNT 1

enum DrawMode {
    DM_FILL = 0x1,
    DM_STROKE = 0x2,
    DM_BOTH = 0x3
};

enum TestID {
    TID_lines,
    TID_circles,
};

typedef struct _results {
    double run_min;
    double run_max;
    double avg_time;
    double median_time;
    double std_deriv;
} results_t;

typedef struct _options {
    char* test_name;
    int iterations;
    int count;
    int width;
    int height;
    enum DrawMode drawMode;
} options_t;

/**
 * @brief Pointer to dedicated struct for each library for storing device ctx, etc..
 */
typedef void* LibCtx;

/**
 * @brief func pointer for library initialization
 */
typedef LibCtx (*PFNinitLibrary) (options_t* opt);
/**
 * @brief func pointer for library cleanup
 */
typedef void (*PFNcleanupLibrary) (LibCtx ctx);

/**
 * @brief all func in each test has the same signature
 */
typedef void (*PFNtest) (options_t* opt, LibCtx ctx);

/**
 * @brief store each test functions per library and results
 */
typedef struct _test {
    PFNtest init;
    PFNtest perform;
    PFNtest cleanup;
    results_t results;
}test_t;

/**
 * @brief store context and function pointers for each libraries
 */
typedef struct _test_context
{
    char* libName;
    LibCtx libCtx;
    PFNinitLibrary init;
    PFNcleanupLibrary cleanup;
    test_t tests [TESTS_COUNT];
}test_context_t;



#endif // VGPERF_H
