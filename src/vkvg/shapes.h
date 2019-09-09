#ifndef VKVG_SHAPES_H
#define VKVG_SHAPES_H

#include "common.h"

void randomize_color (VkvgContext ctx);
void vkvg_draw_shape (shape_t shape, options_t *opt, library_context_t* ctx);
void vkvg_draw (draw_mode_t drawMode, VkvgContext ctx);
#endif
