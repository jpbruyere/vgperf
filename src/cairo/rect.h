#include <stdio.h>
#include <stdlib.h>

#include "../vgperf.h"
#include "common.h"

void ca_rect_init (options_t *opt, library_context_t* ctx)
{
	ctx->ctx = cairo_create(ctx->surf);
	cairo_set_antialias (ctx->ctx, CAIRO_ANTIALIAS_NONE);
	cairo_set_line_width (ctx->ctx, 1);
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

void ca_rect_cleanup (options_t *opt, library_context_t* ctx) {
	cairo_destroy(ctx->ctx);
}

/*
int
ca_test_rect(caskbench_context_t *ctx)
{
	int w = ctx->canvas_width;
	int h = ctx->canvas_height;

	shapes_t shape;
	shape_copy(&ctx->shape_defaults, &shape);
	for (int i=0; i<ctx->size; i++) {
		double x1 = (double)rnd()/RAND_MAX * w;
		double x2 = (double)rnd()/RAND_MAX * w;
		double y1 = (double)rnd()/RAND_MAX * h;
		double y2 = (double)rnd()/RAND_MAX * h;

		shape.x = MIN(x1, x2);
		shape.y = MIN(x1, x2);
		shape.width = abs(x2 - x1);
		shape.height = abs(y2 - y1);

		if (ctx->shape_defaults.fill_type == CB_FILL_RANDOM) {
			shape.fill_type = generate_random_fill_type();
		}
		ca_set_fill_style(ctx, &shape);

		shape.shape_type = CB_SHAPE_RECTANGLE;
		cairoDrawRandomizedShape(ctx,&shape);
	}

	return 1;
}
*/

