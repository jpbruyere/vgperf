#ifndef VKVG_COMMON_H
#define VKVG_COMMON_H

#include "../vgperf.h"
#include "vkvg.h"
#include "vkengine.h"

typedef struct _library_context {
    vk_engine_t* vkEngine;

    VkvgDevice dev;
    VkvgSurface surf;
	VkvgSurface imgSurf;
    VkvgContext ctx;
} library_context_t;

#endif
