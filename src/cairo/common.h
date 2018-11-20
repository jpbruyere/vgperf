#ifndef CAIRO_COMMON_H
#define CAIRO_COMMON_H

#include "../vgperf.h"
#include "cairo.h"

#if WITH_CAIRO_XCB
#include <xcb/xcb.h>
#include <cairo-xcb.h>
#endif

#if WITH_CAIRO_XLIB
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <cairo-xlib.h>
#endif

typedef struct _library_context {
    cairo_surface_t* surf;
    cairo_t* ctx;
} library_context_t;

void ca_randomize_color (cairo_t* ctx);
void ca_draw_shape (shape_t shape, options_t *opt, library_context_t* ctx);
void ca_draw (draw_mode_t drawMode, cairo_t* ctx);
void ca_present (options_t *opt, library_context_t* ctx);
#endif
