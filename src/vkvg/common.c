
#include "common.h"
#include "vkh_device.h"
#include "vkh_presenter.h"

#include "basic_tests.h"

void randomize_color (VkvgContext ctx) {
    vkvg_set_source_rgba(ctx,
        (float)rnd()/RAND_MAX,
        (float)rnd()/RAND_MAX,
        (float)rnd()/RAND_MAX,
        (float)rnd()/RAND_MAX
    );
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

void present_surface (library_context_t* ctx) {
    glfwPollEvents();
    VkhPresenter r = ctx->vkEngine->renderer;
    if (!vkh_presenter_draw (r))
        vkh_presenter_build_blit_cmd (r, vkvg_surface_get_vk_image(ctx->surf));

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
        vkh_presenter_build_blit_cmd (r, vkvg_surface_get_vk_image(ctx->surf));

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
 * @brief clear surface and create vkvg context, run before each test occurence
 * @param options
 * @param library context
 */
void initTest(options_t* opt, library_context_t* ctx) {
    vkvg_surface_clear(ctx->surf);
    ctx->ctx = vkvg_create(ctx->surf);

    //cairo_set_antialias (ctx->ctx, CAIRO_ANTIALIAS_NONE);
    vkvg_set_line_width (ctx->ctx, opt->lineWidth);
}
/**
 * @brief cleanup Test, present surface on screen if requested. run after each test occurence.
 * @param options
 * @param library context
 */
void cleanupTest (options_t* opt, library_context_t* ctx) {
    vkvg_destroy(ctx->ctx);

    if (opt->present == 1)
        present_surface(ctx);
}
void saveImg (library_context_t* ctx, const char* fileName) {
    vkvg_surface_write_to_png(ctx->surf, fileName);
}
/**
 * @brief set library func pointers and register available tests.
 * @param ctx
 */
void init_vkvg_tests (test_context_t* ctx) {
    ctx->libName = "vkvg";
    ctx->init = initLibrary;
    ctx->cleanup = cleanupLibrary;
    ctx->saveImg = saveImg;
    ctx->testCount = 0;
    ctx->tests = (test_t*)malloc(0);

    addTest(ctx, "lines stroke", initTest, line_perform, cleanupTest);
    addTest(ctx, "test", initTest, rect_perform, cleanupTest);
    addTest(ctx, "rectangles", initTest, rectangles_perform, cleanupTest);
    addTest(ctx, "circles", initTest, circles_perform, cleanupTest);
    addTest(ctx, "stars", initTest, stars_perform, cleanupTest);
}
