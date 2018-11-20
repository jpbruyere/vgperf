
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

#if CAIRO_HAS_XCB_SURFACE & WITH_CAIRO_XCB
static xcb_visualtype_t *find_visual(xcb_connection_t *c, xcb_visualid_t visual)
{
    xcb_screen_iterator_t screen_iter = xcb_setup_roots_iterator(xcb_get_setup(c));

    for (; screen_iter.rem; xcb_screen_next(&screen_iter)) {
        xcb_depth_iterator_t depth_iter = xcb_screen_allowed_depths_iterator(screen_iter.data);
        for (; depth_iter.rem; xcb_depth_next(&depth_iter)) {
            xcb_visualtype_iterator_t visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
            for (; visual_iter.rem; xcb_visualtype_next(&visual_iter))
                if (visual == visual_iter.data->visual_id)
                    return visual_iter.data;
        }
    }

    return NULL;
}

library_context_t* ca_xcb_initLibrary(options_t* opt) {
    library_context_t* ctx = (library_context_t*)calloc(1, sizeof(library_context_t));

    xcb_connection_t *c;
    xcb_screen_t *screen;
    xcb_window_t window;
    uint32_t mask[2];
    xcb_visualtype_t *visual;

    c = xcb_connect(NULL, NULL);
    mask[0] = 1;
    mask[1] = XCB_EVENT_MASK_EXPOSURE;
    screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;
    window = xcb_generate_id(c);
    xcb_create_window(c, XCB_COPY_FROM_PARENT, window, screen->root,
            20, 20, opt->width, opt->height, 0,
            XCB_WINDOW_CLASS_INPUT_OUTPUT,
            screen->root_visual,
            XCB_CW_OVERRIDE_REDIRECT | XCB_CW_EVENT_MASK,
            mask);
    xcb_map_window(c, window);

    visual = find_visual(c, screen->root_visual);

    ctx->surf = cairo_xcb_surface_create(c, window, visual, opt->width, opt->height);

    return ctx;
}
void ca_xcb_cleanupLibrary (library_context_t* ctx) {
    xcb_connection_t *c = cairo_xcb_device_get_connection(cairo_surface_get_device(ctx->surf));
    cairo_surface_destroy (ctx->surf);
    xcb_disconnect (c);
    free(ctx);
}
void ca_xcb_cleanupTest (options_t* opt, library_context_t* ctx) {
    cairo_destroy(ctx->ctx);
    cairo_surface_flush (ctx->surf);
}
void ca_xcb_present (options_t* opt, library_context_t* ctx) {
    xcb_flush (cairo_xcb_device_get_connection(cairo_surface_get_device(ctx->surf)));
}

#endif

#if CAIRO_HAS_XLIB_SURFACE & WITH_CAIRO_XLIB
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

    ctx->surf = cairo_xlib_surface_create(dsp, da, XDefaultVisual(dsp, screen), opt->width, opt->height);
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
void ca_xlib_present (options_t* opt, library_context_t* ctx) {
    XSync(cairo_xlib_surface_get_display(ctx->surf), 1);
    //cairo_surface_flush (ctx->surf);
}
#endif

/**
 * @brief init library once at startup
 * @param global options
 * @return a pointer to the library test context
 */
library_context_t* ca_image_initLibrary(options_t* opt) {
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


void init_cairo_test_context (vgperf_context_t* ctx) {
    ctx->saveImg = (PFNSaveImg) ca_saveImg;

    ctx->testCount = 0;
    ctx->tests = (test_t*)malloc(0);

    addTest(ctx, "lines stroke", ca_initTest, ca_line_perform, ca_cleanupTest);
    addTest(ctx, "rectangles", ca_initTest, ca_rectangles_perform, ca_cleanupTest);
    addTest(ctx, "circles", ca_initTest, ca_circles_perform, ca_cleanupTest);
    addTest(ctx, "stars", ca_initTest, ca_stars_perform, ca_cleanupTest);
    //addTest(ctx, "anim1", ca_initTest, cairo_test1_perform, ca_cleanupTest);
    //addTest(ctx, "test1", NULL, cairo_test1_perform, NULL);
    //addTest(ctx, "single poly", ca_initTest, ca_single_poly_perform, ca_cleanupTest);
}

int init_cairo_tests (vgperf_context_t** libs) {
    int ctxCount=0;
    vgperf_context_t* ctx = NULL;

#if (CAIRO_HAS_GL_SURFACE || CAIRO_HAS_GLESV2_SURFACE || CAIRO_HAS_GLESV3_SURFACE) && WITH_CAIRO_GL
    vgperf_context_t* ctx = (vgperf_context_t*)malloc(sizeof(vgperf_context_t));
    libs[ctxCount++] = ctx;

#endif

#if CAIRO_HAS_XLIB_SURFACE & WITH_CAIRO_XLIB
    ctx = (vgperf_context_t*)malloc(sizeof(vgperf_context_t));
    libs[ctxCount++] = ctx;

    ctx->libName = "cairo xlib";
    ctx->init = (PFNinitLibrary) ca_xlib_initLibrary;
    ctx->cleanup = (PFNcleanupLibrary) ca_xlib_cleanupLibrary;
    ctx->present = (PFNtest) ca_xlib_present;
#endif

#if CAIRO_HAS_XCB_SURFACE & WITH_CAIRO_XCB
    ctx = (vgperf_context_t*)malloc(sizeof(vgperf_context_t));
    libs[ctxCount++] = ctx;

    ctx->libName = "cairo xcb";
    ctx->init = (PFNinitLibrary) ca_xcb_initLibrary;
    ctx->cleanup = (PFNcleanupLibrary) ca_xcb_cleanupLibrary;
    ctx->present = (PFNtest) ca_xcb_present;
#endif

#if WITH_CAIRO_IMAGE
    ctx = (vgperf_context_t*)malloc(sizeof(vgperf_context_t));
    libs[ctxCount++] = ctx;

    ctx->libName = "cairo img";
    ctx->init = (PFNinitLibrary) ca_image_initLibrary;
    ctx->cleanup = (PFNcleanupLibrary) ca_cleanupLibrary;
    ctx->present = NULL;
#endif

    for (int i=0; i<ctxCount; i++)
        init_cairo_test_context(libs[i]);

    return ctxCount;
}
