
#include "common.h"
#include "vkh_device.h"
#include "vkh_presenter.h"

#include "basic_tests.h"
#include "test1.h"

void randomize_color (VkvgContext ctx) {
    vkvg_set_source_rgba(ctx,
        (float)rnd()/RAND_MAX,
        (float)rnd()/RAND_MAX,
        (float)rnd()/RAND_MAX,
        (float)rnd()/RAND_MAX
    );
}

void vkvg_draw_shape (shape_t shape, options_t *opt, library_context_t* ctx) {
    int w = opt->width;
    int h = opt->height;

    float x, y, z, v;

    randomize_color (ctx->ctx);

    switch (shape) {
    case SHAPE_LINE:
        x = (float)rnd()/RAND_MAX * w;
        y = (float)rnd()/RAND_MAX * h;
        z = (float)rnd()/RAND_MAX * w;
        v = (float)rnd()/RAND_MAX * h;

        vkvg_move_to(ctx->ctx, x, y);
        vkvg_line_to(ctx->ctx, z, v);
        vkvg_stroke(ctx->ctx);
        break;
    case SHAPE_RECTANGLE:
        x = trunc( (0.5*(float)opt->width*rnd())/RAND_MAX );
        y = trunc( (0.5*(float)opt->height*rnd())/RAND_MAX );
        z = trunc( (0.5*(float)opt->width*rnd())/RAND_MAX ) + 1;
        v = trunc( (0.5*(float)opt->height*rnd())/RAND_MAX ) + 1;

        vkvg_rectangle(ctx->ctx, x+1, y+1, z, v);

        vkvg_draw(opt->drawMode, ctx->ctx);
        break;
    case SHAPE_ROUNDED_RECTANGLE:
        break;
    case SHAPE_CIRCLE:
        x = (float)rnd()/RAND_MAX * w;
        y = (float)rnd()/RAND_MAX * h;
        v = (float)rnd()/RAND_MAX * MIN(w,h) / 2.0;

        vkvg_arc(ctx->ctx, x, y, v, 0, M_PI * 2.0);

        vkvg_draw(opt->drawMode,ctx->ctx);
        break;
    case SHAPE_TRIANGLE:
        break;
    case SHAPE_STAR:
        x = (float)rnd()/RAND_MAX * w;
        y = (float)rnd()/RAND_MAX * h;
        z = (float)rnd()/RAND_MAX *0.5 + 0.15; //scale

        vkvg_move_to (ctx->ctx, x+star_points[0][0]*z, y+star_points[0][1]*z);
        for (int s=1; s<11; s++)
            vkvg_line_to (ctx->ctx, x+star_points[s][0]*z, y+star_points[s][1]*z);
        vkvg_close_path (ctx->ctx);

        vkvg_draw(opt->drawMode,ctx->ctx);
        break;
    case SHAPE_RANDOM:
        break;
    }
}

void vkvg_draw (draw_mode_t drawMode, VkvgContext ctx) {
    switch (drawMode) {
    case DM_BOTH:
        vkvg_fill_preserve(ctx);
        randomize_color(ctx);
        vkvg_stroke(ctx);
        break;
    case DM_FILL:
        vkvg_fill(ctx);
        break;
    case DM_STROKE:
        vkvg_stroke(ctx);
        break;
    }
}

void vkvg_present (options_t* opt, library_context_t* ctx) {
    vkvg_flush (ctx->ctx);
    VkhPresenter r = ctx->vkEngine->renderer;
    glfwPollEvents();
    if (!vkh_presenter_draw (r))
        vkh_presenter_build_blit_cmd (r, vkvg_surface_get_vk_image(ctx->surf), opt->width, opt->height);
    vkDeviceWaitIdle(r->dev->dev);
}

/**
 * @brief init library once at startup
 * @param global options
 * @return a pointer to the library test context
 */
library_context_t* initLibrary(options_t* opt) {
    library_context_t* ctx = (library_context_t*)calloc(1, sizeof(library_context_t));

    ctx->vkEngine = vkengine_create (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, opt->width, opt->height);
    VkhPresenter r = ctx->vkEngine->renderer;

    ctx->dev = vkvg_device_create (r->dev->phy, r->dev->dev, r->qFam, 0);

    ctx->surf = vkvg_surface_create (ctx->dev, opt->width, opt->height);

    if (opt->present == 1)
        vkh_presenter_build_blit_cmd (r, vkvg_surface_get_vk_image(ctx->surf), opt->width, opt->height);

    return ctx;
}
/**
 * @brief cleanup Library and release test context pointer
 * @param library test context
 */
void cleanupLibrary (library_context_t* ctx) {
    vkvg_surface_destroy(ctx->surf);

    vkvg_device_destroy (ctx->dev);

    vkengine_destroy (ctx->vkEngine);

    free(ctx);
}
/**
 * @brief clear surface and create and setup vkvg context, run before each test occurence
 * @param global options
 * @param library context
 */
void initTest(options_t* opt, library_context_t* ctx) {
    vkvg_surface_clear(ctx->surf);
    ctx->ctx = vkvg_create(ctx->surf);

    //cairo_set_antialias (ctx->ctx, CAIRO_ANTIALIAS_NONE);
    vkvg_set_line_width (ctx->ctx, opt->lineWidth);

    switch (opt->capStyle) {
    case LINE_CAP_BUTT:
        vkvg_set_line_cap(ctx->ctx, VKVG_LINE_CAP_BUTT);
        break;
    case LINE_CAP_ROUND:
        vkvg_set_line_cap(ctx->ctx, VKVG_LINE_CAP_ROUND);
        break;
    case LINE_CAP_SQUARE:
        vkvg_set_line_cap(ctx->ctx, VKVG_LINE_CAP_SQUARE);
        break;
    }
    switch (opt->joinStyle) {
    case LINE_JOIN_MITER:
        vkvg_set_line_join(ctx->ctx, VKVG_LINE_JOIN_MITER);
        break;
    case LINE_JOIN_BEVEL:
        vkvg_set_line_join(ctx->ctx, VKVG_LINE_JOIN_BEVEL);
        break;
    case LINE_JOIN_ROUND:
        vkvg_set_line_join(ctx->ctx, VKVG_LINE_JOIN_ROUND);
        break;
    }

}
/**
 * @brief cleanup Test, present surface on screen if requested. run after each test occurence.
 * @param global options
 * @param library context
 */
void cleanupTest (options_t* opt, library_context_t* ctx) {
    vkvg_destroy(ctx->ctx);
}
void saveImg (library_context_t* ctx, const char* fileName) {
    vkvg_surface_write_to_png(ctx->surf, fileName);
}
/**
 * @brief set library func pointers and register available tests.
 * @param ctx
 */
void init_vkvg_tests (vgperf_context_t* ctx) {
    ctx->libName    = "vkvg";
    ctx->init       = (PFNinitLibrary) initLibrary;
    ctx->cleanup    = (PFNcleanupLibrary) cleanupLibrary;
    ctx->saveImg    = (PFNSaveImg) saveImg;
    ctx->present    = (PFNtest) vkvg_present;

    ctx->testCount  = 0;
    ctx->tests      = (test_t*)malloc(0);

    addTest (ctx, "lines stroke", initTest, line_perform, cleanupTest);
    addTest (ctx, "rectangles", initTest, rectangles_perform, cleanupTest);
    addTest (ctx, "circles", initTest, circles_perform, cleanupTest);
    addTest (ctx, "stars", initTest, stars_perform, cleanupTest);
    addTest (ctx, "test1", NULL, vkvg_test1_perform, NULL);
    //addTest(ctx, "single poly", initTest, single_poly_perform, cleanupTest);
}
