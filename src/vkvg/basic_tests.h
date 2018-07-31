#include <stdio.h>
#include <stdlib.h>

#include "../vgperf.h"
#include "common.h"

void line_perform (options_t *opt, library_context_t* ctx)
{
    int w = opt->width;
    int h = opt->height;

    for (int i=0; i<opt->count; i++) {
        float x1 = (float)rnd()/RAND_MAX * w;
        float x2 = (float)rnd()/RAND_MAX * w;
        float y1 = (float)rnd()/RAND_MAX * h;
        float y2 = (float)rnd()/RAND_MAX * h;


        randomize_color (ctx->ctx);
        vkvg_move_to(ctx->ctx, x1, y1);
        vkvg_line_to(ctx->ctx, x2, y2);
        vkvg_stroke(ctx->ctx);
    }
}

void single_poly_perform (options_t *opt, library_context_t* ctx)
{
    int w = opt->width;
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
    vkvg_draw(opt->drawMode, ctx->ctx);
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

        vkvg_draw(opt->drawMode, ctx->ctx);
    }
}

void circles_perform (options_t *opt, library_context_t* ctx) {
    int w = opt->width;
    int h = opt->height;

    for (int i=0; i<opt->count; i++) {
        double xc = (double)rnd()/RAND_MAX * w;
        double yc = (double)rnd()/RAND_MAX * h;
        double r = (double)rnd()/RAND_MAX * MIN(w,h) / 4.0;

        randomize_color (ctx->ctx);

        vkvg_arc(ctx->ctx, xc, yc, r, 0, M_PI * 2.0);
        vkvg_draw(opt->drawMode,ctx->ctx);
    }
}

void stars_perform (options_t *opt, library_context_t* ctx) {
    int w = opt->width;
    int h = opt->height;

    for (int i=0; i<opt->count; i++) {
        float x = (float)rnd()/RAND_MAX * w;
        float y = (float)rnd()/RAND_MAX * h;
        float scale = (float)rnd()/RAND_MAX *0.5 + 0.15;

        vkvg_move_to (ctx->ctx, x+star_points[0][0]*scale, y+star_points[0][1]*scale);

        for (int s=1; s<11; s++)
            vkvg_line_to (ctx->ctx, x+star_points[s][0]*scale, y+star_points[s][1]*scale);

        vkvg_close_path (ctx->ctx);

        randomize_color (ctx->ctx);
        vkvg_draw(opt->drawMode,ctx->ctx);
    }

}
