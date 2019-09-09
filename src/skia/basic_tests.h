#include <stdio.h>
#include <stdlib.h>

#include "../vgperf.h"
#include "common.h"

void skia_line_perform (options_t *opt, library_context_t* ctx)
{
    for (int i=0; i<opt->count; i++)
        skia_draw_shape(SHAPE_LINE, opt, ctx);
}

void skia_single_poly_perform (options_t *opt, library_context_t* ctx)
{
    /*int w = opt->width;
    int h = opt->height;

    float x = (float)rnd()/RAND_MAX * w;
    float y = (float)rnd()/RAND_MAX * h;

    randomize_color (ctx->ctx);
    vkvg_move_to(ctx->ctx, x, y);

    for (int i=0; i<opt->count; i++) {
        x = (float)rnd()/RAND_MAX * w;
        y = (float)rnd()/RAND_MAX * h;

        vkvg_line_to(ctx->ctx, x, y);
    }
    vkvg_draw(opt->drawMode, ctx->ctx);*/
}

void skia_rectangles_perform (options_t *opt, library_context_t* ctx)
{
    for (int i=0; i<opt->count; i++)
        skia_draw_shape(SHAPE_RECTANGLE, opt, ctx);
}

void skia_circles_perform (options_t *opt, library_context_t* ctx) {
    for (int i=0; i<opt->count; i++)
        skia_draw_shape(SHAPE_CIRCLE, opt, ctx);
}

void skia_stars_perform (options_t *opt, library_context_t* ctx) {
    for (int i=0; i<opt->count; i++)
        skia_draw_shape(SHAPE_STAR, opt, ctx);
}
