#ifndef VGPERF_H
#define VGPERF_H

#include <stdlib.h>
#include <float.h>
#include <math.h>
#include "rnd.h"

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

#ifndef MIN
# define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
# define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

#define TESTS_COUNT 4

typedef enum _draw_mode_t {
    DM_FILL = 0x1,
    DM_STROKE = 0x2,
    DM_BOTH = 0x3
}draw_mode_t;

typedef enum _antialias {
    ANTIALIAS_NONE,
    ANTIALIAS_DEFAULT,
    ANTIALIAS_FAST,
    ANTIALIAS_GOOD,
    ANTIALIAS_BEST
} antialias_t;

typedef struct _results_t {
    double run_min;
    double run_max;
    double avg_time;
    double median_time;
    double std_deriv;
} results_t;

typedef struct _options_t {
    int iterations;
    int count;
    int width;
    int height;
    int lineWidth;
    int present;
    int saveImgs;
    draw_mode_t drawMode;
    antialias_t antialias;
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
 * @brief save surface on disk
 */
typedef void (*PFNSaveImg) (LibCtx ctx, const char* fileName);

/**
 * @brief store each test functions per library and results
 */
typedef struct _test {
    char* test_name;
    PFNtest init;//pre-test init, not included in duration measure (may be null)
    PFNtest perform;//all processing here is included in time measurement
    PFNtest cleanup;//post-test cleanup (may be null)
    results_t results;
}test_t;

/**
 * @brief store context and function pointers for each libraries
 */
typedef struct _test_context
{
    char* libName;
    LibCtx libCtx;//library context
    PFNinitLibrary init;//library init and surface creation
    PFNcleanupLibrary cleanup;//library cleanup
    PFNtest initTest; //common init for all tests, run once before each test,
    PFNtest cleanupTest; //common cleanup for tests
    PFNSaveImg saveImg;

    test_t* tests;//test array
    int testCount;
}test_context_t;

void addTest (test_context_t *ctx, const char *testName, PFNtest pfnInit, PFNtest pfnPerform, PFNtest pfncleanup);

#endif // VGPERF_H
