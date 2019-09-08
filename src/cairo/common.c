
#include "common.h"

#include "basic_tests.h"
//#include "test1.h"

#if defined(CAIRO_HAS_XCB_SURFACE) & defined(WITH_CAIRO_XCB)
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

#if defined(CAIRO_HAS_XLIB_SURFACE) & defined(WITH_CAIRO_XLIB)
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


#if defined(WITH_CAIRO_GLES) & defined(WITH_GLFW3)
static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}
library_context_t* ca_gles_initLibrary(options_t* opt) {
    library_context_t* ctx = (library_context_t*)calloc(1, sizeof(library_context_t));

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        errx (-1, "glfwInit failure for cairo egl\n");
    GLFWwindow* window;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API) ;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2 );
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);

    window = glfwCreateWindow(opt->width, opt->height, "cairo gles", NULL, NULL);

    if (!window){
        glfwTerminate();
        errx (-1, "Could not create window with glfw for cairo egl\n");
    }

    int result = glfwGetWindowAttrib(window, GLFW_CLIENT_API) ;

    EGLDisplay display = glfwGetEGLDisplay();
    EGLContext eglctx = glfwGetEGLContext(window);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);


    cairo_device_t* cairo_device = cairo_egl_device_create (display, eglctx);
    cairo_status_t status = cairo_device_status (cairo_device);
    if (status != CAIRO_STATUS_SUCCESS)
        errx (-1, "Could not create EGL device: (%d) %s\n", status, cairo_status_to_string (status) );

    cairo_gl_device_set_thread_aware (cairo_device, 0);
    ctx->surf = cairo_gl_surface_create_for_egl (cairo_device,
                                                     glfwGetEGLSurface(window),
                                                     opt->width, opt->height);

    cairo_device_destroy (cairo_device);

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    return ctx;

}
void ca_gles_cleanupLibrary (library_context_t* ctx) {

    cairo_surface_destroy (ctx->surf);

    glfwTerminate();
    free(ctx);
}
void ca_gles_cleanupTest (options_t* opt, library_context_t* ctx) {
    cairo_destroy(ctx->ctx);
    cairo_surface_flush (ctx->surf);
}
void ca_gles_present (options_t* opt, library_context_t* ctx) {
    cairo_gl_surface_swapbuffers (ctx->surf);
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

#if WITH_CAIRO_XLIB
    libs[ctxCount++] = vgperf_context_create("cairo xlib",
                                             (PFNinitLibrary) ca_xlib_initLibrary,
                                             (PFNcleanupLibrary) ca_xlib_cleanupLibrary,
                                             (PFNtest) ca_xlib_present);
#endif

#if WITH_CAIRO_XCB
    libs[ctxCount++] = vgperf_context_create("cairo xcb",
                                             (PFNinitLibrary) ca_xcb_initLibrary,
                                             (PFNcleanupLibrary) ca_xcb_cleanupLibrary,
                                             (PFNtest) ca_xcb_present);
#endif

#if WITH_CAIRO_IMAGE
    libs[ctxCount++] = vgperf_context_create("cairo img",
                                             (PFNinitLibrary) ca_image_initLibrary,
                                             (PFNcleanupLibrary) ca_cleanupLibrary,
                                             NULL);
#endif

#if defined(WITH_CAIRO_GLES) & defined(WITH_GLFW3)
    libs[ctxCount++] = vgperf_context_create("cairo es3",
                                             (PFNinitLibrary) ca_gles_initLibrary,
                                             (PFNcleanupLibrary) ca_gles_cleanupLibrary,
                                             (PFNtest) ca_gles_present);
#endif

    for (int i=0; i<ctxCount; i++)
        init_cairo_test_context(libs[i]);

    return ctxCount;
}
