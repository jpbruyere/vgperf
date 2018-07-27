#include <stdio.h>
#include <stdlib.h>

#include "../vgperf.h"
#include "common.h"

void rect_init (options_t *opt, library_context_t* ctx)
{
	ctx->ctx = vkvg_create(ctx->surf);
	vkvg_set_line_width (ctx->ctx, 1);
}

void rect_perform (options_t *opt, library_context_t* ctx)
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

void rect_cleanup (options_t *opt, library_context_t* ctx) {
	vkvg_destroy(ctx->ctx);
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

