
#include "shapes.h"

void randomize_color (VkvgContext ctx) {
    vkvg_set_source_rgba(ctx,
        (float)rnd()/RAND_MAX,
        (float)rnd()/RAND_MAX,
        (float)rnd()/RAND_MAX,
        (float)rnd()/RAND_MAX
    );
}

void vkvg_draw_shape (shape_t shape, options_t *opt, library_context_t* ctx) {
    int w = opt->width;
    int h = opt->height;

    float x, y, z, v;

    randomize_color (ctx->ctx);

    switch (shape) {
    case SHAPE_LINE:
        x = (float)rnd()/RAND_MAX * w;
        y = (float)rnd()/RAND_MAX * h;
        z = (float)rnd()/RAND_MAX * w;
        v = (float)rnd()/RAND_MAX * h;

        vkvg_move_to(ctx->ctx, x, y);
        vkvg_line_to(ctx->ctx, z, v);
        vkvg_stroke(ctx->ctx);
        break;
    case SHAPE_RECTANGLE:
        x = truncf( (0.5*(float)opt->width*rnd())/RAND_MAX );
        y = truncf( (0.5*(float)opt->height*rnd())/RAND_MAX );
        z = truncf( (0.5*(float)opt->width*rnd())/RAND_MAX ) + 1;
        v = truncf( (0.5*(float)opt->height*rnd())/RAND_MAX ) + 1;

        vkvg_rectangle(ctx->ctx, x+1, y+1, z, v);

        vkvg_draw(opt->drawMode, ctx->ctx);
        break;
    case SHAPE_ROUNDED_RECTANGLE:
        break;
    case SHAPE_CIRCLE:
        x = (float)rnd()/RAND_MAX * w;
        y = (float)rnd()/RAND_MAX * h;
        v = (float)rnd()/RAND_MAX * MIN(w,h) * 0.5f;

        vkvg_arc(ctx->ctx, x, y, v, 0, (float)M_PI * 2.0f);

        vkvg_draw(opt->drawMode,ctx->ctx);
        break;
    case SHAPE_TRIANGLE:
        break;
    case SHAPE_STAR:
        x = (float)rnd()/RAND_MAX * w;
        y = (float)rnd()/RAND_MAX * h;
        z = (float)rnd()/RAND_MAX *0.5 + 0.15; //scale

        vkvg_move_to (ctx->ctx, x+star_points[0][0]*z, y+star_points[0][1]*z);
        for (int s=1; s<11; s++)
            vkvg_line_to (ctx->ctx, x+star_points[s][0]*z, y+star_points[s][1]*z);
        vkvg_close_path (ctx->ctx);

        vkvg_draw(opt->drawMode,ctx->ctx);
        break;
    case SHAPE_RANDOM:
        break;
    }
}

void vkvg_draw (draw_mode_t drawMode, VkvgContext ctx) {
    switch (drawMode) {
    case DM_BOTH:
        vkvg_fill_preserve(ctx);
        randomize_color(ctx);
        vkvg_stroke(ctx);
        break;
    case DM_FILL:
        vkvg_fill(ctx);
        break;
    case DM_STROKE:
        vkvg_stroke(ctx);
        break;
    }
}
