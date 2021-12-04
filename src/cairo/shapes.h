#ifndef CAIRO_SHAPES_H
#define CAIRO_SHAPES_H

#include "common.h"

void ca_randomize_color (cairo_t *ctx);
void ca_draw_shape (shape_t shape, options_t *opt, library_context_t* ctx);
void ca_draw (draw_mode_t drawMode, cairo_t *ctx);
cairo_surface_t* ca_create_png_surface ();
#endif
