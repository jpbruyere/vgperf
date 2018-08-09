#include <stdio.h>
#include <stdlib.h>

#include "../vgperf.h"
#include "common.h"

void ca_line_perform (options_t *opt, library_context_t* ctx)
{
    for (int i=0; i<opt->count; i++)
        ca_draw_shape(SHAPE_LINE, opt, ctx);
}

void ca_rectangles_perform (options_t *opt, library_context_t* ctx)
{
    for (int i=0; i<opt->count; i++)
        ca_draw_shape(SHAPE_RECTANGLE, opt, ctx);
}

void ca_circles_perform (options_t *opt, library_context_t* ctx) {
    for (int i=0; i<opt->count; i++)
        ca_draw_shape(SHAPE_CIRCLE, opt, ctx);
}

void ca_stars_perform (options_t *opt, library_context_t* ctx) {
    for (int i=0; i<opt->count; i++)
        ca_draw_shape(SHAPE_STAR, opt, ctx);
}

void ca_single_poly_perform (options_t *opt, library_context_t* ctx)
{
    int w = opt->width;
    int h = opt->height;

    double x = (double)rnd()/RAND_MAX * w;
    double y = (double)rnd()/RAND_MAX * h;

    ca_randomize_color (ctx->ctx);
    cairo_move_to(ctx->ctx, x, y);

    for (int i=0; i<opt->count; i++) {
        x = (double)rnd()/RAND_MAX * w;
        y = (double)rnd()/RAND_MAX * h;

        cairo_line_to(ctx->ctx, x, y);
    }
    ca_draw(opt->drawMode, ctx->ctx);
}

