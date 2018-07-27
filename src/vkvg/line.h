#include <stdio.h>
#include <stdlib.h>

#include "../vgperf.h"
#include "common.h"

void line_init (options_t *opt, library_context_t* ctx)
{
	ctx->ctx = vkvg_create(ctx->surf);
	//vkvg_set_ (ctx->ctx, CAIRO_ANTIALIAS_NONE);
	vkvg_set_line_width (ctx->ctx, 1);
}

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

void line_cleanup (options_t *opt, library_context_t* ctx) {
	vkvg_destroy(ctx->ctx);
}
