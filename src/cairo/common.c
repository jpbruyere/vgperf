
#include "common.h"

#include "basic_tests.h"

void ca_randomize_color (cairo_t *ctx) {
    cairo_set_source_rgba(ctx,
        (double)rnd()/RAND_MAX,
        (double)rnd()/RAND_MAX,
        (double)rnd()/RAND_MAX,
        (double)rnd()/RAND_MAX
    );
}

void ca_draw (draw_mode_t drawMode, cairo_t *ctx) {
    switch (drawMode) {
    case DM_BOTH:
        cairo_fill_preserve(ctx);
        ca_randomize_color (ctx);
        cairo_stroke(ctx);
        break;
    case DM_FILL:
        cairo_fill(ctx);
        break;
    case DM_STROKE:
        cairo_stroke(ctx);
        break;
    }
}

/**
 * @brief init library once at startup
 * @param global options
 * @return a pointer to the library test context
 */
library_context_t* ca_initLibrary(options_t* opt) {
    library_context_t* ctx = (library_context_t*)calloc(1, sizeof(library_context_t));

    ctx->surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, opt->width, opt->height);

    return ctx;
}
/**
 * @brief cleanup Library and release test context pointer
 * @param library test context
 */
void ca_cleanupLibrary (library_context_t* ctx) {
    cairo_surface_destroy (ctx->surf);

    free(ctx);
}
void ca_saveImg (library_context_t* ctx, const char* fileName) {
    cairo_surface_write_to_png (ctx->surf, fileName);
}
/**
 * @brief common test init func for simple tests, context creation and settings
 * @param global options
 * @param cairo library context
 */
void ca_initTest(options_t* opt, library_context_t* ctx) {
    ctx->ctx = cairo_create(ctx->surf);
    switch (opt->antialias) {
    case ANTIALIAS_NONE:
        cairo_set_antialias (ctx->ctx, CAIRO_ANTIALIAS_NONE);
        break;
    case ANTIALIAS_DEFAULT:
        cairo_set_antialias (ctx->ctx, CAIRO_ANTIALIAS_DEFAULT);
        break;
    case ANTIALIAS_FAST:
        cairo_set_antialias (ctx->ctx, CAIRO_ANTIALIAS_FAST);
        break;
    case ANTIALIAS_GOOD:
        cairo_set_antialias (ctx->ctx, CAIRO_ANTIALIAS_GOOD);
        break;
    case ANTIALIAS_BEST:
        cairo_set_antialias (ctx->ctx, CAIRO_ANTIALIAS_BEST);
        break;
    }
    switch (opt->capStyle) {
    case LINE_CAP_BUTT:
        cairo_set_line_cap(ctx->ctx, CAIRO_LINE_CAP_BUTT);
        break;
    case LINE_CAP_ROUND:
        cairo_set_line_cap(ctx->ctx, CAIRO_LINE_CAP_ROUND);
        break;
    case LINE_CAP_SQUARE:
        cairo_set_line_cap(ctx->ctx, CAIRO_LINE_CAP_SQUARE);
        break;
    }
    switch (opt->joinStyle) {
    case LINE_JOIN_MITER:
        cairo_set_line_join(ctx->ctx, CAIRO_LINE_JOIN_MITER);
        break;
    case LINE_JOIN_BEVEL:
        cairo_set_line_join(ctx->ctx, CAIRO_LINE_JOIN_BEVEL);
        break;
    case LINE_JOIN_ROUND:
        cairo_set_line_join(ctx->ctx, CAIRO_LINE_JOIN_ROUND);
        break;
    }

    //clear surface
    cairo_set_operator(ctx->ctx, CAIRO_OPERATOR_CLEAR);
    cairo_paint(ctx->ctx);
    cairo_set_operator(ctx->ctx, CAIRO_OPERATOR_OVER);

    cairo_set_line_width (ctx->ctx, opt->lineWidth);
}
void ca_cleanupTest (options_t* opt, library_context_t* ctx) {
    cairo_destroy(ctx->ctx);
}


void init_cairo_tests (vgperf_context_t* ctx) {
    ctx->libName = "cairo";
    ctx->init = (PFNinitLibrary) ca_initLibrary;
    ctx->cleanup = (PFNcleanupLibrary) ca_cleanupLibrary;
    ctx->saveImg = (PFNSaveImg) ca_saveImg;
    ctx->testCount = 0;
    ctx->tests = (test_t*)malloc(0);

    addTest(ctx, "lines stroke", ca_initTest, ca_line_perform, ca_cleanupTest);
    addTest(ctx, "rectangles", ca_initTest, ca_rectangles_perform, ca_cleanupTest);
    addTest(ctx, "circles", ca_initTest, ca_circles_perform, ca_cleanupTest);
    addTest(ctx, "stars", ca_initTest, ca_stars_perform, ca_cleanupTest);
    //addTest(ctx, "single poly", ca_initTest, ca_single_poly_perform, ca_cleanupTest);
}
