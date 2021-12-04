#ifndef CAIRO_COMMON_H
#define CAIRO_COMMON_H

#include "../vgperf.h"
#include <cairo.h>

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

#if defined(WITH_CAIRO_GLES) & defined(WITH_GLFW3)

#define GLFW_EXPOSE_NATIVE_EGL
#define GLFW_INCLUDE_ES3
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <GLES2/gl2.h>
#include <cairo-gl.h>
#endif

typedef struct _library_context {
    cairo_surface_t* surf;
	cairo_surface_t* imgSurf;
    cairo_t* ctx;
	int groupd_draw;
} library_context_t;

void ca_present (options_t *opt, library_context_t* ctx);
#endif
