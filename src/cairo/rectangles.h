#include <stdio.h>
#include <stdlib.h>

#include "../vgperf.h"
#include "common.h"

void ca_rectangles_init (options_t *opt, library_context_t* ctx)
{
	ctx->ctx = cairo_create(ctx->surf);
	cairo_set_antialias (ctx->ctx, CAIRO_ANTIALIAS_DEFAULT);
	cairo_set_line_width (ctx->ctx, 1);
}

void ca_rectangles_perform (options_t *opt, library_context_t* ctx)
{
	double w, h, x, y;
	cairo_t *cr = ctx->ctx;

	for (int i=0; i<opt->count; i++) {
		ca_randomize_color (ctx->ctx);

		w = trunc( (0.5*(double)opt->width*rnd())/RAND_MAX ) + 1;
		h = trunc( (0.5*(double)opt->height*rnd())/RAND_MAX ) + 1;
		x = trunc( (0.5*(double)opt->width*rnd())/RAND_MAX );
		y = trunc( (0.5*(double)opt->height*rnd())/RAND_MAX );
		cairo_rectangle(cr, x+1, y+1, w, h);

		cairo_stroke (cr);
	}
}

void ca_rectangles_cleanup (options_t *opt, library_context_t* ctx) {
	cairo_destroy(ctx->ctx);
}
