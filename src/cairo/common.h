#ifndef CAIRO_COMMON_H
#define CAIRO_COMMON_H

#include "../vgperf.h"
#include "cairo.h"

typedef struct _library_context {
	cairo_surface_t* surf;
	cairo_t* ctx;
} library_context_t;

void ca_randomize_color (cairo_t* ctx);
#endif
