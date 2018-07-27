
#include "common.h"

#include "line.h"
#include "rect.h"
#include "rectangles.h"

void ca_randomize_color (cairo_t *ctx) {
    cairo_set_source_rgba(ctx,
        (double)rnd()/RAND_MAX,
        (double)rnd()/RAND_MAX,
        (double)rnd()/RAND_MAX,
        (double)rnd()/RAND_MAX
    );
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

void init_cairo_tests (test_context_t* ctx) {
    ctx->libName = "cairo";
    ctx->init = ca_initLibrary;
    ctx->cleanup = ca_cleanupLibrary;

    ctx->tests [TID_lines].init = ca_line_init;
    ctx->tests [TID_lines].perform = ca_line_perform;
    ctx->tests [TID_lines].cleanup = ca_line_cleanup;


    ctx->tests [TID_rects].init = ca_rect_init;
    ctx->tests [TID_rects].perform = ca_rect_perform;
    ctx->tests [TID_rects].cleanup = ca_rect_cleanup;

    ctx->tests [TID_rectangles].init = ca_rectangles_init;
    ctx->tests [TID_rectangles].perform = ca_rectangles_perform;
    ctx->tests [TID_rectangles].cleanup = ca_rectangles_cleanup;
}
