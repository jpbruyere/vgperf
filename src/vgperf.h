#ifndef VGPERF_H
#define VGPERF_H

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
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

typedef enum _fill_type_t {
    FILL_TYPE_SOLID,
    FILL_TYPE_SURFACE,
    FILL_TYPE_LINEAR,
    FILL_TYPE_RADIAL,
} fill_type_t;

typedef enum _line_cap_t {
    LINE_CAP_BUTT,
    LINE_CAP_ROUND,
    LINE_CAP_SQUARE
} line_cap_t;

typedef enum _vkvg_line_join_t {
    LINE_JOIN_MITER,
    LINE_JOIN_ROUND,
    LINE_JOIN_BEVEL
} line_join_t;

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
    line_cap_t capStyle;
    line_join_t joinStyle;
    fill_type_t fillType;
    int present;
    int saveImgs;
    draw_mode_t drawMode;
    antialias_t antialias;
} options_t;

extern int star_points[11][2];

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
 * @brief store context and function pointers for each libraries.
 */
typedef struct _vgperf_context
{
    char* libName;
    LibCtx libCtx;//library context pointer, different for each libs with dedicated objs like device, etc.
    PFNinitLibrary init;//library init and surface creation
    PFNcleanupLibrary cleanup;//library cleanup
    PFNSaveImg saveImg;
    test_t* tests;//test array
    int testCount;
}vgperf_context_t;

void addTest (vgperf_context_t *ctx, const char *testName, void *pfnInit, void *pfnPerform, void *pfncleanup);

#endif // VGPERF_H
