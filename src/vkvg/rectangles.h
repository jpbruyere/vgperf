#include <stdio.h>
#include <stdlib.h>

#include "../vgperf.h"
#include "common.h"

void rectangles_init (options_t *opt, library_context_t* ctx)
{
    ctx->ctx = vkvg_create(ctx->surf);
    vkvg_set_line_width (ctx->ctx, 1);
}

void rectangles_perform (options_t *opt, library_context_t* ctx)
{
    double w, h, x, y;

    for (int i=0; i<opt->count; i++) {
        randomize_color (ctx->ctx);

        w = trunc( (0.5f*(float)opt->width*rnd())/RAND_MAX ) + 1;
        h = trunc( (0.5f*(float)opt->height*rnd())/RAND_MAX ) + 1;
        x = trunc( (0.5f*(float)opt->width*rnd())/RAND_MAX );
        y = trunc( (0.5f*(float)opt->height*rnd())/RAND_MAX );
        vkvg_rectangle(ctx->ctx, x+1, y+1, w, h);

        vkvg_stroke (ctx->ctx);
    }
}

void rectangles_cleanup (options_t *opt, library_context_t* ctx) {
    vkvg_destroy(ctx->ctx);
}
