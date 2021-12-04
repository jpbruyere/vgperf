
#include "shapes.h"

void randomize_color (VkvgContext ctx) {
	vkvg_set_source_rgba(ctx,
						 frnd48(),
						 frnd48(),
						 frnd48(),
						 frnd48()
						 );
	//vkvg_set_source_color (ctx, rnd());
}

void vkvg_draw_shape (shape_t shape, options_t *opt, library_context_t* ctx) {
	int w = opt->width;
	int h = opt->height;

	float x, y, z, v, r;

	switch (opt->fillType) {
	case FILL_TYPE_SOLID:
		randomize_color (ctx->ctx);
		break;
	case FILL_TYPE_SURFACE:
		vkvg_set_source_surface (ctx->ctx, ctx->imgSurf, 0, 0);
		break;;
	case FILL_TYPE_LINEAR:
	case FILL_TYPE_RADIAL:
		randomize_color (ctx->ctx);
		break;;
	}

	switch (shape) {
	case SHAPE_LINE:
		x = frnd48() * w;
		y = frnd48() * h;
		z = frnd48() * w;
		v = frnd48() * h;

		vkvg_move_to(ctx->ctx, x, y);
		vkvg_line_to(ctx->ctx, z, v);
		vkvg_stroke(ctx->ctx);
		break;
	case SHAPE_RECTANGLE:
		x = frnd48() * w;
		y = frnd48() * h;
		z = 0.5f * frnd48() * w + 1;
		v = 0.5f * frnd48() * h + 1;

		vkvg_rectangle(ctx->ctx, x, y, z, v);
		vkvg_draw(opt->drawMode, ctx->ctx);

		break;
	case SHAPE_ROUNDED_RECTANGLE:
		x = frnd48() * w;
		y = frnd48() * h;
		z = 0.5f * frnd48() * w + 5;
		v = 0.5f * frnd48() * h + 5;
		r = 0.2f * frnd48() * MIN(z,v);

		vkvg_move_to(ctx->ctx, x, y + r);
		vkvg_arc(ctx->ctx, x + r, y + r, r, (float)M_PI, (float)-M_PI_2);
		vkvg_line_to(ctx->ctx, x + z - r, y);
		vkvg_arc(ctx->ctx, x + z - r, y + r, r, (float)-M_PI_2, 0);
		vkvg_line_to(ctx->ctx, x + z, y + v - r);
		vkvg_arc(ctx->ctx, x + z - r, y + v - r, r, 0, (float)M_PI_2);
		vkvg_line_to(ctx->ctx, x + r, y + v);
		vkvg_arc(ctx->ctx, x + r, y + v - r, r, (float)M_PI_2, (float)M_PI);
		vkvg_line_to(ctx->ctx, x, y + r);
		vkvg_close_path(ctx->ctx);

		vkvg_draw(opt->drawMode, ctx->ctx);
		break;
	case SHAPE_CIRCLE:
		x = frnd48() * w;
		y = frnd48() * h;
		v = frnd48() * MIN(w,h) * 0.2f;

		vkvg_arc(ctx->ctx, x, y, v, 0, (float)M_PI * 2.0f);

		vkvg_draw(opt->drawMode,ctx->ctx);
		break;
	case SHAPE_TRIANGLE:
		break;
	case SHAPE_STAR:
		x = frnd48() * w;
		y = frnd48() * h;
		z = frnd48() * 0.5 + 0.15; //scale

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
