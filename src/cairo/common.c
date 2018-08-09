
#include "common.h"

#include "basic_tests.h"
#include "test1.h"

void ca_randomize_color (cairo_t *ctx) {
    cairo_set_source_rgba(ctx,
        (double)rnd()/RAND_MAX,
        (double)rnd()/RAND_MAX,
        (double)rnd()/RAND_MAX,
        (double)rnd()/RAND_MAX
    );
}

void ca_draw_shape (shape_t shape, options_t *opt, library_context_t* ctx) {
    int w = opt->width;
    int h = opt->height;

    double x, y, z, v;

    ca_randomize_color (ctx->ctx);

    switch (shape) {
    case SHAPE_LINE:
        x = (double)rnd()/RAND_MAX * w;
        y = (double)rnd()/RAND_MAX * h;
        z = (double)rnd()/RAND_MAX * w;
        v = (double)rnd()/RAND_MAX * h;

        cairo_move_to(ctx->ctx, x, y);
        cairo_line_to(ctx->ctx, z, v);
        cairo_stroke(ctx->ctx);
        break;
    case SHAPE_RECTANGLE:
        x = trunc( (0.5*(double)opt->width*rnd())/RAND_MAX );
        y = trunc( (0.5*(double)opt->height*rnd())/RAND_MAX );
        z = trunc( (0.5*(double)opt->width*rnd())/RAND_MAX ) + 1;
        v = trunc( (0.5*(double)opt->height*rnd())/RAND_MAX ) + 1;

        cairo_rectangle(ctx->ctx, x+1, y+1, z, v);

        ca_draw(opt->drawMode, ctx->ctx);
        break;
    case SHAPE_ROUNDED_RECTANGLE:
        break;
    case SHAPE_CIRCLE:
        x = (double)rnd()/RAND_MAX * w;
        y = (double)rnd()/RAND_MAX * h;
        v = (double)rnd()/RAND_MAX * MIN(w,h) / 2.0;

        cairo_arc(ctx->ctx, x, y, v, 0, M_PI * 2.0);

        ca_draw(opt->drawMode,ctx->ctx);
        break;
    case SHAPE_TRIANGLE:
        break;
    case SHAPE_STAR:
        x = (double)rnd()/RAND_MAX * w;
        y = (double)rnd()/RAND_MAX * h;
        z = (float)rnd()/RAND_MAX *0.5 + 0.15; //scale

        cairo_move_to (ctx->ctx, x+star_points[0][0]*z, y+star_points[0][1]*z);
        for (int s=1; s<11; s++)
            cairo_line_to (ctx->ctx, x+star_points[s][0]*z, y+star_points[s][1]*z);
        cairo_close_path (ctx->ctx);

        ca_draw(opt->drawMode,ctx->ctx);
        break;
    case SHAPE_RANDOM:
        break;
    }
}

void ca_draw (draw_mode_t drawMode, cairo_t *ctx) {
    switch (drawMode) {
    case DM_BOTH:
        cairo_fill_preserve(ctx);
        ca_randomize_color (ctx);
        cairo_stroke(ctx);
        break;
    case DM_FILL:
        cairo_fill(ctx);
        break;
    case DM_STROKE:
        cairo_stroke(ctx);
        break;
    }
}

#if CAIRO_HAS_XLIB_SURFACE & WITH_CAIRO_X11
library_context_t* ca_xlib_initLibrary(options_t* opt) {
    library_context_t* ctx = (library_context_t*)calloc(1, sizeof(library_context_t));

    Display *dsp;
    Drawable da;
    int screen;

    if ((dsp = XOpenDisplay(NULL)) == NULL)
       exit(1);
    screen = DefaultScreen(dsp);
    da = XCreateSimpleWindow(dsp, DefaultRootWindow(dsp), 0, 0, opt->width, opt->height, 0, 0, 0);
    XSelectInput(dsp, da, ExposureMask);
    XMapWindow(dsp, da);

    ctx->surf = cairo_xlib_surface_create(dsp, da, DefaultVisual(dsp, screen), opt->width, opt->height);
    cairo_xlib_surface_set_size(ctx->surf, opt->width, opt->height);

    return ctx;
}

/*! Destroy cairo Xlib surface and close X connection.
 */
void ca_xlib_cleanupLibrary (library_context_t* ctx) {
    Display *dsp = cairo_xlib_surface_get_display(ctx->surf);
    cairo_surface_destroy (ctx->surf);
    XCloseDisplay(dsp);
    free(ctx);
}
void ca_xlib_cleanupTest (options_t* opt, library_context_t* ctx) {
    cairo_destroy(ctx->ctx);
    cairo_surface_flush (ctx->surf);
}
#endif

/**
 * @brief init library once at startup
 * @param global options
 * @return a pointer to the library test context
 */
library_context_t* ca_initLibrary(options_t* opt) {
    library_context_t* ctx = (library_context_t*)calloc(1, sizeof(library_context_t));

    ctx->surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, opt->width, opt->height);

    return ctx;
}
/**
 * @brief cleanup Library and release test context pointer
 * @param library test context
 */
void ca_cleanupLibrary (library_context_t* ctx) {
    cairo_surface_destroy (ctx->surf);

    free(ctx);
}
void ca_saveImg (library_context_t* ctx, const char* fileName) {
    cairo_surface_write_to_png (ctx->surf, fileName);
}
/**
 * @brief common test init func for simple tests, context creation and settings
 * @param global options
 * @param cairo library context
 */
void ca_initTest(options_t* opt, library_context_t* ctx) {
    ctx->ctx = cairo_create(ctx->surf);
    switch (opt->antialias) {
    case ANTIALIAS_NONE:
        cairo_set_antialias (ctx->ctx, CAIRO_ANTIALIAS_NONE);
        break;
    case ANTIALIAS_DEFAULT:
        cairo_set_antialias (ctx->ctx, CAIRO_ANTIALIAS_DEFAULT);
        break;
    case ANTIALIAS_FAST:
        cairo_set_antialias (ctx->ctx, CAIRO_ANTIALIAS_FAST);
        break;
    case ANTIALIAS_GOOD:
        cairo_set_antialias (ctx->ctx, CAIRO_ANTIALIAS_GOOD);
        break;
    case ANTIALIAS_BEST:
        cairo_set_antialias (ctx->ctx, CAIRO_ANTIALIAS_BEST);
        break;
    }
    switch (opt->capStyle) {
    case LINE_CAP_BUTT:
        cairo_set_line_cap(ctx->ctx, CAIRO_LINE_CAP_BUTT);
        break;
    case LINE_CAP_ROUND:
        cairo_set_line_cap(ctx->ctx, CAIRO_LINE_CAP_ROUND);
        break;
    case LINE_CAP_SQUARE:
        cairo_set_line_cap(ctx->ctx, CAIRO_LINE_CAP_SQUARE);
        break;
    }
    switch (opt->joinStyle) {
    case LINE_JOIN_MITER:
        cairo_set_line_join(ctx->ctx, CAIRO_LINE_JOIN_MITER);
        break;
    case LINE_JOIN_BEVEL:
        cairo_set_line_join(ctx->ctx, CAIRO_LINE_JOIN_BEVEL);
        break;
    case LINE_JOIN_ROUND:
        cairo_set_line_join(ctx->ctx, CAIRO_LINE_JOIN_ROUND);
        break;
    }

    //clear surface
    cairo_set_operator(ctx->ctx, CAIRO_OPERATOR_CLEAR);
    cairo_paint(ctx->ctx);
    cairo_set_operator(ctx->ctx, CAIRO_OPERATOR_OVER);

    cairo_set_line_width (ctx->ctx, opt->lineWidth);
}
void ca_cleanupTest (options_t* opt, library_context_t* ctx) {
    cairo_destroy(ctx->ctx);
}

void ca_xlib_present (options_t* opt, library_context_t* ctx) {
    XSync(cairo_xlib_surface_get_display(ctx->surf), 1);
    //cairo_surface_flush (ctx->surf);
}

void init_cairo_tests (vgperf_context_t* ctx) {
#if CAIRO_HAS_XLIB_SURFACE & WITH_CAIRO_X11
    ctx->libName = "cairo x11";
    ctx->init = (PFNinitLibrary) ca_xlib_initLibrary;
    ctx->cleanup = (PFNcleanupLibrary) ca_xlib_cleanupLibrary;
    ctx->present = (PFNtest) ca_xlib_present;
#elif CAIRO_HAS_XCB_SURFACE & WITH_CAIRO_XCB

#else
    ctx->libName = "cairo";
    ctx->init = (PFNinitLibrary) ca_initLibrary;
    ctx->cleanup = (PFNcleanupLibrary) ca_cleanupLibrary;
    ctx->present = NULL;
#endif
    ctx->saveImg = (PFNSaveImg) ca_saveImg;
    ctx->testCount = 0;
    ctx->tests = (test_t*)malloc(0);

    addTest(ctx, "lines stroke", ca_initTest, ca_line_perform, ca_cleanupTest);
    addTest(ctx, "rectangles", ca_initTest, ca_rectangles_perform, ca_cleanupTest);
    addTest(ctx, "circles", ca_initTest, ca_circles_perform, ca_cleanupTest);
    addTest(ctx, "stars", ca_initTest, ca_stars_perform, ca_cleanupTest);
    //addTest(ctx, "anim1", ca_initTest, cairo_test1_perform, ca_cleanupTest);
    addTest(ctx, "test1", NULL, cairo_test1_perform, NULL);
    //addTest(ctx, "single poly", ca_initTest, ca_single_poly_perform, ca_cleanupTest);
}
