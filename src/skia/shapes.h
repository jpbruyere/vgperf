#ifndef SKIA_SHAPES_H
#define SKIA_SHAPES_H

#include "common.h"

void skia_randomize_color (sk_paint_t *paint);
void skia_draw_shape (shape_t shape, options_t *opt, library_context_t* ctx);
void skia_draw (draw_mode_t drawMode, sk_paint_t *paint);
#endif
