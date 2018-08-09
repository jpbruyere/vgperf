
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>

#include "../vgperf.h"
#include "common.h"


void cairo_test_fill_rule (cairo_t* cr){
	cairo_set_line_width (cr, 6);

	//cairo_scale(cr,3,3);
	cairo_set_source_rgba(cr,1,0,0,1);
	cairo_move_to(cr,50,150);
	cairo_rel_line_to(cr,50,70);
	cairo_rel_line_to(cr,50,0);
	cairo_rel_line_to(cr,50,-70);
	cairo_rel_line_to(cr,0,-60);
	cairo_rel_line_to(cr,-50,-30);
	cairo_rel_line_to(cr,-50,0);
	cairo_rel_line_to(cr,-50,30);
	cairo_close_path(cr);


//    cairo_set_line_join(cr,CAIRO_LINE_JOIN_ROUND);
	cairo_set_source_rgb (cr, 0, 0.7, 0);
	cairo_rectangle (cr, 12, 12, 232, 70);
	//cairo_stroke (cr);
//    cairo_new_sub_path (cr);
	cairo_arc (cr, 64, 64, 40, 0, M_PI*2);
	//cairo_close_path(cr);

	cairo_new_sub_path (cr);
	cairo_arc_negative (cr, 192, 64, 40, 2*M_PI, 0);
	//cairo_close_path(cr);

	//cairo_rectangle (cr, 30, 30, 20, 200);
	//cairo_rectangle (cr, 130, 30, 20, 200);
	//cairo_set_fill_rule (cr, cairo_FILL_RULE_EVEN_ODD);

	cairo_fill_preserve(cr);

	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_stroke (cr);
}
void cairo_test_text (cairo_t* cr) {
	cairo_text_extents_t extents;

	const char *utf8 = "cairo";
	float x,y;

	cairo_select_font_face (cr, "times", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size (cr, 100.0);
	cairo_text_extents (cr, utf8, &extents);
	cairo_set_source_rgb(cr,0,0,0);

	x=25.0;
	y=150.0;

	cairo_move_to (cr, x,y);
	cairo_show_text (cr, utf8);

	/* draw helping lines */
	cairo_set_source_rgba (cr, 1, 0.2, 0.2, 0.6);
	cairo_set_line_width (cr, 6.0);
	cairo_arc (cr, x, y, 10.0, 0, 2*M_PI);
	cairo_fill (cr);
	cairo_move_to (cr, x,y);
	cairo_rel_line_to (cr, 0, -extents.height);
	cairo_rel_line_to (cr, extents.width, 0);
	cairo_rel_line_to (cr, extents.x_bearing, -extents.y_bearing);
	cairo_stroke (cr);
}
void cairo_test_clip (cairo_t* cr){
	cairo_arc (cr, 128.0, 128.0, 76.8, 0, 2 * M_PI);
	cairo_clip (cr);
	//cairo_new_path (cr);  /* current path is not
	//                         consumed by cairo_clip() */
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_rectangle (cr, 0, 0, 256, 256);
	cairo_fill (cr);
	cairo_set_source_rgba (cr, 0, 1, 0, 1);
	cairo_move_to (cr, -100, -100);
	cairo_line_to (cr, 256, 256);
	cairo_move_to (cr, 356, -100);
	cairo_line_to (cr, 0, 256);
	cairo_set_line_width (cr, 10.0);
	cairo_stroke (cr);
}
void cairo_test_curves (cairo_t* cr){
	float x=25.6,  y=128.0;
	float x1=102.4, y1=230.4,
		   x2=153.6, y2=25.6,
		   x3=230.4, y3=128.0;

	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_move_to (cr, x, y);
	cairo_curve_to (cr, x1, y1, x2, y2, x3, y3);

	cairo_set_line_width (cr, 10.0);
	cairo_stroke (cr);

	cairo_set_source_rgba (cr, 1, 0.2, 0.2, 0.6);
	cairo_set_line_width (cr, 6.0);
	cairo_move_to (cr,x,y);   cairo_line_to (cr,x1,y1);
	cairo_move_to (cr,x2,y2); cairo_line_to (cr,x3,y3);
	cairo_stroke (cr);
}
void cairo_test_rounded_rect (cairo_t* cr) {
	/* a custom shape that could be wrapped in a function */
	float x0      = 25.6,   /* parameters like cairo_rectangle */
		   y0      = 25.6,
		   rect_width  = 204.8,
		   rect_height = 204.8,
		   radius = 102.4;   /* and an approximate curvature radius */

	float x1,y1;

	x1=x0+rect_width;
	y1=y0+rect_height;
	if (!rect_width || !rect_height)
		return;
	if (rect_width/2<radius) {
		if (rect_height/2<radius) {
			cairo_move_to  (cr, x0, (y0 + y1)/2);
			cairo_curve_to (cr, x0 ,y0, x0, y0, (x0 + x1)/2, y0);
			cairo_curve_to (cr, x1, y0, x1, y0, x1, (y0 + y1)/2);
			cairo_curve_to (cr, x1, y1, x1, y1, (x1 + x0)/2, y1);
			cairo_curve_to (cr, x0, y1, x0, y1, x0, (y0 + y1)/2);
		} else {
			cairo_move_to  (cr, x0, y0 + radius);
			cairo_curve_to (cr, x0 ,y0, x0, y0, (x0 + x1)/2, y0);
			cairo_curve_to (cr, x1, y0, x1, y0, x1, y0 + radius);
			cairo_line_to (cr, x1 , y1 - radius);
			cairo_curve_to (cr, x1, y1, x1, y1, (x1 + x0)/2, y1);
			cairo_curve_to (cr, x0, y1, x0, y1, x0, y1- radius);
		}
	} else {
		if (rect_height/2<radius) {
			cairo_move_to  (cr, x0, (y0 + y1)/2);
			cairo_curve_to (cr, x0 , y0, x0 , y0, x0 + radius, y0);
			cairo_line_to (cr, x1 - radius, y0);
			cairo_curve_to (cr, x1, y0, x1, y0, x1, (y0 + y1)/2);
			cairo_curve_to (cr, x1, y1, x1, y1, x1 - radius, y1);
			cairo_line_to (cr, x0 + radius, y1);
			cairo_curve_to (cr, x0, y1, x0, y1, x0, (y0 + y1)/2);
		} else {
			cairo_move_to  (cr, x0, y0 + radius);
			cairo_curve_to (cr, x0 , y0, x0 , y0, x0 + radius, y0);
			cairo_line_to (cr, x1 - radius, y0);
			cairo_curve_to (cr, x1, y0, x1, y0, x1, y0 + radius);
			cairo_line_to (cr, x1 , y1 - radius);
			cairo_curve_to (cr, x1, y1, x1, y1, x1 - radius, y1);
			cairo_line_to (cr, x0 + radius, y1);
			cairo_curve_to (cr, x0, y1, x0, y1, x0, y1- radius);
		}
	}
	cairo_close_path (cr);

	cairo_set_source_rgb (cr, 0.5, 0.5, 1);
	cairo_fill_preserve (cr);
	cairo_set_source_rgba (cr, 0.5, 0, 0, 0.5);
	cairo_set_line_width (cr, 10.0);
	cairo_stroke (cr);
}
void cairo_test_fill_and_stroke2 (cairo_t* cr){
	cairo_move_to (cr, 128.0, 25.6);
	cairo_line_to (cr, 230.4, 230.4);
	cairo_rel_line_to (cr, -102.4, 0.0);
	cairo_curve_to (cr, 51.2, 230.4, 51.2, 128.0, 128.0, 128.0);
	cairo_close_path (cr);

	cairo_move_to (cr, 64.0, 25.6);
	cairo_rel_line_to (cr, 51.2, 51.2);
	cairo_rel_line_to (cr, -51.2, 51.2);
	cairo_rel_line_to (cr, -51.2, -51.2);
	cairo_close_path (cr);

	/*cairo_translate(cr,100,100);
	cairo_move_to (cr, 100, 100);
	cairo_line_to(cr,300,300);
	cairo_line_to(cr,100,300);*/


	cairo_set_line_width (cr, 10.0);
	cairo_set_source_rgb (cr, 0, 0, 1);
	cairo_fill_preserve (cr);
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_stroke (cr);
}
void cairo_print_arc_neg (cairo_t* cr){
	float xc = 128.0;
	float yc = 128.0;
	float radius = 100.0;
	float angle1 = 45.0  * (M_PI/180.0);  /* angles are specified */
	float angle2 = 180.0 * (M_PI/180.0);  /* in radians           */

	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_set_line_width (cr, 10.0);
	cairo_arc_negative (cr, xc, yc, radius, angle1, angle2);
	cairo_stroke (cr);

	/* draw helping lines */
	cairo_set_source_rgba (cr, 1, 0.2, 0.2, 0.6);
	cairo_set_line_width (cr, 6.0);

	cairo_arc (cr, xc, yc, 10.0, 0, 2*M_PI);
	cairo_fill (cr);

	cairo_arc (cr, xc, yc, radius, angle1, angle1);
	cairo_line_to (cr, xc, yc);
	cairo_arc (cr, xc, yc, radius, angle2, angle2);
	//cairo_line_to (cr, xc, yc);
	cairo_stroke (cr);

}
void cairo_test_line_caps (cairo_t* cr) {
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_set_line_width (cr, 30.0);
	cairo_set_line_cap  (cr, CAIRO_LINE_CAP_BUTT); /* default */
	cairo_move_to (cr, 64.0, 50.0); cairo_line_to (cr, 64.0, 200.0);
	cairo_stroke (cr);
	cairo_set_line_cap  (cr, CAIRO_LINE_CAP_ROUND);
	cairo_move_to (cr, 128.0, 50.0); cairo_line_to (cr, 128.0, 200.0);
	cairo_stroke (cr);
	cairo_set_line_cap  (cr, CAIRO_LINE_CAP_SQUARE);
	cairo_move_to (cr, 192.0, 50.0); cairo_line_to (cr, 192.0, 200.0);
	cairo_stroke (cr);

	/* draw helping lines */
	cairo_set_source_rgb (cr, 1, 0.2, 0.2);
	cairo_set_line_width (cr, 2.56);
	cairo_move_to (cr, 64.0, 50.0); cairo_line_to (cr, 64.0, 200.0);
	cairo_move_to (cr, 128.0, 50.0);  cairo_line_to (cr, 128.0, 200.0);
	cairo_move_to (cr, 192.0, 50.0); cairo_line_to (cr, 192.0, 200.0);
	cairo_stroke (cr);
}
void cairo_test_line_joins (cairo_t* cr) {
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_set_line_width (cr, 40.96);
	cairo_move_to (cr, 76.8, 84.48);
	cairo_rel_line_to (cr, 51.2, -51.2);
	cairo_rel_line_to (cr, 51.2, 51.2);
	cairo_set_line_join (cr, CAIRO_LINE_JOIN_MITER); /* default */
	cairo_stroke (cr);

	cairo_move_to (cr, 76.8, 161.28);
	cairo_rel_line_to (cr, 51.2, -51.2);
	cairo_rel_line_to (cr, 51.2, 51.2);
	cairo_set_line_join (cr, CAIRO_LINE_JOIN_BEVEL);
	cairo_stroke (cr);

	cairo_move_to (cr, 76.8, 238.08);
	cairo_rel_line_to (cr, 51.2, -51.2);
	cairo_rel_line_to (cr, 51.2, 51.2);
	cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);
	cairo_stroke (cr);
}
void cairo_print_arc (cairo_t* cr) {
	float xc = 128.0;
	float yc = 128.0;
	float radius = 100.0;
	float angle1 = 45.0  * (M_PI/180.0);  /* angles are specified */
	float angle2 = 180.0 * (M_PI/180.0);  /* in radians           */

	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_set_line_width (cr, 10.0);
	cairo_arc (cr, xc, yc, radius, angle1, angle2);
	cairo_stroke (cr);

	/* draw helping lines */
	cairo_set_source_rgba(cr, 1, 0.2, 0.2, 0.6);
	cairo_set_line_width (cr, 6.0);

	cairo_arc (cr, xc, yc, 10.0, 0, 2*M_PI);
	cairo_fill (cr);

	cairo_arc (cr, xc, yc, radius, angle1, angle1);
	cairo_line_to (cr, xc, yc);
	cairo_arc (cr, xc, yc, radius, angle2, angle2);
	cairo_stroke (cr);
}
static float rotation = 0.f;
void cairo_test1_perform (options_t *opt, library_context_t* ctx) {
	for (int i=0; i<opt->count; i++) {
		rotation+=0.002f;

		ctx->ctx = cairo_create(ctx->surf);
		cairo_t* cr = ctx->ctx;

		cairo_matrix_t mat;
		cairo_matrix_init_translate (&mat, 512,512);
		cairo_matrix_rotate(&mat,rotation);
		cairo_matrix_translate(&mat,-512,-512);

		cairo_set_source_rgba(cr,0.7,0.7,0.7,1);
		cairo_paint(cr);

		cairo_set_matrix(cr,&mat);

		cairo_print_arc(cr);

		cairo_translate(cr,200,0);
		cairo_test_fill_rule(cr);

		cairo_translate(cr,250,0);
		cairo_test_rounded_rect(cr);

		cairo_translate(cr,-450,250);
		cairo_test_fill_and_stroke2(cr);

		cairo_translate(cr,250,0);
		cairo_print_arc_neg(cr);

		cairo_translate(cr,250,0);
		cairo_test_text(cr);

		cairo_translate(cr,-500,250);
		cairo_test_curves(cr);

		cairo_translate(cr,250,0);
		cairo_test_line_joins(cr);

		cairo_translate(cr,250,0);
		cairo_test_line_caps(cr);

		cairo_destroy(ctx->ctx);

		XSync(cairo_xlib_surface_get_display(ctx->surf), 1);
	}

}
