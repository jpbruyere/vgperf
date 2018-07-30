#include <stdio.h>
#include <stdlib.h>

#include "../vgperf.h"
#include "common.h"

void ca_line_perform (options_t *opt, library_context_t* ctx)
{
    int w = opt->width;
    int h = opt->height;

    for (int i=0; i<opt->count; i++) {
        double x1 = (double)rnd()/RAND_MAX * w;
        double x2 = (double)rnd()/RAND_MAX * w;
        double y1 = (double)rnd()/RAND_MAX * h;
        double y2 = (double)rnd()/RAND_MAX * h;


        ca_randomize_color (ctx->ctx);
        cairo_move_to(ctx->ctx, x1, y1);
        cairo_line_to(ctx->ctx, x2, y2);
        cairo_stroke(ctx->ctx);
    }
}

void ca_rect_perform (options_t *opt, library_context_t* ctx)
{
    int w = opt->width;
    int h = opt->height;

    for (int i=0; i<opt->count; i++) {
        double x1 = (double)rnd()/RAND_MAX * w;
        double x2 = (double)rnd()/RAND_MAX * w;
        double y1 = (double)rnd()/RAND_MAX * h;
        double y2 = (double)rnd()/RAND_MAX * h;


        ca_randomize_color (ctx->ctx);
        cairo_move_to(ctx->ctx, x1, y1);
        cairo_line_to(ctx->ctx, x2, y2);
        cairo_fill(ctx->ctx);
    }
}

void ca_rectangles_perform (options_t *opt, library_context_t* ctx)
{
    double w, h, x, y;

    for (int i=0; i<opt->count; i++) {
        ca_randomize_color (ctx->ctx);

        w = trunc( (0.5*(double)opt->width*rnd())/RAND_MAX ) + 1;
        h = trunc( (0.5*(double)opt->height*rnd())/RAND_MAX ) + 1;
        x = trunc( (0.5*(double)opt->width*rnd())/RAND_MAX );
        y = trunc( (0.5*(double)opt->height*rnd())/RAND_MAX );
        cairo_rectangle(ctx->ctx, x+1, y+1, w, h);

        ca_draw(opt->drawMode, ctx->ctx);


    }
}

void ca_circles_perform (options_t *opt, library_context_t* ctx) {
    int w = opt->width;
    int h = opt->height;

    for (int i=0; i<opt->count; i++) {
        double xc = (double)rnd()/RAND_MAX * w;
        double yc = (double)rnd()/RAND_MAX * h;
        double r = (double)rnd()/RAND_MAX * MIN(w,h) / 2.0;

        ca_randomize_color (ctx->ctx);

        cairo_arc(ctx->ctx, xc, yc, r, 0, M_PI * 2.0);
        ca_draw(opt->drawMode,ctx->ctx);
    }

}
