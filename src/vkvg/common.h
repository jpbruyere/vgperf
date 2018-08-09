#ifndef VKVG_COMMON_H
#define VKVG_COMMON_H

#include "../vgperf.h"
#include "vkvg.h"
#include "vkengine.h"

typedef struct _library_context {
    vk_engine_t* vkEngine;

    VkvgDevice dev;
    VkvgSurface surf;
    VkvgContext ctx;
} library_context_t;

void randomize_color (VkvgContext ctx);
void vkvg_draw_shape (shape_t shape, options_t *opt, library_context_t* ctx);
void vkvg_draw (draw_mode_t drawMode, VkvgContext ctx);
void vkvg_present (options_t* opt, library_context_t* ctx);
#endif
