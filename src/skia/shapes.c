
#include "shapes.h"

void skia_randomize_color (sk_paint_t *paint) {
	sk_paint_set_color(paint, rnd());
	uint32_t c = sk_paint_get_color (paint);
	c &= 0x00ffffff;
	c += (uint32_t)(255 * 0.2f) << 24;
	sk_paint_set_color(paint, c);
}

void skia_draw_shape (shape_t shape, options_t *opt, library_context_t* ctx) {
	int w = opt->width;
	int h = opt->height;

	float x, y, z, v;

	skia_randomize_color (ctx->paint);

	switch (shape) {
	case SHAPE_LINE:
		x = (float)rnd()/(float)RAND_MAX * w;
		y = (float)rnd()/(float)RAND_MAX * h;
		z = (float)rnd()/(float)RAND_MAX * w;
		v = (float)rnd()/(float)RAND_MAX * h;

		sk_paint_set_stroke(ctx->paint, true);

		sk_pathbuilder_t* path_builder = sk_pathbuilder_new();
		sk_pathbuilder_move_to(path_builder, x, y);
		sk_pathbuilder_line_to(path_builder, z, v);

		sk_path_t* path = sk_pathbuilder_detach_path(path_builder);
		sk_canvas_draw_path(ctx->ctx, path, ctx->paint);

		sk_pathbuilder_delete(path_builder);
		sk_path_delete(path);

		break;
	case SHAPE_RECTANGLE:
		x = truncf( (0.5*(float)opt->width*rnd())/(float)RAND_MAX );
		y = truncf( (0.5*(float)opt->height*rnd())/(float)RAND_MAX );
		w = truncf( (0.5*(float)opt->width*rnd())/(float)RAND_MAX ) + 1;
		h = truncf( (0.5*(float)opt->height*rnd())/(float)RAND_MAX ) + 1;

		sk_rect_t rect;
		rect.left = x;
		rect.top = y;
		rect.right = x+w;
		rect.bottom = y+h;

		switch (opt->drawMode) {
		case DM_FILL:
			sk_paint_set_stroke(ctx->paint, false);
			break;
		case DM_STROKE:
			sk_paint_set_stroke(ctx->paint, true);
			break;
		case DM_BOTH:
			sk_paint_set_stroke(ctx->paint, true);
			sk_canvas_draw_rect(ctx->ctx, &rect, ctx->paint);
			sk_paint_set_stroke(ctx->paint, false);
			break;
		}
		sk_canvas_draw_rect(ctx->ctx, &rect, ctx->paint);
		break;
	case SHAPE_ROUNDED_RECTANGLE:

		break;
	case SHAPE_CIRCLE:
		x = (float)rnd()/(float)RAND_MAX * w;
		y = (float)rnd()/(float)RAND_MAX * h;
		v = (float)rnd()/(float)RAND_MAX * MIN(w,h) * 0.5f;

		switch (opt->drawMode) {
		case DM_STROKE:
			sk_paint_set_stroke(ctx->paint, true);
			break;
		case DM_BOTH:
			sk_paint_set_stroke(ctx->paint, true);
			sk_canvas_draw_circle(ctx->ctx,x,y,v,ctx->paint);
			sk_paint_set_stroke(ctx->paint, false);
			break;
		}
		sk_canvas_draw_circle(ctx->ctx,x,y,v,ctx->paint);
		break;
	case SHAPE_TRIANGLE:
		break;
	case SHAPE_STAR:
		x = (float)rnd()/(float)RAND_MAX * w;
		y = (float)rnd()/(float)RAND_MAX * h;
		z = (float)rnd()/(float)RAND_MAX *0.5 + 0.15; //scale

		sk_pathbuilder_t* star = sk_pathbuilder_new();
		sk_pathbuilder_move_to(star, x+star_points[0][0]*z, y+star_points[0][1]*z);
		for (int s=1; s<11; s++)
			sk_pathbuilder_line_to(star, x+star_points[s][0]*z, y+star_points[s][1]*z);
		sk_pathbuilder_close(star);

		path = sk_pathbuilder_detach_path(star);

		switch (opt->drawMode) {
		case DM_FILL:
			sk_paint_set_stroke(ctx->paint, false);
			break;
		case DM_STROKE:
			sk_paint_set_stroke(ctx->paint, true);
			break;
		case DM_BOTH:
			sk_paint_set_stroke(ctx->paint, true);
			sk_canvas_draw_path(ctx->ctx, path, ctx->paint);
			sk_paint_set_stroke(ctx->paint, false);
			break;
		}
		sk_canvas_draw_path(ctx->ctx, path, ctx->paint);

		sk_pathbuilder_delete(star);
		sk_path_delete(path);
		break;
	case SHAPE_RANDOM:
		break;
	}
}

void skia_draw (draw_mode_t drawMode, sk_paint_t* ctx) {
	/*switch (drawMode) {
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
	}*/
}
