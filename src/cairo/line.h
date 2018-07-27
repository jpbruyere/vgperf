#include <stdio.h>
#include <stdlib.h>

#include "../vgperf.h"
#include "common.h"

void ca_line_init (options_t *opt, library_context_t* ctx)
{
	ctx->ctx = cairo_create(ctx->surf);
	cairo_set_antialias (ctx->ctx, CAIRO_ANTIALIAS_NONE);
	cairo_set_line_width (ctx->ctx, 1);
}

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

void ca_line_cleanup (options_t *opt, library_context_t* ctx) {
	cairo_destroy(ctx->ctx);
}
