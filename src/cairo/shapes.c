
#include "shapes.h"

void ca_randomize_color (cairo_t *ctx) {
    cairo_set_source_rgba(ctx,
        (double)rnd()/RAND_MAX,
        (double)rnd()/RAND_MAX,
        (double)rnd()/RAND_MAX,
        (double)rnd()/RAND_MAX
    );
}

void ca_draw_shape (shape_t shape, options_t *opt, library_context_t* ctx) {
    int w = opt->width;
    int h = opt->height;

    double x, y, z, v;

    ca_randomize_color (ctx->ctx);

    switch (shape) {
    case SHAPE_LINE:
        x = (double)rnd()/RAND_MAX * w;
        y = (double)rnd()/RAND_MAX * h;
        z = (double)rnd()/RAND_MAX * w;
        v = (double)rnd()/RAND_MAX * h;

        cairo_move_to(ctx->ctx, x, y);
        cairo_line_to(ctx->ctx, z, v);
        cairo_stroke(ctx->ctx);
        break;
    case SHAPE_RECTANGLE:
        x = trunc( (0.5*(double)opt->width*rnd())/RAND_MAX );
        y = trunc( (0.5*(double)opt->height*rnd())/RAND_MAX );
        z = trunc( (0.5*(double)opt->width*rnd())/RAND_MAX ) + 1;
        v = trunc( (0.5*(double)opt->height*rnd())/RAND_MAX ) + 1;

        cairo_rectangle(ctx->ctx, x+1, y+1, z, v);

        ca_draw(opt->drawMode, ctx->ctx);
        break;
    case SHAPE_ROUNDED_RECTANGLE:
        break;
    case SHAPE_CIRCLE:
        x = (double)rnd()/RAND_MAX * w;
        y = (double)rnd()/RAND_MAX * h;
        v = (double)rnd()/RAND_MAX * MIN(w,h) / 2.0;

        cairo_arc(ctx->ctx, x, y, v, 0, M_PI * 2.0);

        ca_draw(opt->drawMode,ctx->ctx);
        break;
    case SHAPE_TRIANGLE:
        break;
    case SHAPE_STAR:
        x = (double)rnd()/RAND_MAX * w;
        y = (double)rnd()/RAND_MAX * h;
        z = (float)rnd()/RAND_MAX *0.5 + 0.15; //scale

        cairo_move_to (ctx->ctx, x+star_points[0][0]*z, y+star_points[0][1]*z);
        for (int s=1; s<11; s++)
            cairo_line_to (ctx->ctx, x+star_points[s][0]*z, y+star_points[s][1]*z);
        cairo_close_path (ctx->ctx);

        ca_draw(opt->drawMode,ctx->ctx);
        break;
    case SHAPE_RANDOM:
        break;
    }
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

